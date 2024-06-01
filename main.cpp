// author: 蔡品祥
//

#include "algebra3.cpp"
#include "ray.cpp"
#include "imageIO.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

//contruct input variable
float x, y, z, Sphere_radius, F_angle;
float r, g, b, spec, Reflect, Ka, Kd, Ks;
float Ax, Ay, Az, Bx, By, Bz, Cx, Cy, Cz;
vec3 eye_position, UL_output, UR_output, LL_output, LR_output, temS, V_dir, V_u, Light;
int R_width = 256, R_height = 256;
vector<vec3> Triangle;
vector<vec4> Sphere;
vector<float> material;
Pixel S, T, shadow = { 0, 0, 0 };


void phongmodel_Triangle(const vec3& a, const vec3& b, const vec3& c, const ray& ra) {
	auto S1 = b - a;
	auto S2 = c - a;
	vec3 cross = S1 ^ S2;
	if (cross * ra.direction()) { cross = cross * -1; }

	auto temd = cross * a;
	auto t = (temd - (ra.origin() * cross)) / (cross * ra.direction());
	auto point = ra.at(t);

	auto Hitp2L_dis = Light - point;
	Hitp2L_dis = Hitp2L_dis.normalize();
	auto normal = cross;
	auto hal = (point - ra.direction()) / 2;
	hal = hal.normalize();

	auto Ia = material[15];
	auto Id = normal * Hitp2L_dis;
	auto Is = pow(normal * hal, material[14]);

	auto Ambient = material[11] * Ia;
	auto Diffuse = material[12] * Id;
	auto Specular = material[13] * Is;
	auto color = Ambient + Diffuse + Specular;

	auto r_ = color * 255 * material[8];
	auto g_ = color * 255 * material[9];
	auto b_ = color * 255 * material[10];

	r_ = r_ > 255 ? 255 : r_; r_ = r_ < 0 ? 0 : r_;
	g_ = g_ > 255 ? 255 : g_; g_ = g_ < 0 ? 0 : g_;
	b_ = b_ > 255 ? 255 : b_; b_ = b_ < 0 ? 0 : b_;
	T.R = r_;
	T.G = g_;
	T.B = b_;

}


void phongmodel_Sphere(const ray& ra, const vec3& tems,const float radius) {
	auto A = ra.direction() * ra.direction();
	auto B = 2 * (ra.direction() * (ra.origin() - tems));
	auto C = (ra.origin() - tems) * (ra.origin() - tems) - radius*radius;
	auto t = (-B - sqrt(B * B - 4 * A * C)) / 2;
	
	auto hitpoint = ra.at(t);
	auto Hitp2L_dis = Light - hitpoint;
	Hitp2L_dis = Hitp2L_dis.normalize();
	auto normal = hitpoint - tems;
	normal = normal.normalize();

	auto hal = (hitpoint - ra.direction()) / 2;
	hal = hal.normalize();

	auto Ia = material[7];
	auto Id = normal * Hitp2L_dis;
	auto Is = pow(normal * hal, material[6]);

	auto Ambient = material[3] * Ia;
	auto Diffuse = material[4] * Id;
	auto Specular = material[5] * Is;
	auto color = Ambient + Diffuse + Specular;

	auto r_ = color * 255 * material[0];
	auto g_ = color * 255 * material[1];
	auto b_ = color * 255 * material[2];

	r_ = r_ > 255 ? 255 : r_; r_ = r_ < 0 ? 0 : r_;
	g_ = g_ > 255 ? 255 : g_; g_ = g_ < 0 ? 0 : g_;
	b_ = b_ > 255 ? 255 : b_; b_ = b_ < 0 ? 0 : b_;
	S.R = r_;
	S.G = g_;
	S.B = b_;


}

void readfile() {
	ifstream in_file("hw2_input.txt");
	if (!in_file.is_open()) { std::cerr << "failed to load file" << endl; } //check file load
	else{
		char category;
		while (in_file >> category)
		{
			switch (category)
			{
			case'E':
				in_file >> x >> y >> z;
				eye_position.set(x, y, z);
				break;
			case'V':
				in_file >> x >> y >> z;
				V_dir.set(x, y, z);
				in_file >> x >> y >> z;
				V_u.set(x, y, z);
				break;
			case'F':
				in_file >> F_angle;
				break;
			case'R':
				in_file >> R_width >> R_height;
				break;
			case'M':
				in_file >> r >> g >> b >> Ka >> Kd >> Ks >> spec >> Reflect;
				material.push_back(r);
				material.push_back(g);
				material.push_back(b);
				material.push_back(Ka);
				material.push_back(Kd);
				material.push_back(Ks);
				material.push_back(spec);
				material.push_back(Reflect);
				break;
			case 'S':
				in_file >> x >> y >> z >> Sphere_radius;
				Sphere.push_back(vec4(x, y, z, Sphere_radius));
				break;
			case'T':
				in_file >> Ax >> Ay >> Az >> Bx >> By >> Bz >> Cx >> Cy >> Cz;
				Triangle.push_back(vec3(Ax, Ay, Az));
				Triangle.push_back(vec3(Bx, By, Bz));
				Triangle.push_back(vec3(Cx, Cy, Cz));
				break;
			case'L':
				in_file >> x >> y >> z;
				Light.set(x, y, z);
				break;
			}
		}
	}
	in_file.close();
}

bool hit_sphere(const vec3& center, float radius, const ray& r) {
	vec3 dif = r.origin() - center;
	auto b = 2 * (dif * r.direction());
	auto a = r.direction() * r.direction();
	auto c = dif * dif - radius * radius;
	auto judgmental = b * b - 4 * a * c;
	return (judgmental >= 0);
}

bool hit_tri(const vec3& a, const vec3& b, const vec3& c, const ray& r) {
	vec3 S1 = b - a;
	vec3 S2 = c - a;
	vec3 cross = S1 ^ S2;

	if (cross == vec3(0, 0, 0) || r.direction() * cross == 0) { return 0; }

	auto temd = cross * a;
	auto f = temd - (r.origin() * cross);
	auto g = cross * r.direction();
	auto t = f / g;
	if (t < 0.0) { return 0; }


	vec3 point = t * r.direction() + r.origin();
	vec3 S3 = point - a, S4 = b - point, S5 = c - point;
	vec3 vec12 = S1 ^ S2, vec13 = S1 ^ S3, vec23 = S2 ^ S3, vec45 = S4 ^ S5;
	float tri12 = vec12.length() / 2;
	float tri13 = vec13.length() / 2;
	float tri23 = vec23.length() / 2;
	float tri45 = vec45.length() / 2;

	if ((tri13 + tri23) > tri12 || (tri13 + tri23 + tri45) > tri12) { return 0; }
	return 1;
}


int main()
{
	readfile();
	ray traced_ray(eye_position);
	traced_ray.setxy(V_dir, V_u, R_width, R_height, F_angle);

	ColorImage image;
//	Pixel W = { 255,255,255 };
	image.init(R_width, R_height);

	for (int j = 0; j < R_height; j++){
		for (int i = 0; i < R_width; i++) {
			traced_ray.ray_set(i, j);

			for (unsigned int s = 0; s < Sphere.size(); s++) {
				temS.set(Sphere[s][0], Sphere[s][1], Sphere[s][2]);
				Sphere_radius = Sphere[s][3];
				if (hit_sphere(temS, Sphere_radius, traced_ray)) {
					phongmodel_Sphere(traced_ray, temS, Sphere_radius);
					image.writePixel(i, j, S);
				}
			}

			for (unsigned int t = 0; t < Triangle.size(); t += 3) {
				if (hit_tri(Triangle[t], Triangle[t + 1], Triangle[t + 2], traced_ray)) {
					phongmodel_Triangle(Triangle[t], Triangle[t + 1], Triangle[t + 2], traced_ray);
					image.writePixel(i, j, T);
				}
			}
		}
	}
	
	char outputname[28] = "image_output/hw2_output.ppm";
	image.outputPPM(outputname);
	return 0;
}

