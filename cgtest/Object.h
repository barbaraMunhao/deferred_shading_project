#ifndef _OBJECT_H
#define _OBJECT_H

#include "Transform.h"
#include "geometry/TriangleMesh.h"
#include "graphics/Material.h"

namespace cg
{ // begin namespace cg

class Object: public SharedObject
{
public:
	Object(cg::Reference<cg::TriangleMesh> mesh, cg::Material* mat) 
	{
		_mesh = mesh;
		_material = mat;
	}

	

	auto& mesh(void)
	{
		return _mesh;
	}

	auto material(void)
	{
		return _material;
	}

	auto& transform()
	{
		return _transform;
	}

private:
	cg::Reference<cg::TriangleMesh> _mesh;
	cg::Material* _material;
	Transform _transform{};
};



} // end namespace cg

#endif // __Shape2_h