/***************************************************************************
 *   Copyright (C) 2009 by nithin,senthil   *
 *   nithin@gauss   *
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
#include <math.h>


#include <framework.h>

#include <QMouseEvent>

#include <logutil.h>


#include <glwidget.h>
#include <input.h>




using namespace std;

GLWidget::GLWidget ( QWidget *parent, OGLWindowRenderer *wr )
  : QGLWidget ( QGLFormat ( QGL::SampleBuffers ), parent ),
  m_ogl_wr ( wr ),
  m_saveFrames ( false )
{
  setFocusPolicy ( Qt::ClickFocus );
}

GLWidget::~GLWidget()
{
  makeCurrent();
}

QSize GLWidget::minimumSizeHint() const
{
  return QSize ( 50, 50 );
}

QSize GLWidget::sizeHint() const
{
  return parentWidget()->contentsRect().size();
}

void GLWidget::initializeGL()
{

  context()->format().setRgba ( true );
  context()->format().setDoubleBuffer ( true );
  context()->format().setAlpha ( true );
  context()->format().setDepth ( true );

  m_ogl_wr->gl_Init();
  startTimer ( 20 );
}

void GLWidget::timerEvent ( QTimerEvent * )
{
  m_ogl_wr->gl_Idle();
  updateGL();

  if ( m_saveFrames == true )
  {
    m_saveFrames_list.append ( grabFrameBuffer() );
  }
}

void GLWidget::paintGL()
{
  m_ogl_wr->gl_Display();

  if ( doubleBuffer() )
    swapBuffers();
}

void GLWidget::resizeGL ( int width, int height )
{
  m_ogl_wr->gl_Reshape ( width, height );

}

eKeyFlags convert_from_key_modifiers(Qt::KeyboardModifiers m)
{
  int kf = 0 ;
  kf |= (m&Qt::ShiftModifier)?(KEYFLAG_SHIFT):(0);
  kf |= (m&Qt::ControlModifier)?(KEYFLAG_CTRL):(0);
  kf |= (m&Qt::AltModifier)?(KEYFLAG_ALT):(0);
  return (eKeyFlags)kf;
}

eMouseFlags convert_from_mouse_buttons(Qt::MouseButtons m)
{
  int mf = 0;
  mf |= (m&Qt::LeftButton)?(MOUSEBUTTON_LEFT):(0);
  mf |= (m&Qt::MidButton)?(MOUSEBUTTON_MIDDLE):(0);
  mf |= (m&Qt::RightButton)?(MOUSEBUTTON_RIGHT):(0);
  return (eMouseFlags)mf;
}

eMouseButton convert_from_mouse_button(Qt::MouseButton b )
{
  eMouseButton mb = (eMouseButton)0;

  switch(b)
  {
  case Qt::LeftButton: mb = MOUSEBUTTON_LEFT;break;
  case Qt::MidButton: mb = MOUSEBUTTON_MIDDLE;break;
  case Qt::RightButton: mb = MOUSEBUTTON_RIGHT;break;
  default: break;
  };
  return mb;
}

void GLWidget::mousePressEvent ( QMouseEvent *event )
{

  int x = event->x();
  int y = event->y();
  eMouseButton mb = convert_from_mouse_button(event->button());
  eKeyFlags    kf = convert_from_key_modifiers(event->modifiers());
  eMouseFlags  mf = convert_from_mouse_buttons(event->buttons());

  if ( m_ogl_wr->get_input_manager()->NotifyMousePressed(x,y,mb,kf,mf) )
    updateGL();

}

void GLWidget::mouseReleaseEvent( QMouseEvent *event )
{

  int x = event->x();
  int y = event->y();
  eMouseButton mb = convert_from_mouse_button(event->button());
  eKeyFlags    kf = convert_from_key_modifiers(event->modifiers());
  eMouseFlags  mf = convert_from_mouse_buttons(event->buttons());


  if ( m_ogl_wr->get_input_manager()->NotifyMouseReleased(x,y,mb,kf,mf) )
    updateGL();

}

void GLWidget::mouseMoveEvent ( QMouseEvent *event )
{
  int x = event->x();
  int y = event->y();

  eKeyFlags    kf = convert_from_key_modifiers(event->modifiers());
  eMouseFlags  mf = convert_from_mouse_buttons(event->buttons());

  if ( m_ogl_wr->get_input_manager()->NotifyMouseMoved(x,y,kf,mf) )
    updateGL();

}

void GLWidget::wheelEvent ( QWheelEvent * event )
{
  int x = event->x();
  int y = event->y();
  int d = event->delta();

  eKeyFlags    kf = convert_from_key_modifiers(event->modifiers());
  eMouseFlags  mf = convert_from_mouse_buttons(event->buttons());

  if(m_ogl_wr->get_input_manager()->NotifyWheel(x,y,d,kf,mf))
    updateGL();
}

void GLWidget::keyPressEvent ( QKeyEvent *event )
{
  unsigned char key = event->text().data() [0].toAscii();
  eKeyFlags     kf = convert_from_key_modifiers(event->modifiers());

  if ( m_ogl_wr->get_input_manager()->NotifyKey(key,kf) )
    updateGL();
}

QImage GLWidget::getFrameBuffer ()
{
  return grabFrameBuffer();
}

void GLWidget::startFrameBufferRecord ()
{
  m_saveFrames        = true;
  m_saveFrames_list.clear();
}

void GLWidget::stopFrameBufferRecord ( QList<QImage>& frames_list )
{
  frames_list +=  m_saveFrames_list ;
  m_saveFrames_list.clear();
  m_saveFrames        = false;
}



