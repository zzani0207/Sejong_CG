#include <vec.h>
#include <stdio.h>
#include <InitShader.h>
#include <vgl.h>
#include "MyPlane.h"
#include <Windows.h>

MyPlane plane;

float g_time = 0.0f;
bool g_bPlay = false;


void myInit()
{
	plane.init(20);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	plane.draw(g_time);
	glFlush();

	glutSwapBuffers();
}

void myIdle()
{
	Sleep(16);
	if (g_bPlay)
	{
		g_time += 0.016f;
		glutPostRedisplay();
	}
}

void myKeyboard(unsigned char key, int x, int y)
{
	//printf("key = %c, x=%d, y=%d\n", key, x, y);
	switch (key)
	{
	case ' ':	g_bPlay = !g_bPlay;			break;
	case '1': plane.decreaseDiv();			break;
	case '2': plane.increaseDiv();			break;
	case 'w': plane.wave(); 				break;
	case 'a': plane.waveSpeedUp();			break;
	case 's': plane.waveSpeedDown();		break;
	case 'd': plane.waveHeightUp();			break;
	case 'f': plane.waveHeightDown();		break;
	case 'g': plane.waveFrequencyUp();		break;
	case 'h': plane.waveFrequencyDown();	break;
	case 'q': exit(0);						break;
	default:								break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Waving Plain");

	glewExperimental = true;
	glewInit();

	printf("A Waving Plane\n");
	printf("Programming Assignment #1 for computer Graphics\n");
	printf("21010907 AhnChanhwi\n");
	printf("\n");
	printf("------------------------------------------------\n");
	printf("'1' : Decrease the number of division\n");
	printf("'2' : Dncrease the number of division\n");
	printf("'A' : Increase wave speed\n");
	printf("'S' : Decrease wave speed\n");
	printf("'D' : Increase wave height\n");
	printf("'F' : Decrease wave height\n");
	printf("'G' : Increase wave frequency\n");
	printf("'H' : Decrease wave frequency\n\n");
	printf("'W' : Start wave/Pause\n");
	printf("'Space Bar' : Play/Pause\n");
	printf("'Q' : Quit\n");
	printf("------------------------------------------------\n");

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(myIdle);
	glutKeyboardFunc(myKeyboard);


	glutMainLoop();
	return 0;
}