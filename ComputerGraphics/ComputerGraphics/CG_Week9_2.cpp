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

	mat4 M(1.0);
	mat4 CTM(1.0);

	float ang1 = 30 * sin(g_Time * 2);
	float ang2 = 30 * sin(g_Time* 4);
	float ang3 = -30 * sin(g_Time*8);

	//Base
	CTM = Translate(0, -0.5, 0);
	M = Scale(0.3, 0.2, 0.2);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	//Uupper Arm
	CTM *= RotateZ(ang1);
	M = Translate(0,0.2,0)*Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	//Lower Arm
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang2);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	//Hand
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang3);
	M =  Translate(0, 0.05, 0)*Scale(0.2, 0.1, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	glutSwapBuffers();
}

void myIdle()
{
	g_Time += 0.01f;
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