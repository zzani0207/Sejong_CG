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

void drawRobotArm1(float ang1, float ang2, float ang3) {
	GLuint uMat = glGetUniformLocation(program, "uMat");

	mat4 M(1.0);					// model matrix
	mat4 CTM(1.0);					// current transform matrix

	//pyramid.Draw(program);
	//cube.Draw(program);

	// Base
	CTM = Translate(0, -0.4, 0);
	M = Scale(0.3, 0.2, 0.2);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	// Upper Arm
	CTM *= RotateZ(ang1);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Lower Arm
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang2);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Hand
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang3);
	M = Translate(0, 0.05, 0) * Scale(0.3, 0.1, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

}

#include <list>

void drawRobotArm2(float ang1, float ang2, float ang3) {
	GLuint uMat = glGetUniformLocation(program, "uMat");
	mat4 M(1.0);					// model matrix
	mat4 CTM(1.0);					// current transform matrix

	std::list<mat4> MStack;

	//pyramid.Draw(program);
	//cube.Draw(program);

	// Base
	CTM = Translate(0, -0.4, 0);
	M = Scale(0.3, 0.2, 0.2);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);
	MStack.push_back(CTM);

	// Upper Arm
	CTM *= RotateZ(ang1);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);
	MStack.push_back(CTM);

	// Lower Arm
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang2);
	M = Translate(0, 0.05, 0) * Scale(0.4, 0.1, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);
	MStack.push_back(CTM);

	// Hand 1
	CTM *= Translate(0.2, 0.1, 0) * RotateZ(ang3);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);
	MStack.push_back(CTM);

	// Hand 1 - middle
	CTM *= Translate(0.0, 0.4, 0);
	M = Translate(0, 0.05, 0) * Scale(0.4, 0.1, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);
	MStack.push_back(CTM);

	// Hand 1 - RIGHT
	CTM *= Translate(0.2, 0.1, 0);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);
	MStack.push_back(CTM);

	MStack.pop_back();

	CTM = MStack.back();
	// Hand 1 - LEFT
	CTM *= Translate(-0.2, 0.1, 0);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);
	MStack.push_back(CTM);

	MStack.pop_back();

	MStack.pop_back();

	MStack.pop_back();
	CTM = MStack.back();

	// Hand 2
	CTM *= Translate(-0.2, 0.1, 0) * RotateZ(-ang3);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	float ang1 = 30 * sin(g_Time * 2);
	float ang2 = 30 * sin(g_Time * 4);
	float ang3 = -30 * sin(g_Time * 6);

	drawRobotArm2(ang1, ang2, ang3);

	glutSwapBuffers();
}

void myIdle()
{
	g_Time += 0.016f;
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