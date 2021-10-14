#include"light.h"

#include<glm/geometric.hpp>
#include<iostream>

/*
	Quad visual

	v0 ------ v1
	|	   /  |     t0(v0,v1,v2)
	|    /    |
	|  / 	  |		t1(v3,v1,v2)
	v2 ------ v3
*/

Light::Light(std::vector<glm::dvec3> vertices, glm::dvec3 p) : position(p), 
	t0({ vertices[0], vertices[1], vertices[2] }, { 0,0,0 }, nullptr),
	t1({ vertices[3], vertices[1], vertices[2] }, { 0,0,0 }, nullptr),
	vertices(vertices)
{

	// generate samplePoints
	double const eps = 0.1;
	int numberOfSamples = 20;

	glm::dvec3 v0v1 = glm::normalize(vertices[1] - vertices[0]);
	glm::dvec3 v0v2 = glm::normalize(vertices[2] - vertices[0]);
	glm::dvec3 v2v3 = glm::normalize(vertices[3] - vertices[0]);

	// Walk p01 until we hit vertices[1], starting at vertices[0]
	glm::dvec3 p01 = vertices[0];
	glm::dvec3 p23 = vertices[2];
	while (glm::distance(p01, vertices[1]) > eps) {
		p01 += (1.0 / numberOfSamples) * v0v1;
		p23 += (1.0 / numberOfSamples) * v2v3;
		
		glm::dvec3 topToBot = p01;
		glm::dvec3 finishedPosition = p23;
		while (glm::distance(topToBot, p23) > eps) {
			topToBot += (1.0 / numberOfSamples) * glm::normalize(p23-topToBot);
			sample_points.push_back(topToBot);
		}
	}
}

bool Light::hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const
{
	return t0.hit(ray, t_min, t_max, rec) || t1.hit(ray, t_min, t_max, rec);
}

// Get random position inside Light Quad
glm::dvec3 Light::getRandomPosition() const
{
	double d1 = random_double();
	double d2 = random_double();

	glm::dvec3 v0v1 = (vertices[1] - vertices[0]);
	glm::dvec3 v0v2 = (vertices[2] - vertices[0]);

	return vertices[0] + d1 * v0v1 + d2 * v0v2;
}
