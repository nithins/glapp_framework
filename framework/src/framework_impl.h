/***************************************************************************
 *   Copyright (C) 2009 by Nithin Shivashankar,   *
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
#ifndef FRAMEWORK_IMPL_INCLUDED
#define FRAMEWORK_IMPL_INCLUDED
#include <map>

#include <framework.h>
#include <model.h>
#include <camera.h>
#include <input.h>
#include <light.h>
#include <timer.h>


#if defined QT_GUI_LIB
#include <QFrame>

class QApplication;

class MainWindowForm;
#endif

namespace Ui
{

  class Framework_Frame;
}

class TFramework:
#ifdef QT_GUI_LIB
    public QFrame,
#endif
    virtual public IFramework,
    virtual public OGLWindowRenderer,
    virtual public IHandleInput
{

#ifdef QT_GUI_LIB
  Q_OBJECT
#endif

private:
  bool m_bAnimating;
  bool g_bShowWorldAxes;
  bool g_bShowFocalPoint;
  bool myBackFaceCullingOn;
  bool mySmoothShadingOn;
  bool myLightsOn;
  bool g_bWireframe;
  bool g_bShadowsOn;

  uint   m_frame_ct;
  double m_last_fps_rpt_time; // in sec
  Timer  m_timer;

  boost::shared_ptr<Camera>        s_cam;
  boost::shared_ptr<ILightManager> m_LightMgr;
  boost::shared_ptr<IModelMgr>     m_ModelMgr;
  boost::shared_ptr<IInputMgr>     m_InputMgr;

#ifdef QT_GUI_LIB
  QApplication *        m_app;
  MainWindowForm *      m_MainWindow;
  Ui::Framework_Frame * m_ui;
#endif

public:

  virtual std::string Name() const
  {
    return std::string("Framework");
  }


  TFramework
      (
#ifdef QT_GUI_LIB
      QApplication *a
#endif
      );

  virtual ~TFramework();

  virtual void gl_Init ();
  virtual bool gl_Display ();
  virtual void gl_Reshape ( int width, int height );
  virtual bool gl_Idle ();
  virtual int  gl_Display_for_Select ( int x, int y, unsigned int * , const int&  );

  IInputMgr  * get_input_manager();

  virtual bool KeyEvent( const unsigned char & , const eKeyFlags  &);

  virtual bool MouseMovedEvent
      ( const int &, const int &, const int &, const int &,
        const eKeyFlags &,const eMouseFlags &);

  virtual void Exec ();
  virtual void AddModel ( boost::shared_ptr<IModel> );
  virtual void DelModel ( boost::shared_ptr<IModel> );

  virtual void ShowStatusMessage ( const std::string );

#ifdef QT_GUI_LIB

private Q_SLOTS:

  void on_take_snapshot_pushButton_clicked ( bool );
  void on_record_pushButton_clicked ( bool );
  void on_use_gl_pick_checkBox_clicked(bool);
#endif
};

#endif
