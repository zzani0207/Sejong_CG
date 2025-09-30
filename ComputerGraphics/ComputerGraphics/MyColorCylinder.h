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
	int numDiv;
	int numVertices;
	//MyCylinderVertex* vertices;

	GLuint vao;
	GLuint vbo;
	GLuint prog;
	bool isInit;

	MyColorCylinder()
	{
		numDiv = 0;
		numVertices = 0;
		isInit = false;
	}
	void init(int div)
	{
		// glutInit, glewInit 이 호출된 이후 호출될 것
		if (div < 3) div = 3;
		numDiv = div;
		numVertices = 6 * numDiv + numDiv * 3 * 2;
		
		MyCylinderVertex* data = new MyCylinderVertex[numVertices];

		float r = 0.5f;
		int curIdx = 0;
		for (int i = 0; i < numDiv; i++)
		{
			float theta1 = 2 * 3.141592f / numDiv * i;
			float theta2 = 2 * 3.141592f / numDiv * (i + 1);

			float x1 = r * cos(theta1), x2 = r * cos(theta2), z1 = r * sin(theta1), z2 = r * sin(theta2);
			float rr = i / (float)numDiv;
			vec4 color = vec4(1 - rr, 0, rr, 1);

			vec4 o1 = vec4(0, 0.5, 0, 1); vec4 co1 = vec4(0, 1, 0, 1);
			vec4 o2 = vec4(0, -0.5, 0, 1); vec4 co2 = vec4(0, 0, 1, 1);

			vec4 a = vec4(x1, 0.5, z1, 1); vec4 ca = color;
			vec4 b = vec4(x1, -0.5, z1, 1); vec4 cb = color;
			vec4 c = vec4(x2, -0.5, z2, 1); vec4 cc = color;
			vec4 d = vec4(x2, 0.5, z2, 1); vec4 cd = color;

			data[curIdx].position = a; data[curIdx].color = ca; curIdx++;
			data[curIdx].position = b; data[curIdx].color = cb; curIdx++;
			data[curIdx].position = c; data[curIdx].color = cc; curIdx++;

			data[curIdx].position = a; data[curIdx].color = ca; curIdx++;
			data[curIdx].position = c; data[curIdx].color = cc; curIdx++;
			data[curIdx].position = d; data[curIdx].color = cd; curIdx++;

			data[curIdx].position = o1; data[curIdx].color = co1; curIdx++;
			data[curIdx].position = a; data[curIdx].color = ca; curIdx++;
			data[curIdx].position = d; data[curIdx].color = cd; curIdx++;

			data[curIdx].position = o2; data[curIdx].color = co2; curIdx++;
			data[curIdx].position = c; data[curIdx].color = cc; curIdx++;
			data[curIdx].position = b; data[curIdx].color = cb; curIdx++;
		}

		// 1. send data to GPU
		if(!isInit) glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		if(!isInit) glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(MyCylinderVertex) * numVertices, data, GL_STATIC_DRAW);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * numVertices, points);
		//glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * numVertices, sizeof(vec4) * numVertices, colors);

		// 2. load shaders
		if(!isInit) prog = InitShader("vshader8.glsl", "fshader8.glsl");
		glUseProgram(prog);

		// 3. connect data to shader
		connectData2Shader();

		delete[] data;
		isInit = true;
	}

	void connectData2Shader()
	{
		GLuint vPosition = glGetAttribLocation(prog, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyCylinderVertex), BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(prog, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyCylinderVertex), BUFFER_OFFSET(sizeof(vec4)));
	}

	void draw(float curTime)
	{
		connectData2Shader();

		GLuint utime = glGetUniformLocation(prog, "uTime");
		glUniform1f(utime, curTime);

		glDrawArrays(GL_TRIANGLES, 0, numVertices);
	}

	void increaseDiv()
	{
		init(numDiv + 1);
	}

	void decreaseDiv()
	{
		init(numDiv - 1);
	}
};

#endif