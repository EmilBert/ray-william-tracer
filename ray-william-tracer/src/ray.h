#pragma once

#include<glm/vec3.hpp>

class Ray {
	/* Ray::at Graphic

			 t=0		 t=1		 t=1
	----------|==========>|-----------|---->
			origin		vec3*t

	*/

public:
	Ray() {}
	Ray(const glm::vec3& origin, const glm::vec3& direction) : orig(origin), dir(direction) {}
	glm::dvec3 origin() const { return orig; }
	glm::dvec3 direction() const { return dir; }
	glm::dvec3 at(double t) const {
		return orig + t * dir;
	}

public:
	glm::dvec3 orig;
	glm::dvec3 dir;
};
