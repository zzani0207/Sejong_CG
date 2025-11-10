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

mat4 myLookAt(vec3 eye, vec3 at, vec3 up)
{
	vec3 n = at - eye; // cx, cy, cz -> camera axis
	float len = length(n);
	if (len > 0.0000001f)
		n = n / len;
	else n = vec3(0, 0, 1);

	vec3 cz = -n;

	float alpha = dot(n, up);
	vec3 v0 = up - alpha * n;

	vec3 cy = normalize(v0);

	vec3 cx = cross(cy, cz);

	mat4 Rw(1.0);
	Rw[0][0] = cx.x; Rw[0][1] = cx.x; Rw[0][2] = cx.x;
	Rw[1][0] = cy.y; Rw[1][1] = cy.y; Rw[1][2] = cy.y;
	Rw[2][0] = cz.z; Rw[2][1] = cz.z; Rw[2][2] = cz.z;

	mat4 Rc(1.0);
	for(int i = 0  ;i < 4 ; i++)
		for (int j = 0; j < 4; j++)
			Rc[i][j] = Rw[j][i];
	//Rc = transpose(Rw); 버그있음

	mat4 Tc = Translate(-eye.x, -eye.y, -eye.z);
	return Rc * Tc;
}

void myInit()
{
	cube.Init();
	pyramid.Init();

	program = InitShader("vshader9.glsl", "fshader9.glsl");
	glUseProgram(program);
}



void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	GLuint uMat = glGetUniformLocation(program, "uMat");


	mat4 V(1.0); // View matrix
	mat4 P(1.0); // Projection matrix

	vec3 eye = vec3(sin(g_Time* 3.141592/ 2), cos(g_Time*3.141592 /4), cos(g_Time * 3.141592 / 2));
	vec3 at = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);

	V = myLookAt(eye, at, up);


	mat4 M(1.0); // Model matrix
	glUniformMatrix4fv(uMat, 1, true, P * V * M);

	//pyramid.Draw(program);
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