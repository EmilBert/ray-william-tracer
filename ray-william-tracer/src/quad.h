#pragma once

#include"hittable.h"
#include"material.h"
#include"triangle.h"

#include"texture.h"

#include<glm/vec3.hpp>
#include<vector>

/*
	Quad visual

	v0 ------ v1
	|	   /  |     t0(v0,v1,v2)
	|    /    |		
	|  / 	  |		t1(v3,v1,v2)
	v2 ------ v3
*/

class Quad : public Hittable {
public:
	Quad() {}
	Quad(glm::dvec3 tl, glm::dvec3 tr, glm::dvec3 bl, glm::dvec3 br, shared_ptr<Material> m) : 
		t0(tl, bl, tr, m), t1(br, tr, bl, m), mat_ptr(m), bl(bl), br(br), tr(tr), tl(tl)
	{}	
	Quad(glm::dvec3 tl, glm::dvec3 tr, glm::dvec3 bl, glm::dvec3 br, shared_ptr<Material> m, std::shared_ptr<Texture> t) : 
		t0(tl, bl, tr, m), t1(br, tr, bl, m), mat_ptr(m), bl(bl), br(br), tr(tr), tl(tl), texture_ptr(t)
	{}

	glm::dvec2 getUV(const glm::dvec3& p) override;
	virtual Texture* getTexture() const;

	virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const override;
	std::vector<glm::dvec3> give_me_vertices() { return { tl, tr, bl, br }; }

private:

	Triangle t0, t1;
	glm::dvec3 tl, tr, bl, br; // Vertices
	shared_ptr<Material> mat_ptr;
	shared_ptr<Texture> texture_ptr;
};