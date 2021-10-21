#include"quad.h"

#include<glm/geometric.hpp>

glm::dvec2 Quad::getUV(const glm::dvec3& p)
{
	double dist_x = glm::distance(tl, tr);
	double dist_y = glm::distance(tl, bl);

	auto res = glm::dvec2(0, 0);

	// u
	glm::dvec3 temp = { p.x, tl.y, tl.z };
	res.x = glm::distance(tl, temp) / dist_x;

	// v
	temp = { tl.x, p.y , tl.z };
	res.y = 1-  glm::distance(tl, temp) / dist_y;

	return res;
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
