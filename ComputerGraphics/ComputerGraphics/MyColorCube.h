#ifndef __MY_COLOR_CUBE_H__
#define __MY_COLOR_CUBE_H__

#include <vgl.h>
#include <vec.h>
#include <InitShader.h>

vec4 cubeVertex[] = {
	vec4(-0.5,-0.5, 0.5,1),
	vec4(-0.5, 0.5, 0.5,1),
	vec4(0.5, 0.5, 0.5,1),
	vec4(0.5,-0.5, 0.5,1),

	vec4(-0.5,-0.5,-0.5,1),
	vec4(-0.5, 0.5,-0.5,1),
	vec4(0.5, 0.5,-0.5,1),
	vec4(0.5,-0.5,-0.5,1) };

vec4 cubeColor[] = {
	vec4(1,0,0,1),
	vec4(0,1,0,1),
	vec4(0,0,1,1),
	vec4(1,1,0,1),

	vec4(0,1,1,1),
	vec4(1,0,1,1),
	vec4(1,1,1,1),
	vec4(0,0,0,1) };

class MyColorCube
{
public:

	int numVertices = 6 * 2 * 3;

	vec4* points;
	vec4* colors;

	GLuint vao;
	GLuint vbo;
	GLuint prog;

	int curIdx = 0;

	void initCube() {
		points = new vec4[numVertices];
		colors = new vec4[numVertices];

		curIdx = 0;
		rect(0, 3, 2, 1);
		rect(0, 4, 7, 3);
		rect(3, 7, 6, 2);
		rect(2, 6, 5, 1);
		rect(1, 5, 4, 0);
		rect(4, 7, 6, 5);

		// 1. send data to GPU
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * numVertices + sizeof(vec4) * numVertices, nullptr, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * numVertices, points);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * numVertices, sizeof(vec4) * numVertices, colors);

		// 2. load shaders
		prog = InitShader("vshader8.glsl", "fshader8.glsl");
		glUseProgram(prog);

		// 3. connect data to shader
		connectShader2Buffer();

	}

	void rect(int a, int b, int c, int d) {
		points[curIdx] = cubeVertex[a]; colors[curIdx] = cubeColor[a];  curIdx++;
		points[curIdx] = cubeVertex[b]; colors[curIdx] = cubeColor[b];  curIdx++;
		points[curIdx] = cubeVertex[c]; colors[curIdx] = cubeColor[c];  curIdx++;

		points[curIdx] = cubeVertex[c]; colors[curIdx] = cubeColor[c];  curIdx++;
		points[curIdx] = cubeVertex[d]; colors[curIdx] = cubeColor[d];  curIdx++;
		points[curIdx] = cubeVertex[a]; colors[curIdx] = cubeColor[a];  curIdx++;
	}

	void draw(float curTime)
	{
		glBindVertexArray(vao);
		glUseProgram(prog);
		connectShader2Buffer();

		GLuint uTime = glGetUniformLocation(prog, "uTime");
		glUniform1f(uTime, curTime);

		glDrawArrays(GL_TRIANGLES, 0, numVertices);
	}

	void connectShader2Buffer()
	{
		GLuint vPosition = glGetAttribLocation(prog, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(prog, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4) * numVertices));
	}
};
#endif