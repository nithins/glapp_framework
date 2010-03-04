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
#ifndef __FRAMEWORK_H
#define __FRAMEWORK_H

#include <string>
#include <boost/shared_ptr.hpp>

#include <id.h>


class IModel;

class IFramework
{

public:

  virtual void AddModel ( boost::shared_ptr<IModel> ) = 0;
  virtual void DelModel ( boost::shared_ptr<IModel> ) = 0;
  virtual void Exec () = 0 ;
  virtual void ShowStatusMessage ( std::string ) = 0;

  virtual ~IFramework() {}

  static boost::shared_ptr<IFramework>  Create ();

  #if !defined(GLAFW_GENERATING_PYTHON_BINDINGS)

  // from c++ we have to provide a refernce to the original from main..
  // no clue why .. else qt main app crashes
  // in python a bogus argument set also works fine though..:( ..
  // 1/2 day to figure that one out

  static boost::shared_ptr<IFramework>  Create (int &argc , char **argv);
  #endif

};

#if !defined(GLAFW_GENERATING_PYTHON_BINDINGS)

class IInputMgr;

class OGLWindowRenderer
{
public:

  virtual void gl_Init () = 0;
  virtual void gl_Reshape ( int width, int height ) = 0;
  virtual bool gl_Display () = 0;
  virtual int  gl_Display_for_Select ( int x, int y, unsigned int * , const int& ) = 0;
  virtual bool gl_Idle () = 0;

  virtual IInputMgr  * get_input_manager() = 0 ;
};
#endif

#endif
