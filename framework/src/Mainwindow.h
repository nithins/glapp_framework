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
#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include "ui_MainWindow.h"

class GLWidget;
class OGLWindowRenderer;
class QToolBox;
class MainWindowForm :  public QMainWindow,public Ui::MainWindow
{
    Q_OBJECT

  public:
    MainWindowForm ( OGLWindowRenderer* ,QMainWindow *parent = 0 );
    QToolBox * getToolBox();

    inline GLWidget * getGLWidget()
    {
      return m_glwidget;
    }

    virtual ~MainWindowForm();


  private Q_SLOTS:

  private:
    GLWidget *m_glwidget;

};
#endif
