#include "Line2.h"

namespace cg
{ // begin namespace cg

Bounds2f
Line2::bounds(Transform2& t) const
{
  Bounds2f b;
  b.inflate(t.transform(p1));
  b.inflate(t.transform(p2));
  return b;
}

void
Line2::draw(GLGraphics2& g2, const Transform2& t) const
{
  g2.setLineColor(_color);
  g2.setPolygonMode(GLGraphics2::LINE);
  g2.drawLine(t.transform(p1), t.transform(p2));
}

Line2
Line2::transform(Transform2& t) const
{
  return Line2{ t.transform(this->p1), t.transform(this->p2) };
}

} // end namespace cg
