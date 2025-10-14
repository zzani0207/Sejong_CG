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

	MySphere()
	{
		la = 0;
		lo = 0;
	}

	void init(int _la, int _lo)
	{
		la = _la; // 가로로 쪼개는 개수 (원통의 개수)
		lo = _lo; // 세로로 쪼개는 개수 (원통의 옆면을 쪼개는 개수)

		numVertices = 3 * ((la - 2) * lo * 2 + 2 * lo);
		MySphereVertex* data = new MySphereVertex[numVertices];

		float dTheta = 2 * 3.141592f / lo;
		float r = 0.5f; // 반지름
		float dPi = 3.141592f / la;

		int curIdx = 0;
		for (int i = 0; i < la; i++)
		{
			for (int j = 0; j < lo - 1; j++)
			{
				float theta1 = dTheta * i;// a, c 의 i 방향 각도
				float theta2 = dTheta * (i + 1);// b, d 의 i 방향 각도

				float pi1 = dPi * j; // a, b 의 j 방향 각도
				float pi2 = dPi * (j + 1); // c, d 의 j 방향 각도
				float r1 = r * sin(pi1); //a,b의 반지름 j
				float r2 = r * sin(pi2);//c,d의 반지름 j
				float y1 = r * cos(pi1);//a,b의 높이 j
				float y2 = r * cos(pi2); //c,d의 높이 j

				vec4 a = vec4(r1 * cos(theta1), y1, r1 * sin(theta1), 1);
				vec4 b = vec4(r1 * cos(theta2), y1, r1 * sin(theta2), 1);
				vec4 c = vec4(r2 * cos(theta1), y2, r2 * sin(theta1), 1);
				vec4 d = vec4(r2 * cos(theta2), y2, r2 * sin(theta2), 1);

				vec4 color = vec4(0.9, 0.9, 0.9, 1);
				if ((i + j) % 2 == 0)
					color = vec4(0.5, 0.5, 0.5, 1);

				vec4 ca = color;
				vec4 cb = color;
				vec4 cc = color;
				vec4 cd = color;

				if (j != 0)
				{
					data[curIdx].position = a; data[curIdx].color = ca; curIdx++;
					data[curIdx].position = b; data[curIdx].color = cb; curIdx++;
					data[curIdx].position = d; data[curIdx].color = cd; curIdx++;
				}
				if(j != la - 1)
				{
					data[curIdx].position = a; data[curIdx].color = ca; curIdx++;
					data[curIdx].position = d; data[curIdx].color = cd; curIdx++;
					data[curIdx].position = c; data[curIdx].color = cc; curIdx++;
				}
			}
		}

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(MySphereVertex) * numVertices, data, GL_STATIC_DRAW);
		delete[] data;
	}

	void connect2Shader(GLuint prog)
	{
		GLuint vPosition = glGetAttribLocation(prog, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MySphereVertex), BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(prog, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MySphereVertex), BUFFER_OFFSET(sizeof(vec4) * numVertices));
	}

	void draw(GLuint prog)
	{
		glBindVertexArray(vao);

		glUseProgram(prog);
		connect2Shader(prog);
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
	}
};

#endif