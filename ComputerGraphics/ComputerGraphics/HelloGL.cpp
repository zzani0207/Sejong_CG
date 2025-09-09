#include <vgl.h>
#include<math.h>

float dtheta = 0.0f;
void display()
{
	glClearColor(1, 0, 0, 1); // Red background

	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_TRIANGLES);

	//glColor3f(0.5, 0.5, 0.5); // Gray color

	//glVertex2f(-0.5, -0.5); // 2f means 2 float arguments
	//glVertex2f(0.5, -0.5);
	//glVertex2f(-0.5, 0.5);

	//glVertex2f(0.5, -0.5);
	//glVertex2f(0.5, 0.5);
	//glVertex2f(-0.5, 0.5);

	int div = 100;
	float r = 0.5;

	for (int i = 0; i < div; i++)
	{
		glColor3f(0, 1.0f / div * i, 1 - 1.0f / div * i); // Gradient color

		glVertex2f(0, 0);
		float theta1 = 2 * 3.141592f / div * i + dtheta;
		glVertex2f(r * cos(theta1), r * sin(theta1));
		float theta2 = 2 * 3.141592f / div * (i + 1) + dtheta;
		glVertex2f(r * cos(theta2), r * sin(theta2));
	}
	glEnd();

	glFlush();

	dtheta += 1 / (2 * 3.141592f);
}


int main(int argc, char** argv)
{
	//gluCreateWindow까진 초기세팅 디폴트 양식
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	//해상도 설정
	glutInitWindowSize(512, 512);
	//창의 제목
	glutCreateWindow("Hello GL");

	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}