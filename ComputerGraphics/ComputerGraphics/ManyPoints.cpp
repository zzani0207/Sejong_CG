#include <vgl.h>
#include <math.h>
#include <stdio.h>

const int numPoints = 5000;

struct MyPoint
{
	float x, y;
};


MyPoint points[numPoints];

void myInit()
{
	for (int i = 0; i < numPoints; i++)
	{
		points[i].x = (rand() % 1000) / 500.0f - 1.0f;
		points[i].y = (rand() % 1000) / 500.0f - 1.0f;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);

	for (int i = 0; i < numPoints; i++)
	{
		glVertex2f(points[i].x, points[i].y);
	}

	glEnd();
	glFlush();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Hello GL");

	glewExperimental = true;
	glewInit();

	printf("GL version: %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
	//myInit();

	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}