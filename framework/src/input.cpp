#include <set>
#include <algorithm>
#include <map>
#include <iostream>
#include <cstring>

#include <framework.h>
#include <input.h>
#include <glutils.h>

using namespace std;

class TInputMgr: public IInputMgr
{

private:
  map<unsigned int, IHandleInput*>  m_InputHandlers;
  IHandleInput                     *m_activeHandler;
  OGLWindowRenderer                *m_ogl_wr;
  IHandleInput                     *m_defaultHandler;
  bool                              m_use_gl_pick;
  int                               old_x, old_y;

public:

  TInputMgr ( OGLWindowRenderer *wr,
              IHandleInput * dh
              ) :
      m_ogl_wr(wr),
      m_activeHandler ( dh),
      m_defaultHandler(dh)
  {
    old_x = 0;
    old_y = 0;

    Register(m_defaultHandler);

    m_use_gl_pick = true;

  }

  virtual void set_use_gl_pick(const bool &v)
  {
    m_use_gl_pick = v;
  }
  virtual bool get_use_gl_pick() const
  {
    return m_use_gl_pick;
  }

  void Register ( IHandleInput *m )
  {
    m_InputHandlers.insert
        ( pair<unsigned int, IHandleInput* > ( m->IID::GetID(), m ) );
  }

  void Unregister ( IHandleInput *m )
  {
    m_InputHandlers.erase ( m->IID::GetID() );
  }

  bool change_inputhandler_keyboard_override
      (const unsigned char & k, const eKeyFlags  &kf)
  {

    if( !(kf&KEYFLAG_CTRL) )
      return false;

    if( (k != '<') && (k != '>'))
      return false;

    map<unsigned int, IHandleInput*>::iterator handler_map_iterator;

    if(k == '<')
    {
      handler_map_iterator = ++m_InputHandlers.find ( m_activeHandler->IID::GetID() );

      if(handler_map_iterator == m_InputHandlers.end())
        handler_map_iterator = m_InputHandlers.begin();
    }
    else
    {
      handler_map_iterator = m_InputHandlers.find ( m_activeHandler->IID::GetID() );

      if( handler_map_iterator != m_InputHandlers.begin())
        handler_map_iterator = --handler_map_iterator;
      else
        handler_map_iterator = --m_InputHandlers.end();
    }

    m_activeHandler = handler_map_iterator->second;

    printUpdatedHandlerInfo();

    return true;

  }

  virtual bool NotifyKey( const unsigned char & k, const eKeyFlags  &kf)
  {
    if(!change_inputhandler_keyboard_override(k,kf))
    {
      return m_activeHandler->KeyEvent ( k, kf );
    }
    else
    {
      return true;
    }
  }

  virtual bool NotifyMousePressed
      ( const int &x, const int &y, const eMouseButton &mb,
        const eKeyFlags &kf,const eMouseFlags &mf)
  {
    if(m_use_gl_pick)
      HandlePick ( x , y );

    old_x = x;old_y = y;

    return m_activeHandler->MousePressedEvent( x, y, mb,kf,mf);

  }

  virtual bool NotifyMouseReleased
      ( const int &x, const int &y, const eMouseButton &mb,
        const eKeyFlags &kf,const eMouseFlags &mf)
  {
    return m_activeHandler->MouseReleasedEvent( x, y, mb,kf,mf);
  }

  virtual bool NotifyMouseMoved
      ( const int &x, const int &y,
        const eKeyFlags &kf,const eMouseFlags &mf)
  {

    int dx = x - old_x;
    int dy = y - old_y;

    old_x = x;
    old_y = y;

    return m_activeHandler->MouseMovedEvent ( x, y, dx, dy,kf,mf );
  }

  virtual bool NotifyWheel
      ( const int &x, const int &y, const int &d,
        const eKeyFlags &kf,const eMouseFlags &mf )
  {
    return m_activeHandler->WheelEvent(x,y,d,kf,mf);
  }

  void HandlePick ( const int &x , const int &y )
  {
#define BUFSIZE 512
    GLuint selectBuf[BUFSIZE];
    memset ( selectBuf, 0, sizeof ( selectBuf ));


    int hits =  m_ogl_wr->gl_Display_for_Select ( x, y, selectBuf, BUFSIZE );
    PickHandlerFromGLSelectBuf ( selectBuf, hits );
  }

  void log_all_handlers(std::ostream &os)
  {
    for(map<unsigned int, IHandleInput*>::iterator iter = m_InputHandlers.begin();
        iter != m_InputHandlers.end(); ++iter)
    {
      os<<iter->first<<"::";
      os<<iter->second->Name()<<std::endl;

    }
  }

  virtual void PickHandlerFromGLSelectBuf ( unsigned int *buf, int hits )
  {
    /* save the old handler .. just for output */
    IHandleInput *oldhandler = m_activeHandler;

    /*set active handler to default */
    m_activeHandler = m_InputHandlers[m_defaultHandler->GetID()];

    /*process the picks*/
    unsigned int names, *ptr, minZ, *ptrNames = NULL, numberOfNames = 0;

    ptr = ( unsigned int * ) buf;
    minZ = 0xffffffff;

    for ( int i = 0; i < hits; i++ )
    {
      names = *ptr;
      ptr++;

      if ( *ptr < minZ )
      {
        numberOfNames = names;
        minZ = *ptr;
        ptrNames = ptr + 2;
      }

      ptr += names + 2;
    }

    ptr = ptrNames;

    for ( unsigned int j = 0; j < numberOfNames; j++, ptr++ )
    {
      // could be buggy here
      if ( m_InputHandlers.find ( *ptr ) != m_InputHandlers.end() )
      {
        m_activeHandler = m_InputHandlers[*ptr];

        break;
      }
    }

    if ( oldhandler != m_activeHandler )
    {
      printUpdatedHandlerInfo();
    }
  }

  void printUpdatedHandlerInfo()
  {
      stringstream message;

      message << "Active input reciever is now " << m_activeHandler->Name();

      _LOG(message.str().c_str() );
  }

};

IInputMgr* IInputMgr::Create ( OGLWindowRenderer *wr, IHandleInput *dh )
{
  return new TInputMgr (wr,dh);
}

void IInputMgr::Delete ( IInputMgr *&m )
{
  delete m;
  m = NULL;
}


