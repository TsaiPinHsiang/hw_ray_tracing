#include "algebra3.h"
#include <iostream>
#include <math.h>
#define PI 3.1415926535898
#define Tan(th) tan(PI/180*(th))


class ray {
	public:
		ray(){}
		ray(const vec3& origin) :orig(origin), dir(vec3(0, 0, 0)), t(0)
		{}
		ray(const vec3& origin, const vec3& direction): orig(origin), dir(direction), t(0)
		{}
		ray(const vec3& origin, const vec3& direction, float time) : orig(origin), dir(direction), t(time)
		{}

		void setxy(vec3& V_dir, vec3& V_ver, const float& width, const float& height, const float& angle) {

			auto horizon = V_ver ^ V_dir;
			V_dir = V_dir.normalize();
			horizon = horizon.normalize();
			V_ver = V_ver.normalize();

			x = horizon * (V_dir.length() * Tan(angle / 2) / (width / 2));
			y = V_ver * (V_dir.length() * Tan(angle / 2) / (height / 2));
			
			vec = V_dir - width / 2 * x + height / 2 * y;
		}

		void ray_set(const int i, const int j) {
			dir = vec + i * x - j * y;
			dir = dir.normalize();
		}
		vec3 origin() const { return orig; }
		vec3 direction() const { return dir; }
		float time() const { return t; }

		vec3 at(float tm) const {
			return orig + tm * dir;
		}

	public:
		vec3 vec;
		vec3 dir;
		vec3 orig;
		vec3 x;
		vec3 y;
		float t;
};
