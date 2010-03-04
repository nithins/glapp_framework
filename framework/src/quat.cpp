#include <quat.h>
#include <math.h>
#include <vecutils.h>

/*
 * Trackball code:
 *
 * Implementation of a virtual trackball.
 * Implemented by Gavin Bell, lots of ideas from Thant Tessman and
 *   the August '88 issue of Siggraph's "Computer Graphics," pp. 121-129.
 *
 * Vector manip code:
 *
 * Original code from:
 * David M. Ciemiewicz, Mark Grossman, Henry Moreton, and Paul Haeberli
 *
 * Much mucking with by:
 * Gavin Bell
 *
 * Nithin: Lifted only the quat part ..;) felt lazy to write my own
 */

/*
 * Local function prototypes (not defined in trackball.h)
 */

//static void normalize_quat(float [4]);

void
vzero ( float *v )
{
  v[0] = 0.0;
  v[1] = 0.0;
  v[2] = 0.0;
}

void
vset ( float *v, float x, float y, float z )
{
  v[0] = x;
  v[1] = y;
  v[2] = z;
}

void
vsub ( const float *src1, const float *src2, float *dst )
{
  dst[0] = src1[0] - src2[0];
  dst[1] = src1[1] - src2[1];
  dst[2] = src1[2] - src2[2];
}

void
vcopy ( const float *v1, float *v2 )
{
  register int i;

  for ( i = 0 ; i < 3 ; i++ )
    v2[i] = v1[i];
}

void
vcross ( const float *v1, const float *v2, float *cross )
{
  float temp[3];

  temp[0] = ( v1[1] * v2[2] ) - ( v1[2] * v2[1] );
  temp[1] = ( v1[2] * v2[0] ) - ( v1[0] * v2[2] );
  temp[2] = ( v1[0] * v2[1] ) - ( v1[1] * v2[0] );
  vcopy ( temp, cross );
}

float
vlength ( const float *v )
{
  return ( float ) sqrt ( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

void
vscale ( float *v, float div )
{
  v[0] *= div;
  v[1] *= div;
  v[2] *= div;
}

void
vnormal ( float *v )
{
  vscale ( v, 1.0f / vlength ( v ) );
}

float
vdot ( const float *v1, const float *v2 )
{
  return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void
vadd ( const float *src1, const float *src2, float *dst )
{
  dst[0] = src1[0] + src2[0];
  dst[1] = src1[1] + src2[1];
  dst[2] = src1[2] + src2[2];
}

/*
 *  Given an axis and angle, compute quaternion.
 */
void
axis_to_quat ( float *a, const  float &phi, float *q )
{
  vnormal ( a );
  vcopy ( a, q );
  vscale ( q, ( float ) sin ( phi / 2.0f ) );
  q[3] = ( float ) cos ( phi / 2.0f );
}

void axis_to_quat(float *a ,const float &sintheta, const float &costheta,float *q)
{
  vnormal ( a );
  vcopy ( a, q );
  vscale ( q, ( float ) sintheta/2.0 );
  q[3] = ( float ) costheta/2.0;
}


/*
 * Given two rotations, e1 and e2, expressed as quaternion rotations,
 * figure out the equivalent single rotation and stuff it into dest.
 *
 * This routine also normalizes the result every RENORMCOUNT times it is
 * called, to keep error from creeping in.
 *
 * NOTE: This routine is written so that q1 or q2 may be the same
 * as dest (or each other).
 */

void
add_quats ( const float *q1, const float *q2, float *dest )
{
  float t1[4], t2[4], t3[4];
  float tf[4];

  vcopy ( q1, t1 );
  vscale ( t1, q2[3] );

  vcopy ( q2, t2 );
  vscale ( t2, q1[3] );

  vcross ( q2, q1, t3 );
  vadd ( t1, t2, tf );
  vadd ( t3, tf, tf );
  tf[3] = q1[3] * q2[3] - vdot ( q1, q2 );

  dest[0] = tf[0];
  dest[1] = tf[1];
  dest[2] = tf[2];
  dest[3] = tf[3];

}

/*
 * Quaternions always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
 * If they don't add up to 1.0, dividing by their magnitued will
 * renormalize them.
 *
 * Note: See the following for more information on quaternions:
 *
 * - Shoemake, K., Animating rotation with quaternion curves, Computer
 *   Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
 * - Pletinckx, D., Quaternion calculus as a basic tool in computer
 *   graphics, The Visual Computer 5, 2-13, 1989.
 */
// static void
// normalize_quat(float *q)
// {
//     int i;
//     float mag;

//     mag = (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
//     for (i = 0; i < 4; i++) q[i] /= mag;
// }

/*
 * Build a rotation matrix, given a quaternion rotation.
 *
 */
void
build_rotmatrix ( float *m, const float *q )
{
  m[0] = 1.0f - 2.0f * ( q[1] * q[1] + q[2] * q[2] );
  m[1] = 2.0f * ( q[0] * q[1] - q[2] * q[3] );
  m[2] = 2.0f * ( q[2] * q[0] + q[1] * q[3] );
  m[3] = 0.0f;

  m[4] = 2.0f * ( q[0] * q[1] + q[2] * q[3] );
  m[5] = 1.0f - 2.0f * ( q[2] * q[2] + q[0] * q[0] );
  m[6] = 2.0f * ( q[1] * q[2] - q[0] * q[3] );
  m[7] = 0.0f;

  m[8] = 2.0f * ( q[2] * q[0] - q[1] * q[3] );
  m[9] = 2.0f * ( q[1] * q[2] + q[0] * q[3] );
  m[10] = 1.0f - 2.0f * ( q[1] * q[1] + q[0] * q[0] );
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}



Quat::Quat()
{
  Reset();

}

Quat::Quat ( float *q )
{
  m_q[0] = q[0];
  m_q[1] = q[1];
  m_q[2] = q[2];
  m_q[3] = q[3];

}

Quat::Quat ( const float &theta, float *axes )
{
  axis_to_quat ( axes, theta, m_q );
}

Quat::Quat ( const float &sintheta,const float &costheta, float *axes )
{
  axis_to_quat ( axes, sintheta,costheta, m_q );
}


Quat::Quat ( const Quat& q )
{
  *this = q;
}

void Quat::operator= ( const Quat & q )
{
  m_q[0] = q.m_q[0];
  m_q[1] = q.m_q[1];
  m_q[2] = q.m_q[2];
  m_q[3] = q.m_q[3];
}

void Quat::operator*= ( const Quat & q )
{
  add_quats ( m_q, q.m_q, m_q );
}

void Quat::GetMat ( float *mat ) const
{
  build_rotmatrix ( mat, m_q );
}

void Quat::GetMat ( double *mat ) const
{
  float mat_tmp[16];
  build_rotmatrix ( mat_tmp, m_q );

  COPYVEC4 ( ( mat + 0 ), ( mat_tmp + 0 ) );
  COPYVEC4 ( ( mat + 4 ), ( mat_tmp + 4 ) );
  COPYVEC4 ( ( mat + 8 ), ( mat_tmp + 8 ) );
  COPYVEC4 ( ( mat + 12 ), ( mat_tmp + 12 ) );
}

void Quat::GetInvMat ( float *mat ) const
{
  float q_inv[4] = { -m_q[0], -m_q[1], -m_q[2], m_q[3]};
  build_rotmatrix ( mat, q_inv );
}

void Quat::GetInvMat ( double *mat ) const
{
  float q_inv[4] = { -m_q[0], -m_q[1], -m_q[2], m_q[3]};
  float mat_tmp[16];
  build_rotmatrix ( mat_tmp, q_inv );

  COPYVEC4 ( ( mat + 0 ), ( mat_tmp + 0 ) );
  COPYVEC4 ( ( mat + 4 ), ( mat_tmp + 4 ) );
  COPYVEC4 ( ( mat + 8 ), ( mat_tmp + 8 ) );
  COPYVEC4 ( ( mat + 12 ), ( mat_tmp + 12 ) );


}

void Quat::Reset()
{
  m_q[0] = 0;
  m_q[1] = 0;
  m_q[2] = 0;
  m_q[3] = 1;
}



Quat operator* ( const Quat &q1, const Quat &q2 )
{
  Quat t;

  t  = q1;
  t *= q2;
  return t;
}

Quat operator- ( const Quat &q1 )
{
  Quat t;

  t  = q1;
  t.m_q[0] *= -1;
  t.m_q[1] *= -1;
  t.m_q[2] *= -1;

  return t;
}

using namespace std;

ostream& operator<< ( ostream &o, const Quat &q1 )
{
  o << "(" << q1.m_q[0] << " " << q1.m_q[1] << " " << q1.m_q[2] << ")" << q1.m_q[3];
  return o;
}




