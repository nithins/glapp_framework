#include <iostream>
#include <algorithm>
#include <cfloat>

#include <GL/glu.h>

#include <vecutils.h>
#include <glutils.h>


#include <modelcontroller.h>
#include <input.h>
#include <quat.h>
#include <framework.h>

#ifndef PI
#define PI 3.14159265
#endif

#ifndef SQUARE
#define SQUARE(x) ((x)*(x))
#endif

using namespace std;

void matMul4x4 ( const double *m1, const double *m2, double * out )
{
  float tmp_out[16];

  for ( int i = 0 ; i < 4 ;i++ )
  {
    for ( int j = 0 ; j < 4 ;j++ )
    {
      tmp_out[i*4+j] = 0.0;

      for ( int k = 0 ; k < 4 ;k++ )
      {
        tmp_out[i*4+j] += m1[i*4+k] * m2[k*4+j];
      }
    }
  }

  copy ( tmp_out, tmp_out + 16, out );

}


class TModelController: public virtual IModelController
{

  // state of the controller
    float m_size;
    float m_pos[3];
    Quat  m_rot;

  // temporrary state of the controller

    double m_mvm[16];
    double m_pm[16];
    int    m_vp[4];

    bool   m_bRotationActive;
    bool   m_bTranslationActive;

    double m_firstTBVec[3];
    double m_secondTBVec[3];




  public:


    virtual void serialize(ostream &o)
    {
      float q[4];
      m_rot.get_data(q);

      o<<m_size<<" "
       <<q[0]<<" "
       <<q[1]<<" "
       <<q[2]<<" "
       <<q[3]<<" "
       <<m_pos[0]<<" "
       <<m_pos[1]<<" "
       <<m_pos[2]<<" "
       <<endl;
    }
    virtual void serialize(istream &i)
    {
      float q[4];

      i>>m_size
       >>q[0]
       >>q[1]
       >>q[2]
       >>q[3]
       >>m_pos[0]
       >>m_pos[1]
       >>m_pos[2];

      m_rot = Quat(q);
    }

    virtual void set_pos ( const float *v )
    {
      copy ( v, v + 3, m_pos );
    }

    TModelController ( ) :
        m_bRotationActive ( false ),
        m_bTranslationActive ( false )
    {
      m_size = 1;

      m_pos[0] = 0;
      m_pos[1] = 0;
      m_pos[2] = 0;
    }

    ~TModelController()
    {
    }

    virtual void set_uniform_scale(double s)
    {
      m_size = s;
    }

    virtual double get_uniform_scale()
    {
      return m_size;
    }


    virtual void Reset()
    {
      m_rot.Reset();

      SETVEC3 ( m_pos, 0, 0, 0 );

      m_size = 1;
    }

    virtual void Render() const
    {
      double mat[16];

      SaveTBData();

      if ( m_bTranslationActive == true )
      {
        GetTempTranslateMat ( mat );
        glMultMatrixd ( mat );
      }

      GetCompositeMatrix ( mat );

      glMultMatrixd ( mat );

      if ( m_bRotationActive == true )
      {
//        DrawTB();

        Quat q = GetTempQuat();

        q.GetMat ( mat );
        glMultMatrixd ( mat );
      }

      return;
    }

    virtual void getMatrix ( double *mat )
    {
      GetCompositeMatrix ( mat );
    };

    Quat GetTempQuat() const
    {
      float dp = DOTPROD3 ( m_firstTBVec, m_secondTBVec );

      if ( fabs ( dp ) < 1.0 )
      {

        float v[3];
        CROSSPROD3 ( v, m_secondTBVec, m_firstTBVec );

        return Quat ( acos ( dp ), v );

      }
      else
      {
        return Quat();
      }
    }


#define TBSANITYFACTOR 0.98
    void DrawTB() const
    {

      GLfloat lineWidth;

      glPushMatrix();
      glPushAttrib ( GL_ENABLE_BIT );

      glDisable ( GL_LIGHTING );

      glScalef ( TBSANITYFACTOR, TBSANITYFACTOR, TBSANITYFACTOR );

      glGetFloatv ( GL_LINE_WIDTH, &lineWidth );
      glLineWidth ( 2.0 );

      glColor3f ( 0.6, 0.45, 0.6 );

      const float thetastep = .1 ;// radians

      float tdata[4];
      GetTempQuat().get_data ( tdata );
      float theta = acos ( tdata[3] ) * 2 - thetastep;


      float pt[4];
      COPYVEC3 ( pt, m_firstTBVec );
      pt[3] = 1.0;

      glBegin ( GL_LINE_STRIP );

      for ( float angle = 0 ; angle < theta; angle += thetastep )
      {
        glVertex3fv ( pt );
        Quat r ( thetastep, tdata );
        ( r*Quat ( pt ) * ( -r ) ).get_data ( pt );
        glVertex3fv ( pt );
        glVertex3f ( 0, 0, 0 );

      }

      glVertex3fv ( pt );

      glVertex3dv ( m_secondTBVec );

      glEnd();


      glColor3f ( 0.3, 0.3, 0.6 );
      glBegin ( GL_LINES );

      glVertex3f ( 0, 0, 0 );
      glVertex3dv ( m_firstTBVec );

      glVertex3f ( 0, 0, 0 );
      glVertex3dv ( m_secondTBVec );

      NORMALIZEVEC3_ ( GLfloat, tdata );

      glVertex3f ( COMMAVEC3 ( tdata ) );
      glVertex3f ( COMMAVEC3 ( -tdata ) );

      glEnd();

      glPopAttrib();
      glPopMatrix();

      glLineWidth ( lineWidth );
    }

    void GetCompositeMatrix ( double *mat ) const
    {
      m_rot.GetMat ( mat );

      for(int i = 0 ; i < 12 ; ++i)
      {
        mat[i] *= m_size;
      }

      copy ( m_pos, m_pos + 3, mat + 12 );
    }

    void GetTempTranslateMat ( double *mat ) const
    {
      mat[0] = 1;
      mat[1] = 0;
      mat[2] = 0;
      mat[3] = 0;

      mat[4] = 0;
      mat[5] = 1;
      mat[6] = 0;
      mat[7] = 0;

      mat[8] = 0;
      mat[9] = 0;
      mat[10] = 1;
      mat[11] = 0;

      double x_dir[3] = {m_mvm[0], m_mvm[4], m_mvm[8]};
      double y_dir[3] = {m_mvm[1], m_mvm[5], m_mvm[9]};

      mat[12] = ( m_secondTBVec[0] - m_firstTBVec[0] ) * x_dir[0] + ( m_secondTBVec[1] - m_firstTBVec[1] ) * y_dir[0];
      mat[13] = ( m_secondTBVec[0] - m_firstTBVec[0] ) * x_dir[1] + ( m_secondTBVec[1] - m_firstTBVec[1] ) * y_dir[1];
      mat[14] = ( m_secondTBVec[0] - m_firstTBVec[0] ) * x_dir[2] + ( m_secondTBVec[1] - m_firstTBVec[1] ) * y_dir[2];
      mat[15] = 1;
    }



    void SaveTBData() const
    {
      TModelController * t = const_cast<TModelController*> ( this );

      // put away the matrices
      glGetDoublev ( GL_MODELVIEW_MATRIX, t->m_mvm );
      glGetDoublev ( GL_PROJECTION_MATRIX, t->m_pm );
      glGetIntegerv ( GL_VIEWPORT, t->m_vp );


    }

    void CalcPtOnTB ( const int &x, const int &y, double *w )
    {

      // intersect the infinite line along xy with the TB sphere..
      // sphere rad = .8  more than this things goes mad

      double mvm[16], mat[16];
      GetCompositeMatrix ( mat );
      matMul4x4 ( mat, m_mvm, mvm );


      int ry = m_vp[3] - y - 1;

      double p1[3], p2[3];
      gluUnProject ( ( GLdouble ) x, ( GLdouble ) ry, 1, mvm, m_pm, m_vp, p1, p1 + 1, p1 + 2 );
      gluUnProject ( ( GLdouble ) x, ( GLdouble ) ry, 0, mvm, m_pm, m_vp, p2, p2 + 1, p2 + 2 );

      double a = ( ( p2[0] - p1[0] ) * ( p2[0] - p1[0] ) +
                   ( p2[1] - p1[1] ) * ( p2[1] - p1[1] ) +
                   ( p2[2] - p1[2] ) * ( p2[2] - p1[2] ) ) ;

      double b = 2 * ( ( p2[0] - p1[0] ) * ( p1[0] ) + ( p2[1] - p1[1] ) * ( p1[1] ) + ( p2[2] - p1[2] ) * ( p1[2] ) );

      double c = p1[0] * p1[0] + p1[1] * p1[1] + p1[2] * p1[2] - TBSANITYFACTOR * TBSANITYFACTOR  ;

      double d = b * b - 4 * a * c;

      if ( d < 0 )
      {
        d = 0;
      }

      double u = ( -b + sqrt ( d ) ) / ( 2 * a );


      w[0] = p1[0] + u * ( p2[0] - p1[0] );
      w[1] = p1[1] + u * ( p2[1] - p1[1] );
      w[2] = p1[2] + u * ( p2[2] - p1[2] );

      NORMALIZEVEC3_( double, w );
    }

    bool look_at ( const float *e, const float *c, const float * u )
    {
      float x_dir[3];
      float y_dir[] = {u[0], u[1], u[2]};
      float z_dir[] = {e[0]-  c[0], e[1] - c[1], e[2] - c[2]};

      NORMALIZEVEC3_( float, y_dir );
      NORMALIZEVEC3_( float, z_dir );

      if ( fabs ( DOTPROD3 ( y_dir, z_dir ) ) > 0.99 )
      {
        _LOG("unable to determine lookat");
        return false;
      }

      CROSSPROD3 ( x_dir, y_dir, z_dir );

      CROSSPROD3 ( y_dir, z_dir, x_dir );

      NORMALIZEVEC3_ ( float, x_dir );
      NORMALIZEVEC3_ ( float, y_dir );

      // form the orthonormal 3x3 matrix

      float mat[3][3] =
      { {COMMAVEC3 ( x_dir ) },
        {COMMAVEC3 ( y_dir ) },
        {COMMAVEC3 ( z_dir ) }
      };

      int mi = 0;

      if ( mat[mi][mi] < mat[1][1] )
        mi = 1;

      if ( mat[mi][mi] < mat[2][2] )
        mi = 2;

      float r = sqrt
                ( 1.0 +
                  mat[mi][mi]  -
                  mat[ ( mi+1 ) %3][ ( mi+1 ) %3]  -
                  mat[ ( mi+2 ) %3][ ( mi+2 ) %3]  );

      float q[4];

      q[3]        = ( mat[ ( mi+2 ) %3][ ( mi+1 ) %3] -
                      mat[ ( mi+1 ) %3][ ( mi+2 ) %3] ) / ( 2.0 * r );

      q[mi]       = r / 2.0;

      q[ ( mi+1 ) %3] = ( mat[ mi][ ( mi+1 ) %3] +
                          mat[ ( mi+1 ) %3][ mi] ) / ( 2.0 * r );

      q[ ( mi+2 ) %3] = ( mat[ mi][ ( mi+2 ) %3] +
                          mat[ ( mi+2 ) %3][ mi] ) / ( 2.0 * r );

      float mag = sqrt(SQUARE(q[0]) + SQUARE(q[1])+ SQUARE(q[2])+ SQUARE(q[3]));

      q[0] /=mag;
      q[1] /=mag;
      q[2] /=mag;
      q[3] /=mag;

      m_rot = Quat ( q );

      copy ( e, e + 3, m_pos );

      return true;
    }

    void CalcPtOnXYPlane ( const int &x, const int &y, double *w ) const
    {

      double mvm[16], mat[16];
      GetCompositeMatrix ( mat );
      matMul4x4 ( mat, m_mvm, mvm );

      mvm[0] = 1;
      mvm[1] = 0;
      mvm[2] = 0;
      //mvm[3] = 0;

      mvm[4] = 0;
      mvm[5] = 1;
      mvm[6] = 0;
      //mvm[7] = 0;

      mvm[8]  = 0;
      mvm[9]  = 0;
      mvm[10] = 1;
      //mvm[11] = 0;

      //copy ( m_pos, m_pos + 3, mvm + 12 );



      // this is the real y
      int ry = m_vp[3] - y - 1;

      double p1[3], p2[3];
      gluUnProject ( ( GLdouble ) x, ( GLdouble ) ry, 1, mvm, m_pm, m_vp, p1, p1 + 1, p1 + 2 );
      gluUnProject ( ( GLdouble ) x, ( GLdouble ) ry, 0, mvm, m_pm, m_vp, p2, p2 + 1, p2 + 2 );

      double u = ( p1[2] - 0 ) / ( p1[2] - p2[2] );


      w[0] = p1[0] + u * ( p2[0] - p1[0] );
      w[1] = p1[1] + u * ( p2[1] - p1[1] );
      w[2] = p1[2] + u * ( p2[2] - p1[2] );

    }

    virtual void StartTB ( const uint &x, const uint & y )
    {
      CalcPtOnTB ( x, y, m_firstTBVec );
      COPYVEC3 ( m_secondTBVec, m_firstTBVec );
      m_bRotationActive    = true;
      m_bTranslationActive = false;
    }

    virtual void StopTB ( const uint &x, const uint & y )
    {
      CalcPtOnTB ( x, y, m_secondTBVec );
      m_rot *= GetTempQuat();
      m_bRotationActive = false;
    }

    virtual void StartTrans ( const uint &x, const uint &y )
    {
      CalcPtOnXYPlane ( x, y, m_firstTBVec );
      COPYVEC3 ( m_secondTBVec, m_firstTBVec );
      m_bTranslationActive = true;
      m_bRotationActive = false;

    }

    virtual void StopTrans ( const uint &x, const uint &y )
    {
      CalcPtOnXYPlane ( x, y, m_secondTBVec );

      float x_dir[3] = {m_mvm[0], m_mvm[4], m_mvm[8]};
      float y_dir[3] = {m_mvm[1], m_mvm[5], m_mvm[9]};

      m_pos[0] += ( m_secondTBVec[0] - m_firstTBVec[0] ) * x_dir[0] + ( m_secondTBVec[1] - m_firstTBVec[1] ) * y_dir[0];
      m_pos[1] += ( m_secondTBVec[0] - m_firstTBVec[0] ) * x_dir[1] + ( m_secondTBVec[1] - m_firstTBVec[1] ) * y_dir[1];
      m_pos[2] += ( m_secondTBVec[0] - m_firstTBVec[0] ) * x_dir[2] + ( m_secondTBVec[1] - m_firstTBVec[1] ) * y_dir[2];

      m_bTranslationActive = false;

    }

    virtual void Move ( const uint &x, const uint & y )
    {
      if ( m_bRotationActive == true )
      {
        CalcPtOnTB ( x, y, m_secondTBVec );
      }

      if ( m_bTranslationActive == true )
      {
        CalcPtOnXYPlane ( x, y, m_secondTBVec );
      }
    }
};

IModelController* IModelController::Create ( )
{
  return new TModelController ();
}

void  IModelController::Delete ( IModelController* c )
{
  delete c;
}



