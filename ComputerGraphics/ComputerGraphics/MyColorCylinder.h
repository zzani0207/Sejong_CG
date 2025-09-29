#ifndef __MY_COLOR_CYLINDER_H__
#define __MY_COLOR_CYLINDER_H__

#include <vgl.h>
#include <vec.h>
#include <InitShader.h>
#include <math.h>

struct MyCylinderVertex
{
	vec4 position;
	vec4 color;

};

class MyColorCylinder
{
public:
	int m_div;
	int m_numVertices;
	//MyCylinderVertex* vertices;

	GLuint vao;
	GLuint vbo;
	GLuint prog;
	MyColorCylinder()
	{
		m_div = 10;
		m_numVertices = m_div * 6;
	}
	void init(int div)
	{
		m_div = div;
		m_numVertices = m_div * 6;
		MyCylinderVertex* data = new MyCylinderVertex[m_numVertices];

		float r = 0.5f;
		int curIdx = 0;
		for (int i = 0; i < m_div; i++)
		{
			float ang1 = (i * 360.0f / m_div) * DegreesToRadians;
			float ang2 = ((i + 1) * 360.0f / m_div) * DegreesToRadians;
			float x1 = r*cos(ang1), y1 = r*sin(ang1), x2 = r*cos(ang2), y2 = r*sin(ang2);

			vec4 a = vec4(x1, y1, 0.5, 1);
			vec4 b = vec4(x1, y1, -0.5, 1);
			vec4 c = vec4(x2, y2, -0.5, 1);
			vec4 d = vec4(x2, y2, 0.5, 1);

			data[curIdx].position = a; data[curIdx].color = vec4(1, 0, 0, 1); curIdx++;
			data[curIdx].position = b; data[curIdx].color = vec4(0, 1, 0, 1); curIdx++;
			data[curIdx].position = c; data[curIdx].color = vec4(0, 0, 1, 1); curIdx++;

			data[curIdx].position = a; data[curIdx].color = vec4(1, 0, 0, 1); curIdx++;
			data[curIdx].position = c; data[curIdx].color = vec4(0, 0, 1, 1); curIdx++;
			data[curIdx].position = d; data[curIdx].color = vec4(1, 1, 0, 1); curIdx++;
		}

		// 1. send data to GPU
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(MyCylinderVertex) * m_numVertices + sizeof(MyCylinderVertex) * m_numVertices, nullptr, GL_STATIC_DRAW);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * numVertices, points);
		//glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * numVertices, sizeof(vec4) * numVertices, colors);

		// 2. load shaders
		prog = InitShader("vshader8.glsl", "fshader8.glsl");
		glUseProgram(prog);

		// 3. connect data to shader

	}

};





#endif