// Transform.cpp: implementation of the Transform class.
// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR

#include "Transform.h"
#include <iostream>

mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
	const float rad = glm::radians(degrees);
	vec3 axisnorm = glm::normalize(axis);
	const float x = axisnorm[0], y = axisnorm[1], z = axisnorm[2];
	mat3 IMat(1.0f);
	mat3 Mat2(x*x, x*y, x*z, x*y, y*y, y*z, z*x, z*y, z*z);
	mat3 Mat3(0.0f, z, -y, -z, 0.0f, x, y, -x, 0.0f);

	return cos(rad)*IMat + (1 - cos(rad))*Mat2 + sin(rad)*Mat3;
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
	eye = rotate(degrees, up) * eye;
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{
	vec3 left = glm::normalize(glm::cross(eye, up));
	eye = rotate(degrees, left) * eye;
	up = rotate(degrees, left) * up;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
    mat4 ret(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1);
    return ret;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
    mat4 ret(1.0);
    ret[3] = vec4(tx,ty,tz,1.0f);
    return ret;
}

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
    vec3 x = glm::cross(up,zvec); 
    vec3 y = glm::cross(zvec,x); 
    vec3 ret = glm::normalize(y); 
    return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
