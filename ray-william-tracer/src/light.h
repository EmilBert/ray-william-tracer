#pragma once

#include<glm/vec3.hpp>
#include<vector>

class Light {
public:
	// Assume non-skewed quads only!
	Light(std::vector<glm::dvec3> vertices); 

	const std::vector<glm::dvec3>& get_sample_points() { return sample_points; }

private:
	std::vector<glm::dvec3> sample_points; // Points ON the quad 
};