#include <vgl.h>
#include <stdio.h>
#include <InitShader.h>
#include <vec.h>
#include "MyColorCube.h"
#include "MyColorCylinder.h"

MyColorCube cube;
MyColorCylinder cylinder;

float g_time = 0.0f;
bool g_bPlay = false;


void myInit() {
	cube.initCube();
	cylinder.init(8);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//cube.draw(g_time);
	cylinder.draw(g_time);
	glFlush();

	glutSwapBuffers();
}

void myIdle() {
	Sleep(16);
	if (g_bPlay)
	{
		g_time += 0.016f;
		glutPostRedisplay();
	}
}

void myKeyboard(unsigned char key, int x, int y)
{
	printf("key = %c, x=%d, y=%d\n", key, x, y);
	switch (key) {
	case ' ':	g_bPlay = !g_bPlay;			break;
	case '1': cylinder.decreaseDiv();		break;
	case '2': cylinder.increaseDiv();		break;
	default:								break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Hello GL");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(myIdle);
	glutKeyboardFunc(myKeyboard);


	glutMainLoop();
	return 0;
}