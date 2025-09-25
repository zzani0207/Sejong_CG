#include <vgl.h>
#include <stdio.h>
#include <InitShader.h>
#include <vec.h>
//It's important to know the number of vertices
const int numVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

vec4 points[numVertices];
vec4 colors[numVertices];

GLuint vao;
GLuint vbo;
GLuint prog;

float g_Time = 0.0f;

vec4 cubeVertex[8] =
{
	//the core of the cube is (0,0,0) that place is center of the cube
	vec4(-0.5, -0.5,  0.5, 1.0),
	vec4(-0.5,  0.5,  0.5, 1.0),
	vec4(0.5,  0.5,  0.5, 1.0),
	vec4(0.5, -0.5,  0.5, 1.0),
	vec4(-0.5, -0.5, -0.5, 1.0),
	vec4(-0.5,  0.5, -0.5, 1.0),
	vec4(0.5,  0.5, -0.5, 1.0),
	vec4(0.5, -0.5, -0.5, 1.0)
};

vec4 cubeColor[8] =
{
	vec4(0.0, 0.0, 0.0, 1.0), // black
	vec4(1.0, 0.0, 0.0, 1.0), // red
	vec4(1.0, 1.0, 0.0, 1.0), // yellow
	vec4(0.0, 1.0, 0.0, 1.0), // green
	vec4(0.0, 0.0, 1.0, 1.0), // blue
	vec4(1.0, 0.0, 1.0, 1.0), // magenta
	vec4(1.0, 1.0, 1.0, 1.0), // white
	vec4(0.0, 1.0, 1.0, 1.0)  // cyan
};

int curIdx = 0;
void rect(int a, int b, int c, int d)
{
	colors[curIdx] = cubeColor[a]; points[curIdx] = cubeVertex[a]; curIdx++;
	colors[curIdx] = cubeColor[b]; points[curIdx] = cubeVertex[b]; curIdx++;
	colors[curIdx] = cubeColor[c]; points[curIdx] = cubeVertex[c]; curIdx++;

	colors[curIdx] = cubeColor[a]; points[curIdx] = cubeVertex[a]; curIdx++;
	colors[curIdx] = cubeColor[c]; points[curIdx] = cubeVertex[c]; curIdx++;
	colors[curIdx] = cubeColor[d]; points[curIdx] = cubeVertex[d]; curIdx++;
}

void initCube()
{
	curIdx = 0;
	rect(0, 3, 2, 1);
	rect(0, 4, 7, 3);
	rect(3, 7, 6, 2);
	rect(2, 6, 5, 1);
	rect(1, 5, 4, 0);
	rect(4, 7, 6, 5);
}

//we'll always write scrippt on here
void myInit()
{
	initCube();

	// 1. send data to GPU

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), nullptr, GL_STATIC_DRAW); //reserve space
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);//upload data
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);//upload data

	// 2, load shadeers
	prog = InitShader("vshader7.glsl", "fshader7.glsl");
	glUseProgram(prog);

	// 3. connect data with shaders
	GLuint vPosition = glGetAttribLocation(prog, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(prog, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
}

float angle1 = 0.0f;
float angle2 = 0.0f;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//before drawing, we need to update the uniform variables
	GLuint uAnlgle1 = glGetUniformLocation(prog, "uAngle1");
	GLuint uAnlge2 = glGetUniformLocation(prog, "uAngle2");
	glUniform1f(uAnlgle1, angle1);
	glUniform1f(uAnlge2, angle2);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);//It's started from 0 that means first vertex

	glFlush();

	angle1 += 1.0f;
	angle2 += 0.5f;
}

void myIdle()
{
	Sleep(600/72);//sleep for 1/30 second
	glutPostRedisplay();//draw the screen again
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Hello GL");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));


	myInit();

	glutDisplayFunc(display);
	glutIdleFunc(myIdle);//when the system is idle, it will call myIdle function (called per frame)
	glutMainLoop(); //if change is happen, it will call display function

	return 0;
}