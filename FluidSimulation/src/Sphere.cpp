#include "RenderChimp.h"
#include "Ray.h"
#include "Plane.h"
#include "Volume.h"
#include "AABox.h"
#include "Sphere.h"

/*---------------------------------------------------------------------------*/

void Sphere::setRadius(
		f32				nradius
	)
{
	radius = nradius;
	radiusSquared = nradius * nradius;

}

/*---------------------------------------------------------------------------*/

bool Sphere::encloses(
		vec3f			*point
	)
{
	return (*point - origin).length2() <= radiusSquared;
}

/*---------------------------------------------------------------------------*/

vec3f Sphere::getCentrum()
{
	return origin;
}

/*---------------------------------------------------------------------------*/

f32 Sphere::volume()
{
	return (4.0f * fPI * radiusSquared * radius) / 3.0f;
}

/*---------------------------------------------------------------------------*/

bool Sphere::intersect(
		Ray				*ray
	)
{
	/* TODO: Implement me! */
	return false;
}

/*---------------------------------------------------------------------------*/

bool Sphere::intersect(
		Ray				*ray,
		f32				*dist
	)
{
	/* TODO: Implement me! */
	*dist = 0.0f;
	return true;
}

/*---------------------------------------------------------------------------*/

bool Sphere::intersect(
		AABox			*aabox
	)
{
	return aabox->intersect(this);
}

/*---------------------------------------------------------------------------*/

bool Sphere::intersect(
		Sphere			*sphere
	)
{
	f32 r = sphere->radius + radius;
	r *= r;

	return (sphere->origin - origin).length2() <= r;
}

/*---------------------------------------------------------------------------*/

void Sphere::calculateBounds(
		VertexArray		*vertexArray,
		f32				overCompensate = 0.0f
	)
{
	/* TODO: Remaster */
#if 0
	f32 *va = vertexArray->getArray();
	u32 stride = vertexArray->getStride();
	u32 offset = vertexArray->getAttributeOffset("Vertex");
	u32 n = vertexArray->getLength();
	u32 i;
	f32 *f;

	origin = vec3f(0.0f, 0.0f, 0.0f);
	radiusSquared = 0.0f;

	if (offset == -1 || n == 0) {
		radius = 0.0f;
		return;
	}

	f = &va[offset];

	for (i = 0; i < n; i++) {

		origin += vec3f(f[0], f[1], f[2]);

		f += stride;

	}

	origin /= (f32) n;


	f = &va[offset];

	for (i = 0; i < n; i++) {

		vec3f p = vec3f(f[0], f[1], f[2]);

		f32 r = (p - origin).length2();

		radiusSquared = r > radiusSquared ? r : radiusSquared;

		f += stride;

	}

	radius = sqrt(radiusSquared);
#endif

}

/*---------------------------------------------------------------------------*/

void Sphere::combine(
		Sphere			*sphere
	)
{
	/* TODO: Implement me! */
}

/*---------------------------------------------------------------------------*/

