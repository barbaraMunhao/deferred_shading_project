#ifndef __Shape2_h
#define __Shape2_h

#include "graphics/GLGraphics2.h"
#include "Transform2.h"

namespace cg
{ // begin namespace cg

template <int D> class Shape;

template <>
class Shape<2>: public SharedObject
{
public:
  virtual Bounds2f bounds(Transform2&) const = 0;
  virtual void draw(GLGraphics2& g2, const Transform2&) const = 0;
  Color _color;
}; // Shape

using Shape2 = Shape<2>;

} // end namespace cg

#endif // __Shape2_h