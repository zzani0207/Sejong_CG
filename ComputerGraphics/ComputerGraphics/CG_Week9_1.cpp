#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include "MyCube.h"
#include "MyPyramid.h"

MyCube cube;
MyPyramid pyramid;

GLuint program;
GLuint uMat;

mat4 g_Mat = mat4(1.0);

float g_Time = 0;

void myInit()
{
	cube.Init();
	pyramid.Init();

	program = InitShader("vshader9.glsl", "fshader9.glsl");
	glUseProgram(program);
}

mat4 CTM;
void drawWindmill(float angle)
{
	GLuint uMat = glGetUniformLocation(program, "uMat");

	mat4 M(1.0);
	//Windmill Base
	M = CTM * Scale(0.5, 1, 0.4);
	glUniformMatrix4fv(uMat, 1, true, M);
	pyramid.Draw(program);

	//Blade
	CTM = CTM * Translate(0, 0.20, 0.25) * RotateZ(g_Time * 30);

	//Blade1
	M = CTM * Scale(0.8, 0.05, 0.05);
	glUniformMatrix4fv(uMat, 1, true, M);
	cube.Draw(program);

	//Blade2
	M = CTM * RotateZ(90) * Scale(0.8, 0.05, 0.05);
	glUniformMatrix4fv(uMat, 1, true, M);
	cube.Draw(program);

	//Joint
	M = CTM * Scale(0.1, 0.1, 0.1);
	glUniformMatrix4fv(uMat, 1, true, M);
	cube.Draw(program);
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	GLuint uMat = glGetUniformLocation(program, "uMat");

	CTM = mat4(1.0); //current transformation matrix

	//for (float x = -0.7; x <= 0.7; x += 0.7)
	//{
	//	for (float y = -0.7; y <= 0.7; y += 0.7)
	//	{
	//		CTM = Translate(x, y, 0) * RotateY(g_Time * 50) * Scale(0.2, 0.2, 0.2);
	//		drawWindmill(g_Time * 30);
	//	}
	//}

	mat4 M;

	//Sun
	CTM = RotateY(g_Time * 5);
	M = CTM * Scale(0.3, 0.3, 0.3);
	glUniformMatrix4fv(uMat, 1, true, M);
	cube.Draw(program);

	//Earth
	CTM *= RotateZ(g_Time * 30) * Translate(0.7, 0, 0);
	M = CTM * RotateY(g_Time * 10) * Scale(0.1, 0.1, 0.1);
	glUniformMatrix4fv(uMat, 1, true, M);
	cube.Draw(program);

	//Moon
	CTM *= RotateZ(g_Time * 40) * Translate(0.2, 0, 0);
	M = CTM * Scale(0.03, 0.03, 0.03);
	glUniformMatrix4fv(uMat, 1, true, M);
	cube.Draw(program);

	glutSwapBuffers();
}

void myIdle()
{
	g_Time += 0.1f;
	Sleep(16);
	glutPostRedisplay();
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Cube and Pyramid");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(myDisplay);
	glutIdleFunc(myIdle);

	glutMainLoop();

	return 0;
}