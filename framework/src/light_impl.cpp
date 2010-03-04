#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>

#include <GL/glu.h>

#include <light_impl.h>
#include <input.h>
#include <modelcontroller.h>
#include <vecutils.h>
#include <glutils.h>
#include <framework.h>


using namespace std;

const double g_light_model_grow_factor = 0.1;

class TLight: virtual public IModel, virtual public IHandleInput
{
public:
  enum eLightType { LIGHT_TYPE_DIRECTIONAL,
                    LIGHT_TYPE_POSITIONAL,
                    LIGHT_TYPE_SPOT};
private:

  GLfloat m_specular[4];
  GLfloat m_ambient [4];
  GLfloat m_diffuse [4];

  GLfloat m_position[4];
  GLfloat m_direction [4];

  GLfloat m_attenuation[3];

  GLfloat m_spot_angle;
  GLfloat m_spot_exponent;

  GLenum  m_lightNo;

  bool    m_enabled;

  bool    m_light_params_updated;

  bool    m_light_posdir_updated;

  bool    m_render_graphic;

  eLightType m_light_type;

  string m_name;

  IModelController * m_controller;

public:

  void set_specular ( const float *v )
  {
    copy ( v, v + 4, m_specular );
    m_light_params_updated = true;
  }

  void set_ambient ( const float *v )
  {
    copy ( v, v + 4, m_ambient );
    m_light_params_updated = true;
  }

  void set_diffuse ( const float *v )
  {
    copy ( v, v + 4, m_diffuse );
    m_light_params_updated = true;
  }

  void set_attenuation ( const float *v )
  {
    copy ( v, v + 3, m_attenuation );
    m_light_params_updated = true;
  }

  void set_enabled ( const bool &e )
  {
    m_enabled = e;
    m_light_params_updated = true;
  }

  void set_position ( const float *pos )
  {
    copy ( pos, pos + 3, m_position );
    m_position[3] = 1.0;
    m_light_posdir_updated = true;
  }

  void set_direction ( const float *dir )
  {
    copy ( dir, dir + 3, m_direction );
    m_direction[3] = 0.0;
    m_light_posdir_updated = true;
  }

  void set_spot_params ( const float &angle, const float & exponent )
  {
    m_spot_angle    = angle;
    m_spot_exponent = exponent;
    m_light_params_updated = true;
  }

  void set_light_type ( const eLightType &t )
  {
    m_light_type = t;
    m_light_params_updated = true;
  }

  void set_render_graphic(const bool & b)
  {
    m_render_graphic = b;
  }

  void get_specular (  float *v ) const
  {
    copy ( m_specular, m_specular + 4, v );
  }

  void get_ambient (  float *v ) const
  {
    copy ( m_ambient, m_ambient + 4, v );
  }

  void get_diffuse (  float *v ) const
  {
    copy ( m_diffuse, m_diffuse + 4, v );
  }

  void get_attenuation (  float *v ) const
  {
    copy ( m_specular, m_specular + 3, v );
  }

  bool is_enabled () const
  {
    return m_enabled;
  }

  void get_position (  float *pos ) const
  {
    copy ( m_position, m_position + 3, pos );
  }

  void get_direction (  float *dir ) const
  {
    copy ( m_direction, m_direction + 3, dir );
  }

  void get_spot_params (  float &angle,  float & exponent ) const
  {
    angle    = m_spot_angle;
    exponent = m_spot_exponent;
  }

  eLightType get_light_type() const
  {
    return m_light_type;
  }

  bool get_render_graphic()
  {
    return m_render_graphic ;
  }

  // convenience functions

  inline void set_specular
      ( const float &v1, const float &v2, const float &v3, const float &v4 )
  {
    float v[] = {v1, v2, v3, v4};
    set_specular ( v );
  }

  inline void set_ambient
      ( const float &v1, const float &v2, const float &v3, const float &v4 )
  {
    float v[] = {v1, v2, v3, v4};
    set_ambient ( v );
  }

  inline void set_diffuse
      ( const float &v1, const float &v2, const float &v3, const float &v4 )
  {
    float v[] = {v1, v2, v3, v4};
    set_diffuse ( v );
  }

  inline void set_attenuation
      ( const float &v1, const float &v2, const float &v3 )
  {
    float v[] = {v1, v2, v3};
    set_attenuation ( v );
  }

  inline void set_position
      ( const float &v1, const float &v2, const float &v3 )
  {
    float v[] = {v1, v2, v3};
    set_position ( v );
  }

  inline void set_direction
      ( const float &v1, const float &v2, const float &v3 )
  {
    float v[] = {v1, v2, v3};
    set_direction ( v );
  }

  TLight ( GLenum l ) : m_lightNo ( l ),
  m_light_params_updated ( false ),
  m_light_posdir_updated(false)

  {
    m_controller = IModelController::Create();

    this->set_specular ( .25, .25, .25, 1.0f );
    this->set_ambient ( 0.2f, 0.2f, 0.2f, 1.0f );
    this->set_diffuse ( 0.75f, 0.75f, 0.75f, 0.75f );

    int lno   = l-GL_LIGHT0;
    int lno_2 = lno>>1;
    int lno_4 = lno>>2;

    this->set_attenuation ( 1, 0, 0 );
    this->set_direction
        ( -1*pow(-1,lno%2),-1*pow(-1,lno_4%2),-1*pow(-1,lno_2%2));
    this->set_position
        ( pow(-1,lno%2),pow(-1,lno_4%2),pow(-1,lno_2%2));

    this->set_enabled ( true );
    this->set_spot_params ( 20, 1 );
    this->set_light_type ( (eLightType) (lno %2) );
    this->set_render_graphic(true);

    stringstream ss;

    ss<<"Light no "<<lno;

    m_name = ss.str();

  }

  std::string Name() const
  {
    return m_name;
  }

  virtual ~TLight()
  {
    IModelController::Delete ( m_controller );
  }

  void setup_controller()
  {

    m_controller->set_uniform_scale(0.1);

    switch ( m_light_type )
    {

    case LIGHT_TYPE_SPOT:
      {

        m_controller->look_at
            ( m_position[0], m_position[1], m_position[2],
              m_position[0]+m_direction[0],
              m_position[1]+m_direction[1],
              m_position[2]+m_direction[2],
              0,1,0 );

        break;
      }


    case LIGHT_TYPE_POSITIONAL:
      {

        m_controller->set_pos ( m_position );

        break;
      }

    case LIGHT_TYPE_DIRECTIONAL:
      {

        if ( m_controller->look_at
             ( -m_direction[0], -m_direction[1], -m_direction[2],
               0, 0, 0,
               0, 1, 0 ) == false )

          m_controller->look_at
              ( -m_direction[0], -m_direction[1], -m_direction[2],
                0, 0, 0,
                1, 0, 0 );

        break;
      }
    };

  }

  virtual int Render() const
  {
    if ( m_light_posdir_updated == true )
    {
      TLight * l = const_cast<TLight *> ( this );

      l->setup_controller();

      l->m_light_posdir_updated = false;
    }

    if ( m_light_params_updated == true )
    {
      TLight * l = const_cast<TLight *> ( this );

      l->m_light_params_updated = false;

      render_light_params();
    }
    m_controller->Render();

    render_light_posdir();

    if(m_render_graphic == false)
      return 0;

    if(m_enabled)
      glColor3f(0.75,0.75,0.0);
    else
      glColor3f ( 0.5, 0.5, 0.5 );

    switch ( m_light_type )
    {
    case LIGHT_TYPE_POSITIONAL:render_positional_bulb();break;
    case LIGHT_TYPE_DIRECTIONAL:render_directional_bulb();break;
    case LIGHT_TYPE_SPOT:render_spot_bulb();break;
    };

    return 0;
  }


  void render_light_posdir() const
  {
    float direction[] = {0, 0, 1, 0};
    float position[] = {0.0, 0.0, 0.0, 1.0};

    switch ( m_light_type )
    {
    case LIGHT_TYPE_DIRECTIONAL:
      glLightfv ( m_lightNo, GL_POSITION, direction );
      break;

    case LIGHT_TYPE_SPOT:
      glLightfv ( m_lightNo, GL_SPOT_DIRECTION, direction );
      glLightf ( m_lightNo, GL_SPOT_CUTOFF, m_spot_angle );
      glLightf ( m_lightNo, GL_SPOT_EXPONENT, m_spot_exponent );
      glLightfv ( m_lightNo, GL_POSITION, position );
      break;

    case LIGHT_TYPE_POSITIONAL:
      glLightfv ( m_lightNo, GL_POSITION, position );
      glLightf ( m_lightNo, GL_SPOT_CUTOFF, 180 );
      glLightf ( m_lightNo, GL_SPOT_EXPONENT, 0 );
      break;
    };
  }

  void render_light_params() const
  {

    if ( is_enabled() == false )
    {
      float zero_vec[] = {0.0,0.0,0.0,1.0};

      glLightfv ( m_lightNo, GL_AMBIENT, zero_vec );

      glLightfv ( m_lightNo, GL_SPECULAR, zero_vec );
      glLightfv ( m_lightNo, GL_DIFFUSE, zero_vec );

      glLightf ( m_lightNo, GL_CONSTANT_ATTENUATION, 1.0);
      glLightf ( m_lightNo, GL_LINEAR_ATTENUATION, 0.0 );
      glLightf ( m_lightNo, GL_QUADRATIC_ATTENUATION, 0.0);
      glEnable ( m_lightNo );

      glDisable ( m_lightNo );
      return;
    }
    else
    {

      glLightfv ( m_lightNo, GL_AMBIENT, m_ambient );

      glLightfv ( m_lightNo, GL_SPECULAR, m_specular );
      glLightfv ( m_lightNo, GL_DIFFUSE, m_diffuse );

      glLightf ( m_lightNo, GL_CONSTANT_ATTENUATION, m_attenuation[0] );
      glLightf ( m_lightNo, GL_LINEAR_ATTENUATION, m_attenuation[1] );
      glLightf ( m_lightNo, GL_QUADRATIC_ATTENUATION, m_attenuation[2] );
      glEnable ( m_lightNo );
    }
  }

  void render_spot_bulb() const
  {
    glRotatef ( 180.0, 1.0, 0.0, 0.0 );
    GLUquadric* quadric = gluNewQuadric();
    gluQuadricOrientation ( quadric, GLU_INSIDE );
    gluCylinder ( quadric, 1, 0.1, 1 / sin ( m_spot_angle * 3.14 / 180.0 ), 10, 15 );
    gluQuadricOrientation ( quadric, GLU_OUTSIDE );
    gluCylinder ( quadric, 1, 0.1, 1 / sin ( m_spot_angle* 3.14 / 180.0 ), 10, 15 );
    gluDeleteQuadric ( quadric );

    render_positional_bulb();
  }

  void render_positional_bulb() const
  {
    GLUquadric* quadric = gluNewQuadric();
    gluSphere ( quadric, 0.5, 15, 15 );
    gluDeleteQuadric ( quadric );
  }

  void render_directional_bulb() const
  {
    glPushMatrix();
    glPushAttrib ( GL_ENABLE_BIT );
    glEnable ( GL_RESCALE_NORMAL );

    glRotatef ( 180.0, 1.0, 0.0, 0.0 );
    glScalef ( 2.0,2.0, 0.5 );

    DrawZArrow ();

    glTranslatef ( 0.125, 0, 0 );
    DrawZArrow ();
    glTranslatef ( -0.125, 0.125, 0 );
    DrawZArrow ();
    glTranslatef ( -0.125, -0.125, 0 );
    DrawZArrow ();
    glTranslatef ( 0.125, -0.125, 0 );
    DrawZArrow ();
    glPopAttrib();
    glPopMatrix();
  }

private:


  virtual bool WheelEvent
      ( const int &, const int &, const int &d,
        const eKeyFlags &kf,const eMouseFlags &)
  {

    if(m_render_graphic == false)
      return false;

    if(kf&KEYFLAG_CTRL)
    {

      double gf = 1.0;

      if(d>0)
        gf += g_light_model_grow_factor;
      else
        gf -= g_light_model_grow_factor;

      m_controller->set_uniform_scale(m_controller->get_uniform_scale()*gf);
      return true;
    }

    return false;
  }


  virtual bool MousePressedEvent
      ( const int &x, const int &y, const eMouseButton &mb,
        const eKeyFlags &,const eMouseFlags &)
  {

    if(m_render_graphic == false)
      return false;


    switch(mb)
    {
    case MOUSEBUTTON_RIGHT:
      {
        m_controller->StartTB ( x, y );
        return true;
      }
    case MOUSEBUTTON_LEFT:
      {
        m_controller->StartTrans ( x, y );
        return true;
      }
    default:
      return false;
    }
  }

  virtual bool MouseReleasedEvent
      ( const int &x, const int &y, const eMouseButton &mb,
        const eKeyFlags &,const eMouseFlags &)
  {

    if(m_render_graphic == false)
      return false;

    switch(mb)
    {
    case MOUSEBUTTON_RIGHT:
      {
        m_controller->StopTB ( x, y );
        return true;
      }

    case MOUSEBUTTON_MIDDLE:
      {
        set_enabled ( !is_enabled() );
        return true;
      }
    case MOUSEBUTTON_LEFT:
      {
        m_controller->StopTrans( x, y );
        return true;
      }
    default:
      return false;
    }
  }

  virtual bool MouseMovedEvent
      ( const int &x, const int &y, const int &, const int &,
        const eKeyFlags &,const eMouseFlags &mf)
  {
    if(m_render_graphic == false)
      return false;

    if(mf &MOUSEBUTTON_LEFT || mf &MOUSEBUTTON_RIGHT)
    {
      m_controller->Move ( x, y );
      return true;
    }
    return false;
  }

};


#include <ui_lightmanager_frame.h>
#include <qtcolorpicker.h>

static float global_specular[] = {1, 1, 1, 1};
static float global_emission[] = {0, 0, 0, 1};
//static float global_ambient[] = {0.5,0.5,.5, 1};
//static float global_diffuse[] = {1.0,1.0,1.0, 1};


TLightManager::TLightManager ( int numlights, IFramework *f )
{

  m_framework  = f;
  m_num_lights = numlights;
  m_dirty      = true;

  for ( int i = 0 ; i < m_num_lights;++i )
  {
    m_lights.push_back(boost::shared_ptr<TLight> (
    new TLight ( GL_LIGHT0 + i )));
  }

  for(std::vector<boost::shared_ptr<TLight> >::iterator iter = m_lights.begin();
      iter != m_lights.end();++iter)
  {
    m_framework->AddModel(*iter);
  }


  for ( int i = 1 ; i < m_num_lights;++i )
  {
    m_lights[i]->set_enabled ( false );
  }

  //  copy(global_ambient,global_ambient+4,m_ambient);
  copy(global_emission,global_emission+4,m_emission);
  copy(global_specular,global_specular+4,m_specular);
  //  copy(global_diffuse,global_diffuse+4,m_diffuse);

  m_shininess = 40;

  m_color_material_mode = GL_AMBIENT_AND_DIFFUSE;

#ifdef QT_GUI_LIB

  m_ui         = new Ui::Lightmanager_Frame();
  m_ui->setupUi ( this );

  QColor r_specular,r_emission;
  //  r_ambient.setRgbF(COMMAVEC4(m_ambient));
  //  r_diffuse.setRgbF(COMMAVEC4(m_diffuse));
  r_specular.setRgbF(COMMAVEC4(m_specular));
  r_emission.setRgbF(COMMAVEC4(m_emission));

  //  m_ui->material_ambient_colorpicker->setCurrentColor(r_ambient);
  //  m_ui->material_diffuse_colorpicker->setCurrentColor(r_diffuse);
  m_ui->material_specular_colorpicker->setCurrentColor(r_specular);
  m_ui->material_emission_colorpicker->setCurrentColor(r_emission);


  m_ui->material_shininess_spinBox->setMaximum(128);
  m_ui->material_shininess_spinBox->setMinimum(0);

  m_ui->material_shininess_spinBox->setValue(m_shininess);

  m_ui->light_number_spinBox->setMaximum(m_num_lights);
  m_ui->light_number_spinBox->setMinimum(1);

  m_active_ui_light = 0;

  m_ui->light_number_spinBox->setValue(m_active_ui_light+1);

  m_ui->light_type_comboBox->addItem("Directional");
  m_ui->light_type_comboBox->addItem("Positional");
//  m_ui->light_type_comboBox->addItem("Spot");

  update_lights_ui();

#endif

}

TLightManager::~TLightManager()
{

}

void TLightManager::render_light_params() const
{
  /*turn material colors on */
  glEnable ( GL_COLOR_MATERIAL );

  /* Allow material to control ambient and diffuse only*/
  glColorMaterial ( GL_FRONT_AND_BACK,m_color_material_mode ) ;

  glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR, m_specular );

  glMaterialfv ( GL_FRONT_AND_BACK, GL_EMISSION, m_emission );

  //  glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT, m_ambient );
  //
  //  glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, m_diffuse );

  glMaterialf  ( GL_FRONT_AND_BACK, GL_SHININESS, m_shininess );
}

int TLightManager::Render() const
{

  if(m_dirty)
  {
    render_light_params();

    TLightManager *pThis = const_cast<TLightManager *> (this);
    pThis->m_dirty = false;
  }
  return 0;
}

ILightManager * ILightManager::create ( int l , IFramework *f )
{
  return new TLightManager ( l, f );
}

void ILightManager::destroy ( ILightManager *&p )
{
  delete p;
  p = NULL;
}

//void TLightManager::on_material_ambient_colorpicker_colorChanged
//    (const QColor &c)
//{
//  qreal r[4];
//  c.getRgbF(COMMAVEC4(&r));
//
//  COPYVEC4(m_ambient,r);
//  m_dirty = true;
//}
void TLightManager::on_material_specular_colorpicker_colorChanged
    (const QColor &c)
{
  qreal r[4];
  c.getRgbF(COMMAVEC4(&r));

  COPYVEC4(m_specular,r);
  m_dirty = true;
}
//void TLightManager::on_material_diffuse_colorpicker_colorChanged
//    (const QColor &c)
//{
//  qreal r[4];
//  c.getRgbF(COMMAVEC4(&r));
//
//  COPYVEC4(m_diffuse,r);
//
//  _LOG("diffuse updated");
//  m_dirty = true;
//}

void TLightManager::on_material_emission_colorpicker_colorChanged
    (const QColor &c)
{
  qreal r[4];
  c.getRgbF(COMMAVEC4(&r));

  COPYVEC4(m_emission,r);

  m_dirty = true;
}

void TLightManager::on_material_shininess_spinBox_valueChanged(int v)
{
  m_shininess = v;
  m_dirty = true;
}

void TLightManager::on_light_number_spinBox_valueChanged(int i)
{
  m_active_ui_light = i-1;
  update_lights_ui();

}

inline QColor convert_float_to_qcolor4(const float *f)
{
  QColor c;
  c.setRgbF(COMMAVEC4(f));
  return c;
}

void TLightManager::update_lights_ui()
{
  float ambient[4],diffuse[4],specular[4];

  m_lights[m_active_ui_light]->get_ambient(ambient);
  m_lights[m_active_ui_light]->get_specular(specular);
  m_lights[m_active_ui_light]->get_diffuse(diffuse);

  m_ui->light_ambient_colorpicker->setCurrentColor(convert_float_to_qcolor4(ambient));
  m_ui->light_specular_colorpicker->setCurrentColor(convert_float_to_qcolor4(specular));
  m_ui->light_diffuse_colorpicker->setCurrentColor(convert_float_to_qcolor4(diffuse));

  m_ui->light_type_comboBox->setCurrentIndex(m_lights[m_active_ui_light]->get_light_type());
}

void TLightManager::on_light_type_comboBox_currentIndexChanged(int i)
{
  m_lights[m_active_ui_light]->set_light_type((TLight::eLightType) i);
}

void TLightManager::on_light_ambient_colorpicker_colorChanged(const QColor &c)
{
  qreal qr[4];

  c.getRgbF(COMMAVEC4(&qr));

  float fr[4];

  COPYVEC4(fr,qr);

  m_lights[m_active_ui_light]->set_ambient(fr);

}

void TLightManager::on_light_diffuse_colorpicker_colorChanged(const QColor &c)
{
  qreal qr[4];

  c.getRgbF(COMMAVEC4(&qr));

  float fr[4];

  COPYVEC4(fr,qr);

  m_lights[m_active_ui_light]->set_diffuse(fr);
}

void TLightManager::on_light_specular_colorpicker_colorChanged(const QColor &c)
{
  qreal qr[4];

  c.getRgbF(COMMAVEC4(&qr));

  float fr[4];

  COPYVEC4(fr,qr);

  m_lights[m_active_ui_light]->set_specular(fr);
}

void TLightManager::on_show_light_graphics_checkBox_clicked(const bool &v)
{
  for(int i = 0 ;i < m_num_lights;++i)
  {
    m_lights[i]->set_render_graphic(v);
  }
}


