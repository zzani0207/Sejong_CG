#ifndef __MY_PLANE_H__
#define __MY_PLANE_H__

#include <vgl.h>
#include <vec.h>
#include <InitShader.h>
#include <math.h>

struct MyPlaneVertex
{
	vec4 position;
	vec4 color;
};

class MyPlane
{
public:
	int numSquare;
	int numVertices;
	//MyCylinderVertex* vertices;

	GLuint vao;
	GLuint vbo;
	GLuint prog;
	bool isInit;
	bool isWave;

	MyPlane()
	{
		numSquare = 0;
		numVertices = 0;
		isInit = false;
		isWave = false;
	}
	void init(int numSquare)
	{
		// glutInit, glewInit 이 호출된 이후 호출될 것
		if (numSquare < 3) numSquare = 3;
		this->numSquare = numSquare;
		numVertices = 6 * numSquare * numSquare;

		MyPlaneVertex* data = new MyPlaneVertex[numVertices];

		float edge = 1.5f; // 셰이더에서 거리 계산을 편하게 하기 위해 크기를 1.0으로 변경
		int curIdx = 0;
		const vec4 darkGray = vec4(0.2f, 0.2f, 0.2f, 1);
		const vec4 lightGray = vec4(0.8f, 0.8f, 0.8f, 1);
		for (int i = 0; i < numSquare; i++)
		{
			float length = edge / numSquare;
			// 평면의 중심이 (0,0)이 되도록 좌표 수정
			float y1 = length * i - edge/2.0f;
			float y2 = length * (i + 1) - edge/2.0f;
			for (int j = 0; j < numSquare; j++)
			{
				vec4 color = !((i + j) % 2) ? darkGray : lightGray;
				float x1 = length * j - edge/2.0f;
				float x2 = length * (j + 1) - edge/2.0f;

				vec4 a = vec4(x1, y1, 0, 1); vec4 ca = color;
				vec4 b = vec4(x1, y2, 0, 1); vec4 cb = color;
				vec4 c = vec4(x2, y2, 0, 1); vec4 cc = color;
				vec4 d = vec4(x2, y1, 0, 1); vec4 cd = color;

				data[curIdx].position = a; data[curIdx].color = ca; curIdx++;
				data[curIdx].position = b; data[curIdx].color = cb; curIdx++;
				data[curIdx].position = c; data[curIdx].color = cc; curIdx++;

				data[curIdx].position = a; data[curIdx].color = ca; curIdx++;
				data[curIdx].position = c; data[curIdx].color = cc; curIdx++;
				data[curIdx].position = d; data[curIdx].color = cd; curIdx++;
			}
		}

		// 1. send data to GPU
		if (!isInit) glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		if (!isInit) glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(MyPlaneVertex) * numVertices, data, GL_STATIC_DRAW);
		
		// 2. load shaders
		if (!isInit) prog = InitShader("vshader_plane.glsl", "fshader_plane.glsl");
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
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyPlaneVertex), BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(prog, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyPlaneVertex), BUFFER_OFFSET(sizeof(vec4)));
	}

	void draw(float curTime)
	{
		glBindVertexArray(vao);
		glUseProgram(prog);
		connectData2Shader();

		GLuint utime = glGetUniformLocation(prog, "uTime");
		glUniform1f(utime, curTime);

		// isWave 상태를 uniform 변수(uIsWave)로 셰이더에 전달
		GLuint uIsWave = glGetUniformLocation(prog, "uIsWave");
		glUniform1i(uIsWave, isWave); // bool을 int로 전달 (true=1, false=0)

		glDrawArrays(GL_TRIANGLES, 0, numVertices);
	}

	void increaseDiv()
	{
		init(numSquare + 1);
	}

	void decreaseDiv()
	{
		init(numSquare - 1);
	}

	// 이 함수는 isWave 플래그를 토글하는 역할만 합니다.
	void Wave()
	{
		isWave = !isWave;
	}
};

#endif
