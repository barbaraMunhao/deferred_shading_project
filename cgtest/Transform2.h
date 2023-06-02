#ifndef __Transform2_h
#define __Transform2_h

#include "math/Matrix3x3.h"

namespace cg
{ // begin namespace cg

class Transform2
{
public:
  vec2f position{vec2f::null};

  Transform2():
    position(vec2f::null()),
    _angle{0},
    _rotation{1, 0}
  {
    // do nothing
  }

  auto angle() const
  {
    return _angle;
  }

  void setAngle(float angle)
  {
    _angle = angle;
    _rotation.set(cos(angle), sin(angle));
  }

  vec2f transform(const vec2f& p) const
  {
    auto x = _rotation.x * p.x - _rotation.y * p.y + position.x;
    auto y = _rotation.y * p.x + _rotation.x * p.y + position.y;

    return vec2f{x, y};
  }

  vec2f transformVector(const vec2f& v) const
  {
    auto x = _rotation.x * v.x - _rotation.y * v.y;
    auto y = _rotation.y * v.x + _rotation.x * v.y;

    return vec2f{x, y};
  }

private:
  float _angle;
  vec2f _rotation;

}; // Transform2

} // end namespace cg

#endif // __Transform2_h