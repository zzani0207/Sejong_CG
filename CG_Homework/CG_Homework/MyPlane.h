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
	float waveSpeed;
	float waveHeight;
	float waveFrequency;

	MyPlane()
	{
		numSquare = 0;
		numVertices = 0;
		isInit = false;
		isWave = false;
		waveSpeed = 1.0f;
		waveHeight = 1.0f;
		waveFrequency = 1.0f;
	}

	void init(int numSquare)
	{
		// glutInit, glewInit 이 호출된 이후 호출될 것
		if (numSquare < 3) numSquare = 3;
		this->numSquare = numSquare;
		numVertices = 6 * numSquare * numSquare;

		MyPlaneVertex* data = new MyPlaneVertex[numVertices];

		float edge = 1.5f; // 평면의 한 변의 길이
		int curIdx = 0;
		const vec4 darkGray = vec4(0.2f, 0.2f, 0.2f, 1);
		const vec4 lightGray = vec4(0.8f, 0.8f, 0.8f, 1);
		for (int i = 0; i < numSquare; i++)
		{
			float length = edge / numSquare; // 한 칸의 길이

			// 평면의 중심이 (0,0)이 되도록 좌표 수정
			float y1 = length * i - edge/2.0f;
			float y2 = length * (i + 1) - edge/2.0f;
			for (int j = 0; j < numSquare; j++)
			{
				vec4 color = !((i + j) % 2) ? darkGray : lightGray;

				// 평면의 중심이 (0,0)이 되도록 좌표 수정
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

		GLuint uWaveHeight = glGetUniformLocation(prog, "uWaveHeight");
		glUniform1f(uWaveHeight, waveHeight);

		GLuint uWaveFrequency = glGetUniformLocation(prog, "uWaveFrequency");
		glUniform1f(uWaveFrequency, waveFrequency);

		GLuint uWaveSpeed = glGetUniformLocation(prog, "uWaveSpeed");
		glUniform1f(uWaveSpeed, waveSpeed);

		glDrawArrays(GL_TRIANGLES, 0, numVertices);
	}

	void increaseDiv()
	{
		printf("Div: %d / Num of Triangles: %d / Num of Vertices: %d\n", numSquare, numSquare * numSquare * 2, numVertices);
		init(numSquare + 1);
	}

	void decreaseDiv()
	{
		printf("Div: %d / Num of Triangles: %d / Num of Vertices: %d\n", numSquare, numSquare * numSquare * 2, numVertices);
		init(numSquare - 1);
	}

	void wave()
	{
		isWave = !isWave;
	}

	void waveSpeedUp()
	{
		printf("Wave Speed: %.1f\n", waveSpeed);
		waveSpeed += 0.05f;
		if (waveSpeed > 3.0f) waveSpeed = 3.0f;
		
	}

	void waveSpeedDown()
	{
		printf("Wave Speed: %.1f\n", waveSpeed);
		waveSpeed -= 0.05f;
		if (waveSpeed < 0.0f) waveSpeed = 0.0f;
	}

	void waveHeightUp()
	{
		printf("Wave Height: %.1f\n", waveHeight);
		waveHeight += 0.1f;
		if (waveHeight > 4.5f) waveHeight = 4.5f;
	}

	void waveHeightDown()
	{
		printf("Wave Height: %.1f\n", waveHeight);
		waveHeight -= 0.1f;
		if (waveHeight < 0.0f) waveHeight = 0.0f;
	}

	void waveFrequencyUp()
	{
		printf("Wave Frequency: %.1f\n", waveFrequency);
		waveFrequency += 0.1f;
		if (waveFrequency > 30.0f) waveFrequency = 30.0f;
	}

	void waveFrequencyDown()
	{
		printf("Wave Frequency: %.1f\n", waveFrequency);
		waveFrequency -= 0.1f;
		if (waveFrequency < 1.0f) waveFrequency = 1.0f;
	}
};

#endif
