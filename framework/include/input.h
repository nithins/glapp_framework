#ifndef __INPUT_H_DEFINED
#define __INPUT_H_DEFINED

#include <id.h>

enum eKeyFlags
{
  KEYFLAG_SHIFT = 0x01,
  KEYFLAG_CTRL = 0x02,
  KEYFLAG_ALT = 0x04
              };

enum eMouseFlags
{
  MOUSEBUTTON_LEFT   = 0x01,
  MOUSEBUTTON_MIDDLE = 0x02,
  MOUSEBUTTON_RIGHT  = 0x04
                     };

typedef eMouseFlags eMouseButton;

class IHandleInput: public IID
{

public:
  virtual bool KeyEvent( const unsigned char & , const eKeyFlags  &)
  {
    return false;
  }

  virtual bool MousePressedEvent
      ( const int &, const int &, const eMouseButton &,
        const eKeyFlags &,const eMouseFlags &)
  {
    return false;
  }

  virtual bool MouseReleasedEvent
      ( const int &, const int &, const eMouseButton &,
        const eKeyFlags &,const eMouseFlags &)
  {
    return false;
  }

  virtual bool MouseMovedEvent
      ( const int &, const int &, const int &, const int &,
        const eKeyFlags &,const eMouseFlags &)
  {
    return false;
  }

  virtual bool WheelEvent
      ( const int &, const int &, const int &,
        const eKeyFlags &,const eMouseFlags &)
  {
    return false;
  }


  virtual ~IHandleInput(){}
};

#if !defined(GLAFW_GENERATING_PYTHON_BINDINGS)

class IFramework;

class OGLWindowRenderer;

class IInputMgr
{

public:
  virtual void Register ( IHandleInput * ) = 0;
  virtual void Unregister ( IHandleInput * ) = 0;

  virtual bool NotifyKey( const unsigned char & , const eKeyFlags  &) = 0;

  virtual bool NotifyMousePressed
      ( const int &, const int &, const eMouseButton &,
        const eKeyFlags &,const eMouseFlags &) = 0;

  virtual bool NotifyMouseReleased
      ( const int &, const int &, const eMouseButton &,
        const eKeyFlags &,const eMouseFlags &) = 0;

  virtual bool NotifyMouseMoved
      ( const int &, const int &,
        const eKeyFlags &,const eMouseFlags &) = 0;

  virtual bool NotifyWheel
      ( const int &, const int &, const int &,
        const eKeyFlags &,const eMouseFlags &) = 0;

  virtual void set_use_gl_pick(const bool &) = 0;
  virtual bool get_use_gl_pick() const = 0;

  virtual ~IInputMgr() {}

  static IInputMgr* Create ( OGLWindowRenderer *, IHandleInput * );
  static void Delete ( IInputMgr* & );
};

#endif

#endif//__INPUT_HPP_DEFINED
