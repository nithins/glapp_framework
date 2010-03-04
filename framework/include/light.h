#ifndef __LIGHT_H_INCLUDED
#define __LIGHT_H_INCLUDED

#include <model.h>


#if !defined(GLAFW_GENERATING_PYTHON_BINDINGS)

class IFramework;

class ILightManager: virtual public IModel
{

  public:
    static ILightManager * create ( int i, IFramework * );
    static void            destroy ( ILightManager *& );

    virtual ~ILightManager() {}
};
#endif

#endif// __LIGHT_H_INCLUDED
