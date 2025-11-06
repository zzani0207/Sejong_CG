#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include "MyCube.h"
#include "MyPyramid.h"
#include "MyTarget.h"

MyCube cube;
MyPyramid pyramid;
MyTarget target(&cube);

GLuint program;
GLuint uMat;

mat4 CTM;

bool bPlay = false;
bool bChasingTarget = false;
bool bDrawTarget = false;

float ang1 = 0;
float ang2 = 0;
float ang3 = 0;

void myInit()
{
	cube.Init();
	pyramid.Init();

	program = InitShader("vshader_arm.glsl", "fshader_arm.glsl");
	glUseProgram(program);
}

float g_time = 0;

void drawRobotArm(float ang1, float ang2, float ang3)
{
	mat4 temp = CTM;

	// BASE
	mat4 M(1.0);

	M = Translate(0,0,0.075) * Scale(0.3, 0.2, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	M = Translate(0, 0, -0.075) * Scale(0.3, 0.2, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	// First Joint
	M =  RotateX(90) * Scale(0.05, 0.22, 0.05) ;
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Upper Arm
	CTM *= RotateZ(ang1);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.5, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Second Joint
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang2);          
	M =  RotateX(90) * Scale(0.05, 0.22, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Lower Arm1
	M = Translate(0, 0.225, 0.07) * Scale(0.1, 0.55, 0.04);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Lower Arm2
	M = Translate(0, 0.225, -0.07) * Scale(0.1, 0.55, 0.04);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Third Joint
	CTM *= Translate(0, 0.4, 0);
	M = Translate(0, 0.05, 0) * RotateX(90) * Scale(0.05, 0.22, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Hand
	CTM *=  RotateZ(ang3);
	M = Translate(0, 0.1, 0) * Scale(0.3, 0.2, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	CTM = temp;
}

void computeAngle()
{
	printf("Current Target Pos: %.2f %.2f %.2f\n", target.currentPos.x, target.currentPos.y, target.currentPos.z);
}


void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	GLuint uColor = glGetUniformLocation(program, "uColor");
	glUniform4f(uColor, -1, -1, -1, -1);


	uMat = glGetUniformLocation(program, "uMat");
	CTM = Translate(0, -0.4, 0) * RotateY(g_time * 30);
	drawRobotArm(ang1, ang2, ang3);


	glUniform4f(uColor, 1, 0, 0, 1);
	if (bDrawTarget == true)
		target.Draw(program, CTM, g_time);

	glutSwapBuffers();
}

void myIdle()
{
	if (bPlay)
	{
		g_time += 1 / 60.0f;
		Sleep(1 / 60.0f * 1000);

		if (bChasingTarget == false)
		{
			ang1 = 45 * sin(g_time * 3.141592);
			ang2 = 60 * sin(g_time * 2 * 3.141592);
			ang3 = 30 * sin(g_time * 3.141592);
		}
		else
			computeAngle();

		glutPostRedisplay();
	}
}

void myKeyboard(unsigned char c, int x, int y)
{

	switch (c)
	{
	case '1':
		bChasingTarget = !bChasingTarget;
		break;
	case '2':
		bDrawTarget = !bDrawTarget;
		break;
	case '3':
		target.toggleRandom();
		break;
	case ' ':
		bPlay = !bPlay;
		break;
	default:
		break;
	}
}


int main(int argc, char** argv)
{
	printf("SIMPLE INTELLIGENT ROBOT ARM\n");
	printf("Programming Assignment #2 for Computer Graphics\n");
	printf("Department of Software, Sejong University\n\n");
	printf("--------------------------------------------------------------\n");
	printf("'1' key: Toggle Between Chasing the target or not\n");
	printf("'2' key : Toggle Between Showing the target position or not\n");
	printf("'3' key : Toggle Between Randomly Positioning the target or not\n\n");
	printf("Spacebar : start / stop Playing\n");
	printf("--------------------------------------------------------------\n");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Simple Robot Arm");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(myIdle);

	glutMainLoop();

	return 0;
}