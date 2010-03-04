
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <iostream>

#include <GL/gl.h>

#include <input.h>
#include <framework.h>
#include <logutil.h>
#include <model.h>

IModel::~IModel()
{
}


using namespace std;

template<class TYPE, class t> struct deletePtr : public unary_function<TYPE, void>
{
  void operator() ( pair<TYPE*, t> x )
  {
    delete x.first;
  }
};

class TModelMgr: public IModelMgr
{

  private:
    map<IModel*, bool>  m_RenderModels;
    set<IAnimation*>   m_AnimateModels;
    IFramework * m_framework;

  public:

    TModelMgr ( IFramework * f )
        : m_framework ( f )
    {
    }

    virtual ~TModelMgr()
    {
      m_RenderModels.clear();
      m_AnimateModels.clear();
    }

    virtual void Register ( IModel *m )
    {
      IHandleInput *i = dynamic_cast<IHandleInput *> ( m );

      if ( i != NULL )
      {
        m_RenderModels.insert ( pair<IModel*, bool> ( m, true ) );
      }
      else
      {
        m_RenderModels.insert ( pair<IModel*, bool> ( m, false ) );
      }

      IAnimation *a = dynamic_cast<IAnimation *> ( m );

      if ( a != NULL )
      {
        m_AnimateModels.insert ( a );
      }

      _LOG ( "IModelMgr   :: Added model   ::" << m->Name() );

      _LOG ( "IModelMgr   :: Handles input ::" << ( i != NULL )  );

      _LOG ( "IModelMgr   :: Is Animation  ::" << ( a != NULL ) );
    }

    virtual void Unregister ( IModel *m )
    {
      m_RenderModels.erase ( m );

      IAnimation *a = dynamic_cast<IAnimation *> ( m );

      if ( a != NULL )
      {
        m_AnimateModels.erase ( a );
      }

    }

    virtual int RenderAll()
    {
      int polycount = 0;

      for ( map<IModel *, bool>::iterator i = m_RenderModels.begin();
            i != m_RenderModels.end();i++ )
      {
        glMatrixMode ( GL_MODELVIEW );

        glPushMatrix();

        IModel *m = i->first;

        m->Name();

        if ( ( *i ).second == true )
        {
          glPushName ( m->IID::GetID() );
        }

        polycount += ( * ( *i ).first ).Render();

        if ( ( *i ).second == true )
        {
          glPopName();
        }

        glMatrixMode ( GL_MODELVIEW );

        glPopMatrix();
      }

      return polycount;
    }

    virtual void ResetAll()
    {
      for ( map<IModel *, bool>::iterator i = m_RenderModels.begin();
            i != m_RenderModels.end();i++ )
      {
        ( * ( *i ).first ).Reset();
      }

      return ;
    }

    virtual int RenderAllForPick()
    {
      int polycount = 0;

      for ( map<IModel *, bool>::iterator i = m_RenderModels.begin();
            i != m_RenderModels.end();i++ )
      {
        glPushMatrix();

        if ( ( *i ).second == true )
        {
          glPushName ( ( * ( *i ).first ).IID::GetID() );
        }

        polycount += ( * ( *i ).first ).RenderForPick();

        if ( ( *i ).second == true )
        {
          glPopName();
        }

        glPopMatrix();
      }

      return polycount;
    }

    virtual int RenderAllShadows ( float * lp )
    {
      int polycount = 0;

      for ( map<IModel *, bool>::iterator i = m_RenderModels.begin();
            i != m_RenderModels.end();i++ )
      {
        glPushMatrix();

        polycount += ( *i ).first->RenderShadow ( lp );

        glPopMatrix();
      }

      return polycount;
    }

    struct AnimateModel
    {
      unsigned long long m_t;
      AnimateModel ( unsigned long long t ) : m_t ( t ) {}

      void operator() ( IAnimation *a )
      {
        a->Animate ( m_t );
      }
    };

    virtual void AnimateAll ( unsigned long long t )
    {
      for_each ( m_AnimateModels.begin(), m_AnimateModels.end(), AnimateModel ( t ) );
    }

    virtual void AddModel ( IModel * model )
    {
      Register ( model );
    }

    virtual void DelModel ( IModel * model )
    {
      Unregister ( model );
    }
};

IModelMgr* IModelMgr::Create ( IFramework *f )
{
  return new TModelMgr ( f );
}

void IModelMgr::Delete ( IModelMgr *& mgr )
{
  delete mgr;
  mgr = NULL;
}




