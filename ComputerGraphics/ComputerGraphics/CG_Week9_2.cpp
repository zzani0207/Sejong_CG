#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include "MyCube.h"
#include "MyPyramid.h"

#include <list>

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
void drawRobotArm1(float ang1, float ang2, float ang3)
{
	GLuint uMat = glGetUniformLocation(program, "uMat");

	mat4 M(1.0);
	mat4 CTM(1.0);

	//Base
	CTM = Translate(0, -0.5, 0);
	M = Scale(0.3, 0.2, 0.2);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	//Uupper Arm
	CTM *= RotateZ(ang1);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	//Lower Arm
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang2);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	//Hand
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang3);
	M = Translate(0, 0.05, 0) * Scale(0.2, 0.1, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);
}
void drawRobotArm2(float ang1, float ang2, float ang3)
{
	GLuint uMat = glGetUniformLocation(program, "uMat");

	mat4 M(1.0);
	mat4 CTM(1.0);

	std::list<mat4> MStack;
	//Base
	CTM = Translate(0, -0.5, 0);
	M = Scale(0.3, 0.2, 0.2);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	MStack.push_back(CTM);

	//Uupper Arm
	CTM *= RotateZ(ang1);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	MStack.push_back(CTM);
	//Lower Arm
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang2);
	M = Translate(0, 0.05, 0) * Scale(0.4, 0.1, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	MStack.push_back(CTM);

	//Hand1
	CTM *= Translate(0.2, 0.1, 0) * RotateZ(ang3);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	MStack.push_back(CTM);

	MStack.pop_back();
	CTM = MStack.back();
	MStack.pop_back();

	//Hand2
	CTM *= Translate(-0.2, 0.1, 0) * RotateZ(ang3);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	MStack.push_back(CTM);
	//Hand1 - middle
	CTM *= Translate(0.4, 0.4, 0);
	M = Translate(0, 0.05, 0) * Scale(0.4, 0.1, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	MStack.push_back(CTM);

	//Hand1 - right
	CTM *= Translate(0.2, 0.1, 0);
	M = Translate(0, 0.05, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	MStack.push_back(CTM);
	MStack.pop_back();
	CTM = MStack.back();
	MStack.pop_back();
	//Hand1 - left
	CTM *= Translate(-0.2, 0.1, 0);
	M = Translate(0, 0.05, 0) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);
}
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

	drawRobotArm2(30 * sin(g_Time), 45 * sin(g_Time ), 40 * sin(g_Time));

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