/***************************************************************************
 *   Copyright (C) 2009 by nithin   *
 *   nithin@vidyaranya   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <GL/glew.h>

#include <glutils.h>
#include <logutil.h>

#include <framework_impl.h>

#ifdef QT_GUI_LIB
#include <QString>
#include <QTemporaryFile>
#include <QDir>
#include <QProgressDialog>

#include <Mainwindow.h>
#include <ui_framework_frame.h>
#include <glwidget.h>

#endif


using namespace std;

/* Constants */
const int   ANIMATION_DELAY = 20;    /* milliseconds between rendering */

TFramework::TFramework
(
#ifdef QT_GUI_LIB
  QApplication *a
#endif
)
{
  /*initial parameters */
  m_bAnimating  = false ;

  myLightsOn = true;
  mySmoothShadingOn = true;
  myBackFaceCullingOn = true;
  g_bShowWorldAxes = false;
  g_bWireframe = false;
  g_bShowFocalPoint = false;
  g_bShadowsOn = false;

  m_timer.start();
  m_frame_ct = 0;
  m_last_fps_rpt_time = 0.0;

  m_InputMgr.reset(IInputMgr::Create ( this,this));
  m_ModelMgr.reset(IModelMgr::Create ( this ));

#ifdef QT_GUI_LIB
  m_app = a;

  m_MainWindow = new MainWindowForm ( this );

  m_ui = new Ui::Framework_Frame();
  m_ui->setupUi ( this );

  m_MainWindow->show();

  m_MainWindow->getToolBox()->removeItem ( 0 );
  m_MainWindow->getToolBox()->addItem ( this, "Framework" );
#endif

}

TFramework::~TFramework()
{

}


void TFramework::gl_Init ()
{

  glewInit();

  if ( glewIsSupported ( "GL_VERSION_2_0" ) )
  {
    if ( ! ( GLEW_ARB_vertex_shader &&
             GLEW_ARB_fragment_shader &&
             GL_EXT_geometry_shader4 &&
             GL_TEXTURE_RECTANGLE_ARB ) )
    {
      _ERROR ( "Some extensions are not supported" );
    }

    if ( ! glewIsSupported ( "GL_ARB_pixel_buffer_object" ) )
    {
      _ERROR ( "ERROR: Support for necessary OpenGL extensions missing." );
    }
  }
  else
  {
    _ERROR ( "No support for opengl 2.0" );
  }



  /* by default the back ground color is black */
  glClearColor ( 1.0, 1.0, 1.0, 1.0 );

  /* set to draw in window based on depth  */
  glEnable ( GL_DEPTH_TEST );

  /*turn on naming */
  glInitNames ();

  glShadeModel ( GL_SMOOTH );

  glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

  /*turn lighting on */
  glEnable ( GL_LIGHTING ) ;

  /*turn back face culling off */
  glEnable ( GL_CULL_FACE );

  /*cull backface */
  glCullFace ( GL_BACK );

  /*polymode */
  glPolygonMode ( GL_FRONT, GL_FILL );

  glPolygonMode ( GL_BACK, GL_FILL );

  m_LightMgr.reset(ILightManager::create ( 2, this ));

  AddModel ( m_LightMgr );

  /*get camera*/

  s_cam.reset(Camera::Create());

  /*get the perspective view */
  s_cam->SetPerspective();
}

#define FPS_REPORT_INTERVAL 5

bool TFramework::gl_Display ()
{
  /* clears requested bits (color and depth) in glut window */
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

  /* draw entire scene into cleared window */
  glPushMatrix();

  glLoadIdentity();

  s_cam->Set();

  glEnable ( GL_RESCALE_NORMAL );

  /* draw the other stuff */

  if ( g_bShowWorldAxes == true )
  {
    DrawAxes();
  }

  if ( g_bShowFocalPoint == true )
  {
    s_cam->DrawFocus();
  }

  m_ModelMgr->RenderAll();

  glPopMatrix();

  /* check for any errors when rendering */
  GLenum errorCode = glGetError();

  if ( errorCode == GL_NO_ERROR )
  {
    glFlush();
  }

  double timer_interval = m_timer.getElapsedTimeInSec() - m_last_fps_rpt_time;

  if(timer_interval > FPS_REPORT_INTERVAL )
  {
    double fps = (double)m_frame_ct / timer_interval;

    stringstream message;

    message << fps << " fps";

    ShowStatusMessage ( message.str().c_str() );

    m_frame_ct = 0;

    m_last_fps_rpt_time += timer_interval;
  }
  else
  {
    m_frame_ct++;
  }

  return true;
}


void TFramework::gl_Reshape ( int width, int height )
/* pre:  glut window has been resized
   post: resets cameras location and aspect to match window */
{
  glViewport ( 0, 0, width, height );
}

bool TFramework::gl_Idle ()
{
  m_ModelMgr->AnimateAll ( m_timer.getElapsedTimeInMilliSec() );
  return true;
}

IInputMgr * TFramework::get_input_manager()
{
  return m_InputMgr.get();
}



bool TFramework::MouseMovedEvent
      ( const int &, const int &, const int &dx, const int &dy,
        const eKeyFlags &,const eMouseFlags &)
{
  s_cam->MoveLR ( -dx );
  s_cam->MoveUD ( -dy );
  return true;
}

bool TFramework::KeyEvent( const unsigned char &key , const eKeyFlags  &kf)
{
  bool handled = true;

  bool altPress = (kf&KEYFLAG_ALT)?(true):(false);

  switch ( key )
  {
      /* toggle animation running */

    case 'P':
      m_bAnimating = !m_bAnimating;
      break;
      /* turn shadows on/off */

    case 'S':
      g_bShadowsOn = !g_bShadowsOn;

      break;

    case 'F':
      g_bShowFocalPoint = !g_bShowFocalPoint;
      break;
      /*toggle back face culling */

    case 'b':
      myBackFaceCullingOn = ! myBackFaceCullingOn;

      if ( myBackFaceCullingOn )
      {
        glEnable ( GL_CULL_FACE );
        ShowStatusMessage ( "Back face culling on" );
      }
      else
      {
        glDisable ( GL_CULL_FACE );
        ShowStatusMessage ( "Back face culling off" );
      }

      break;

    case 'B':

      GLint ffmode;

      glGetIntegerv ( GL_FRONT_FACE, &ffmode );

      if ( ffmode == GL_CW )
      {
        glFrontFace ( GL_CCW );
        ShowStatusMessage ( "Front face CCW" );
      }
      else
      {
        glFrontFace ( GL_CW );
        ShowStatusMessage ( "Front face CW" );
      }

      break;

      /* toggle smooth shading */

    case 'c':
      mySmoothShadingOn = ! mySmoothShadingOn;

      if ( mySmoothShadingOn )
        glShadeModel ( GL_SMOOTH );
      else
        glShadeModel ( GL_FLAT );

      break;

      /*print legend */

    case 'L':
      myLightsOn = ! myLightsOn;

      if ( myLightsOn )
        glEnable ( GL_LIGHTING );
      else
        glDisable ( GL_LIGHTING );

      break;

    case 'x':
      g_bShowWorldAxes = !g_bShowWorldAxes;

      break;

    case 'W':
      g_bWireframe = !g_bWireframe;

      if ( g_bWireframe == true )
      {
        glPolygonMode ( GL_FRONT, GL_LINE );
        glPolygonMode ( GL_BACK, GL_LINE );
      }
      else
      {
        glPolygonMode ( GL_FRONT, GL_FILL );
        glPolygonMode ( GL_BACK, GL_FILL );
      }

      break;

    case 'w':

      if ( altPress )
        s_cam->MoveFB ( 1 );
      else
        s_cam->StrafeFB ( 1 );

      break;

    case 's':
      if ( altPress )
        s_cam->MoveFB ( -1 );
      else
        s_cam->StrafeFB ( -1 );

      break;

    case 'a':
      if ( altPress )
        s_cam->MoveLR ( 1 );
      else
        s_cam->StrafeLR ( 1 );

      break;

    case 'd':
      if ( altPress )
        s_cam->MoveLR ( -1 );
      else
        s_cam->StrafeLR ( -1 );

      break;

    case 'q':
      if ( altPress )
        s_cam->MoveUD ( 1 );
      else
        s_cam->StrafeUD ( 1 );

      break;

    case 'e':
      if ( altPress )
        s_cam->MoveUD ( -1 );
      else
        s_cam->StrafeUD ( -1 );

      break;

    case '[':
      s_cam->Twist ( -1 );

      break;

    case ']':
      s_cam->Twist ( 1 );

      break;

    case 'r':
      s_cam->Reset();

      m_ModelMgr->ResetAll();

      break;

    default:
      handled = false;

      break;
  }

  return handled;

}

int TFramework::gl_Display_for_Select ( int x, int y, unsigned int * selectBuf, const int& bufsize )
{
  GLint viewport[4];

  glSelectBuffer ( bufsize, selectBuf );
  glRenderMode ( GL_SELECT );

  /* draw entire scene into cleared window */
  glMatrixMode ( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  glGetIntegerv ( GL_VIEWPORT, viewport );
  gluPickMatrix ( x, viewport[3] - y, 10, 10, viewport );

  s_cam->SetPerspective();

  glMatrixMode ( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  s_cam->Set();

  /* clears requested bits (color and depth) in glut window */
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

  /*action */
  m_ModelMgr->RenderAllForPick();

  glPopMatrix();

  glMatrixMode ( GL_PROJECTION );

  glPopMatrix();

  glMatrixMode ( GL_MODELVIEW );

  // returning to normal rendering mode
  int hits =  glRenderMode ( GL_RENDER );

  return hits;
}

void TFramework::Exec ()
{
#ifdef QT_GUI_LIB
  m_app->exec();
#endif
}

void TFramework::AddModel ( boost::shared_ptr<IModel> m_sp)
{
  IModel *m = m_sp.get();

  m_ModelMgr->AddModel (m);

  IHandleInput *i = dynamic_cast<IHandleInput *> ( m );

  if ( i != NULL )
  {
    m_InputMgr->Register ( i );
  }

#ifdef QT_GUI_LIB

  if ( m->getQFrame() != NULL )
  {
    int index = m_MainWindow->getToolBox()->addItem ( m->getQFrame(),
                                                      m->Name().c_str() );
    m_MainWindow->getToolBox()->setCurrentIndex ( index );
  }

#endif
}


void TFramework::ShowStatusMessage ( std::string message )
{
#ifdef QT_GUI_LIB
  m_MainWindow->statusBar()->showMessage ( message.c_str() );
#else
  _LOG ( message );
#endif

}

void TFramework::DelModel ( boost::shared_ptr<IModel> m_sp)
{
  IModel *m = m_sp.get();
  IHandleInput *i = dynamic_cast<IHandleInput *> ( m );

  if ( i != NULL )
  {
    m_InputMgr->Unregister ( i );
  }

  m_ModelMgr->DelModel ( m );
}

#ifdef QT_GUI_LIB

void TFramework::on_take_snapshot_pushButton_clicked ( bool )
{
  QString filename;

  {
    QTemporaryFile tempfile ( "./XXXXXX.png" );
    tempfile.open();
    filename = tempfile.fileName();
  }

  QImage img = m_MainWindow->getGLWidget()->getFrameBuffer();
  img.save ( filename, "PNG", 100 );
}

void TFramework::on_record_pushButton_clicked ( bool state )
{

  if ( state == true )
  {
    m_MainWindow->getGLWidget()->startFrameBufferRecord ();
  }
  else
  {
    QList<QImage> images;
    m_MainWindow->getGLWidget()->stopFrameBufferRecord ( images );

    QString dirname;

    {
      QTemporaryFile tempdir ( "./XXXXXX" );
      tempdir.open();
      dirname = tempdir.fileName();
    }

    QDir dir;

    if ( dir.mkpath ( dirname ) )
    {

      QProgressDialog progress ( "Copying Frames ...", "Abort Copy", 0, images.size(), this );
      progress.setWindowModality ( Qt::WindowModal );
      progress.show();

      for ( int i = 0; i < images.size();++i )
      {

        progress.setValue ( i );

        if ( progress.wasCanceled() )
          break;

        QImage img = images[i];

        QString filename = QString ( "%1/%2.png" ).arg ( dirname ).arg ( i );

        if ( !img.save ( filename, "PNG", 100 ) )
        {
          _LOG ( "Could not file dir" << filename.toAscii().constData() );
          break;
        }
      }

      progress.setValue ( images.size() );
    }
    else
    {
      _LOG ( "Could not create dir" << dirname.toAscii().constData() );
    }

    images.clear();
  }

}

void TFramework::on_use_gl_pick_checkBox_clicked(bool v)
{
  m_InputMgr->set_use_gl_pick(v);
}

#endif

boost::shared_ptr<IFramework> IFramework::Create ()
{

#ifdef QT_GUI_LIB

  int    argc = 0;
  char **argv = NULL;

  QApplication * a = new QApplication ( argc, argv );

  return boost::shared_ptr<IFramework>(new TFramework ( a ));
#else
  return boost::shared_ptr<IFramework>(new TFramework ());
#endif
}

boost::shared_ptr<IFramework>  IFramework::Create (int &argc , char **argv)
{
#ifdef QT_GUI_LIB
  QApplication * a = new QApplication ( argc, argv );

  return boost::shared_ptr<IFramework>(new TFramework ( a ));
#else
  return boost::shared_ptr<IFramework>(new TFramework ());
#endif

}
