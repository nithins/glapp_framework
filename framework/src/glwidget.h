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
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QString>
#include <QList>
#include <QImage>

class OGLWindowRenderer;

class GLWidget : public QGLWidget
{
    Q_OBJECT

  public:
    GLWidget ( QWidget *parent = 0, OGLWindowRenderer *f = 0 );
    ~GLWidget();

    QImage getFrameBuffer();

    void startFrameBufferRecord();
    void stopFrameBufferRecord(QList<QImage> &);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

  protected:
    void initializeGL();
    void paintGL();
    void resizeGL ( int width, int height );
    void mousePressEvent ( QMouseEvent *event );
    void mouseMoveEvent ( QMouseEvent *event );
    void mouseReleaseEvent ( QMouseEvent *event );
    void keyPressEvent ( QKeyEvent *event );
    void wheelEvent ( QWheelEvent * event );

    void timerEvent ( QTimerEvent *event );


  private:
    OGLWindowRenderer * m_ogl_wr;

    bool          m_saveFrames;
    QString       m_saveFramesDir;
    QString       m_saveFramesFormat;
    QList<QImage> m_saveFrames_list;
};

#endif
