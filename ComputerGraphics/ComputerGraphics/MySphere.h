#ifndef __MY_SPHERE_H__
#define __MY_SPHERE_H__

#include <vgl.h>
#include <vec.h>
#include <InitShader.h>
#include <math.h>
#include <stdio.h>

struct MySphereVertex
{
	vec4 position;
	vec4 color;
};

class MySphere
{
public:
	int la; // latitude 위도
	int lo; // longitude 경도
	int numVertices;

	GLuint vao;
	GLuint vbo;
	GLuint prog;

	MySphere(int _la, int _lo)
	{
		la = _la;
		lo = _lo;
	}

	void init()
	{

	}
}

#endif