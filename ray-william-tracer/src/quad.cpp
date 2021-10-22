#include"quad.h"

#include<glm/geometric.hpp>

glm::dvec2 Quad::getUV(const glm::dvec3& p) {
	// Formula used: https://en.wikipedia.org/wiki/Vector_projection#Vector_projection_2

	// U
	auto ab = (tr - tl);
	auto ap = (p - tl);
	auto projected = tl + (glm::dot(ap, ab) / glm::dot(ab, ab)) * ab;

	double u = glm::distance(tl, projected) / glm::distance(tl, tr);

	// V
	ab = (bl - tl);
	projected = tl + (glm::dot(ap, ab) / glm::dot(ab, ab)) * ab;
	double v = 1 - glm::distance(tl, projected) / glm::distance(tl, bl);

	return { u,v };
}


Texture* Quad::getTexture() const
{
	return texture_ptr.get();
}

bool Quad::hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const
{
	if (t0.hit(ray, t_min, t_max, rec)) return true;
	if (t1.hit(ray, t_min, t_max, rec)) return true;

	return false;
}
