#ifndef __QUAT_CPP_DEFINED_
#define __QUAT_CPP_DEFINED_

#include <iostream>

class Quat
{
    float m_q[4];

  public:
    Quat();
    Quat ( const Quat& q );
    Quat ( const float &theta, float *axes );
    Quat ( const float &sintheta,const float &costheta, float *axes );
    Quat ( float *q );


    void operator= ( const Quat &q );
    void operator*= ( const Quat & q );

    void GetMat ( float *mat ) const;
    void GetMat ( double *mat ) const;
    void GetInvMat ( float *mat ) const;
    void GetInvMat ( double *mat ) const;

    void Reset();



    friend Quat operator* ( const Quat &q1, const Quat &q2 );

    friend Quat operator- ( const Quat &q1 );

    friend std::ostream& operator<< ( std::ostream &, const Quat & );

    void get_data ( float *p )
    {
      p[0] = m_q[0];
      p[1] = m_q[1];
      p[2] = m_q[2];
      p[3] = m_q[3];

    }



};

#endif
