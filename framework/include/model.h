#ifndef __MODEL_H_INCLUDED
#define __MODEL_H_INCLUDED

#include <id.h>

#if !defined QT_GUI_LIB && !defined(GLAFW_GENERATING_PYTHON_BINDINGS)
#define QT_GUI_LIB
#endif

#ifdef QT_GUI_LIB
class QFrame;
#endif

class IModel:public IID
{
public:
  virtual int Render() const = 0;
  virtual int RenderForPick() const {return Render();}
  virtual ~IModel();
  virtual int RenderShadow(float *) const { return 0;}
  virtual void Reset(){}

#if defined(QT_GUI_LIB) && !defined(GLAFW_GENERATING_PYTHON_BINDINGS)
  virtual QFrame * getQFrame(){return 0;}
#endif

};

class IAnimation:public IModel
{
public:
  virtual void Animate(unsigned long long tmsec) = 0 ;
  virtual ~IAnimation(){}
};


#if !defined(GLAFW_GENERATING_PYTHON_BINDINGS)


class IFramework;

class IModelMgr
{
public:

  virtual int RenderAll()=0;
  virtual int RenderAllForPick()=0;
  virtual int RenderAllShadows(float *)=0;
  virtual void AnimateAll(unsigned long long tmsec)=0;
  virtual void ResetAll()=0;
  virtual ~IModelMgr(){}
  virtual void AddModel(IModel * model) =0;
  virtual void DelModel(IModel * model) =0;

  static IModelMgr * Create(IFramework *);
  static void Delete(IModelMgr *&);
};

#endif

#endif//__MODEL_HPP_INCLUDED
