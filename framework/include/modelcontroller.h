#ifndef __MODELCONTROLLER_H_INCLUDED__
#define __MODELCONTROLLER_H_INCLUDED__
// not be exported to python

#if !defined(GLAFW_GENERATING_PYTHON_BINDINGS)

#include <iostream>

class IModelController
{
  public:

    typedef unsigned int uint;
    virtual ~IModelController() {}

    virtual void set_pos ( const float * ) = 0;
    virtual bool look_at ( const float *, const float *, const float * ) = 0;


    virtual void StartTB ( const uint &, const uint & ) = 0;
    virtual void StopTB ( const uint &, const uint & ) = 0;
    virtual void StartTrans ( const uint &, const uint & ) = 0;
    virtual void StopTrans ( const uint &, const uint & ) = 0;
    virtual void Move ( const uint &, const uint & ) = 0;
    virtual void Render() const = 0;
    virtual void Reset() = 0;
    virtual void getMatrix ( double * ) = 0;
    virtual void   set_uniform_scale(double ) = 0;
    virtual double get_uniform_scale() = 0;

    inline void set_pos ( const float &v1, const float &v2, const float &v3 )
    {
      float v[] = {v1, v2, v3};
      set_pos ( v );
    }

    inline bool look_at
    ( const float &e1, const float &e2, const float &e3 ,
      const float &c1, const float &c2, const float &c3 ,
      const float &u1, const float &u2, const float &u3 )
    {
      float e[] = {e1, e2, e3};
      float c[] = {c1, c2, c3};
      float u[] = {u1, u2, u3};
      return look_at ( e, c, u );
    }

    virtual void serialize(std::ostream &) = 0;
    virtual void serialize(std::istream &) = 0;

    static IModelController* Create();
    static void Delete ( IModelController* );

};

#endif

#endif
