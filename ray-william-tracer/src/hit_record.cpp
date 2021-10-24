#include"hit_record.h"

#include<glm/geometric.hpp>
#include<glm/vec3.hpp>

#include<iostream>

void hit_record::set_face_normal(const Ray& ray, const glm::dvec3& outward_normal)
{
	front_face = glm::dot(ray.direction(), outward_normal) < 0;
	normal = front_face ? outward_normal : glm::dvec3(-outward_normal.x, -outward_normal.y, -outward_normal.z);
}
