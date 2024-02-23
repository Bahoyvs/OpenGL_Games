#pragma once

#define V_D2R 0.0174532
#define V_R2D 57.29608323

typedef struct {
	double x, y;
} vec_t;

typedef struct {
	double magnitude, angle;
} polar_t;

double magV(vec_t v);
double angleV(vec_t v);
vec_t addV(vec_t v1, vec_t v2);
vec_t subV(vec_t v1, vec_t v2);
vec_t mulV(double k, vec_t v);
double dotP(vec_t v1, vec_t v2);
vec_t unitV(vec_t v);
vec_t pol2rec(polar_t p);
polar_t rec2pol(vec_t v);
double angleBetween2V(vec_t v1, vec_t v2);