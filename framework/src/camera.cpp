#include <iostream>

#include <GL/glu.h>

#include <camera.h>
#include <glutils.h>



class CameraImp: public Camera
{

  private:

    // orthogonal vectors
    GLfloat m_u[3];
    GLfloat m_r[3];
    GLfloat m_f[3];

    // focal point, cam pos and dist (redundant)
    GLfloat m_c[3];
    GLfloat m_p[3];
    GLfloat m_d;

  public:

    CameraImp()
    {
      Reset();
    }

    virtual ~CameraImp()
    {}

    void DrawFocus()
    {
      glPushMatrix();

      glScalef(0.1,0.1,0.1);

      DrawAxes();

      glPopMatrix();
    }


    void SetPerspective ()
    {
      /* get info about viewport (x, y, w, h) */
      GLint viewport[4];
      glGetIntegerv ( GL_VIEWPORT, viewport );

      /* set camera to view viewport area */
      glMatrixMode ( GL_PROJECTION );

      /* view scene in perspective */
      gluPerspective ( 30.0, ( GLfloat ) ( viewport[2] ) / ( GLfloat ) ( viewport[3] ), 0.1, 500.0 );
      /* prepare to work with model again */
      glMatrixMode ( GL_MODELVIEW );
    }

    void SetOrthographic ()
    {
      /* get info about viewport (x, y, w, h) */
      GLint viewport[4];
      glGetIntegerv ( GL_VIEWPORT, viewport );

      /* set camera to view viewport area */
      glMatrixMode ( GL_PROJECTION );

      /* view scene in orthographic */

      glOrtho ( -2, 2, -2, 2, 0.1, 500.0 );

      /* prepare to work with model again */
      glMatrixMode ( GL_MODELVIEW );
    }

#define CAM_MOV_CONST 0.05

    void Set ()
    {
      gluLookAt ( m_p[0], m_p[1], m_p[2],
                  m_c[0], m_c[1], m_c[2],
                  m_u[0], m_u[1], m_u[2] );

    }

    void MoveLR ( int dir )
    {

      // f = norm((f->) +k(r->))
      GLfloat t[3];
      MULVECSCL3 ( t, m_r, dir*CAM_MOV_CONST*0.1 );
      ADDVECVEC3 ( m_f, m_f, t );
      NORMALIZEVEC3 ( GLfloat, m_f, m_f );

      // p = c-d*(f->)
      MULVECSCL3 ( t, m_f, m_d );
      SUBVECVEC3 ( m_p, m_c, t );

      // (r->) = (u->)x(f->)
      CROSSPROD3 ( m_r, m_u, m_f );
    }

    void MoveUD ( int dir )
    {

      // (f->) = norm((f->) +k(u->))
      GLfloat t[3];
      MULVECSCL3 ( t, m_u, dir*CAM_MOV_CONST*0.1 );
      ADDVECVEC3 ( m_f, m_f, t );
      NORMALIZEVEC3 ( GLfloat, m_f, m_f );

      // p = c-d*(f->)
      MULVECSCL3 ( t, m_f, m_d );
      SUBVECVEC3 ( m_p, m_c, t );

      // (u->) = (f->)x(r->)
      CROSSPROD3 ( m_u, m_f, m_r );

    }

    void MoveFB ( int dir )
    {

      GLfloat t[3];

      // d = (1+dir*k)*d
      m_d *= ( 1.0 + ( ( dir == 1 ) ? -CAM_MOV_CONST : CAM_MOV_CONST ) );

      // p = c-d*(f->)
      MULVECSCL3 ( t, m_f, m_d );
      SUBVECVEC3 ( m_p, m_c, t );
    }

    void StrafeLR ( int dir )
    {
      // p = p + k*(r->)
      // c = c + k*(r->)

      GLfloat t[3];
      MULVECSCL3 ( t, m_r, ( dir == 1 ) ? CAM_MOV_CONST : -CAM_MOV_CONST );
      ADDVECVEC3 ( m_p, m_p, t );
      ADDVECVEC3 ( m_c, m_c, t );
    }

    void StrafeFB ( int dir )
    {
      // p = p + k*(f->)
      // c = c + k*(f->)

      GLfloat t[3];
      MULVECSCL3 ( t, m_f, ( dir == 1 ) ? CAM_MOV_CONST : -CAM_MOV_CONST );
      ADDVECVEC3 ( m_p, m_p, t );
      ADDVECVEC3 ( m_c, m_c, t );
    }

    void StrafeUD ( int dir )
    {
      // p = p + k*(u->)
      // c = c + k*(u->)

      GLfloat t[3];
      MULVECSCL3 ( t, m_u, ( dir == 1 ) ? CAM_MOV_CONST : -CAM_MOV_CONST );
      ADDVECVEC3 ( m_p, m_p, t );
      ADDVECVEC3 ( m_c, m_c, t );
    }

    void Twist ( int dir )
    {
      // (u->) = norm((u->) + k*(r->))
      GLfloat t[3];
      MULVECSCL3 ( t, m_r, ( dir == 1 ) ? CAM_MOV_CONST : -CAM_MOV_CONST );
      ADDVECVEC3 ( m_u, m_u, t );
      NORMALIZEVEC3 ( GLfloat, m_u, m_u );

      // (r->) = (u->)x(f->)
      CROSSPROD3 ( m_r, m_u, m_f );

    }

    void Reset()
    {
      // orthogonal vectors
      SETVEC3 ( m_u, 0, 1, 0 );
      SETVEC3 ( m_r, -1, 0, 0 );
      SETVEC3 ( m_f, 0, 0, -1 );

      // focal point, cam pos and dist (redundant)
      SETVEC3 ( m_c, 0, 0, 0 );
      SETVEC3 ( m_p, 0, 0, 5 );
      m_d = 5;
    }
};

Camera * Camera::Create()
{
  return new CameraImp();
}

void Camera::Delete ( Camera *&p )
{
  if ( p != NULL )
  {
    delete p;
  }

  p = NULL;
}
