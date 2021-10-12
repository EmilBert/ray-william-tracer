#pragma once

#include<glm/vec3.hpp>
#include<vector>

class Light {
public:
	// Assume non-skewed quads only!
	Light(std::vector<glm::dvec3> vertices, glm::dvec3 p); 

	const std::vector<glm::dvec3>& get_sample_points() { return sample_points; }
	glm::dvec3 position;

private:
	std::vector<glm::dvec3> sample_points; // Points ON the quad 
};