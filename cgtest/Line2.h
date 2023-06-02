#ifndef __Line2_h
#define __Line2_h

#include "Shape2.h"

namespace cg
{ // begin namespace cg

class Line2: public Shape2
{
public:
  vec2f p1;
  vec2f p2;

  Line2() = default;

  Line2(const vec2f& p1, const vec2f& p2):
    p1{p1},
    p2{p2}
  {
    // do nothing
  }

  Bounds2f bounds(Transform2&) const override;
  Line2 transform(Transform2&) const;
  void draw(GLGraphics2& g2, const Transform2&) const override;
}; // Line2

} // end namespace cg

#endif // __Line2_h