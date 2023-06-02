#ifndef __RigidBody2_h
#define __RigidBody2_h

#include "Shape2.h"
#include "Transform2.h"

namespace cg
{ // begin namespace cg

template <int D> class RigidBody;

template <>
class RigidBody<2>: public SharedObject
{
public:
  Transform2 pose;
  vec2f v{vec2f::null()}; // linear velocity
  float w{}; // angular velocity
  void* userData{};

  RigidBody(Shape2* shape):
    _shape{shape}
  {
    //pose.position = shape->centerOfMass();
  }

  Shape2* shape() const
  {
    return _shape;
  }

private:
  Reference<Shape2> _shape;

}; // RigidBody

using RigidBody2 = RigidBody<2>;

} // end namespace cg

#endif // __RigidBody2_h