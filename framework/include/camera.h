#ifndef __CAMERA_H_INCLUDED__
#define __CAMERA_H_INCLUDED__

#if !defined(GLAFW_GENERATING_PYTHON_BINDINGS)

class Camera
{
protected:
    Camera(){}

public:
  virtual ~Camera(){}

  virtual void DrawFocus(){}
  virtual void SetPerspective(){}
  virtual void SetOrthographic(){}
  virtual void Set(){}
  virtual void MoveLR(int ){}
  virtual void MoveUD(int ){}
  virtual void MoveFB(int ){}
  virtual void StrafeLR(int ){}
  virtual void StrafeUD(int ){}
  virtual void StrafeFB(int ){}
  virtual void Twist(int ){}
  virtual void Reset(){}

  static Camera* Create();
  static void    Delete(Camera* &);

};

#endif

#endif
