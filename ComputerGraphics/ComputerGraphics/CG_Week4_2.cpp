#include <vgl.h>
#include <stdio.h>
#include <InitShader.h>
#include <vec.h>
#include "MyColorCube.h"

MyColorCube cube;

float g_time = 0.0f;
bool g_bPlay = false;

void myInit() 
{
	cube.initCube();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//clear bit and color and draw color black
	glEnable(GL_DEPTH_TEST);

	cube.draw(g_time);
	
	glFlush();

	glutSwapBuffers();//change the buffer from back to front
}

void myIdle() {
	Sleep(16);
	if (g_bPlay)
	{
		g_time += 0.016f;
		glutPostRedisplay();
	}
}

void myKeyboard(unsigned char key, int x, int y) //x, y is the mouse position
{
	printf("key =%c, x=%d, y=%d\n", key, x, y);
	switch (key)
	{
	case' ':
		g_bPlay = !g_bPlay;
		break;
	default:
		break;
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
	glutDisplayFunc(display); // register display function
	glutIdleFunc(myIdle); // register idle function
	glutKeyboardFunc(myKeyboard);

	glutMainLoop();
	return 0;
}