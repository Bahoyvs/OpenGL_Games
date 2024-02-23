#include "vec.h"
#include <math.h>

double magV(vec_t v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

double angleV(vec_t v) {
	double angle = atan2(v.y, v.x) * V_R2D;
	return angle < 0 ? angle + 360 : angle;
}

vec_t addV(vec_t v1, vec_t v2) {
	return{ v1.x + v2.x, v1.y + v2.y };
}

vec_t subV(vec_t v1, vec_t v2) {
	return{ v1.x - v2.x, v1.y - v2.y };
}

vec_t mulV(double k, vec_t v) {
	return{ k * v.x, k * v.y };
}

double dotP(vec_t v1, vec_t v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

vec_t unitV(vec_t v) {
	return mulV(1.0 / magV(v), v);
}

// convert from polar representation to rectangular representation
vec_t pol2rec(polar_t p) {
	return{ p.magnitude * cos(p.angle * V_D2R),  p.magnitude * sin(p.angle * V_D2R) };
}

polar_t rec2pol(vec_t v) {
	return{ magV(v), angleV(v) };
}

double angleBetween2V(vec_t v1, vec_t v2) {
	double magV1 = magV(v1);
	double magV2 = magV(v2);
	double dot = dotP(v1, v2);
	double angle = acos(dot / (magV1 * magV2)) * V_R2D; // in degree
	return angle;
}
