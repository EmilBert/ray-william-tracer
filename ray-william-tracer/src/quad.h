#pragma once

#include"hittable.h"
#include"vec3.h"
#include"material.h"
#include"triangle.h"

class Quad : public Hittable {
public:
	Quad() {}
	//Quad(Point3 p0, Point3 p1, Point3 p2, Point3 p3, shared_ptr<Material> m) : v0(p0), v1(p1), v2(p2), v3(p3), t0(p0, p2, p1, m), t1(p3, p1, p2, m), mat_ptr(m) {}
	Quad(Point3 p0, Point3 p1, Point3 p2, Point3 p3, shared_ptr<Material> m) : t0(p0, p2, p1, m), t1(p3, p1, p2, m), mat_ptr(m) {}
	//Quad(Point3 origin, double width, double height, double xRot, double yRot, shared_ptr<Material> m) {}

	virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const override;

private:

	/*
		Quad visual

		v0 ------ v1
		|	   /  |     t0(v0,v1,v2)
		|    /    |		
		|  / 	  |		t1(v3,v1,v2)
		v2 ------ v3
	*/
	Triangle t0, t1;
	Point3 v0, v1, v2, v3; // Triangle vertices
	shared_ptr<Material> mat_ptr;
};

/*
Quad::Quad(Point3 origin, double width, double height, double xRot = 0, double yRot = 0, shared_ptr<Material> m) {
	
}
*/

// Algorithm used: Möller Trumbore method
bool Quad::hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const
{
	if(t0.hit(ray, t_min, t_max, rec)) return true;
	if(t1.hit(ray, t_min, t_max, rec)) return true;

	return false;
}