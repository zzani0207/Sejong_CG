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
float g_Aspect = 1;


mat4 myLookAt(vec3 e, vec3 at, vec3 u) {
	// camera axis: cx, cy, cz 
	vec3 n = at - e;

	float len = length(n);
	if (len > 0.000001f)
		n = n / len;
	else
		n = vec3(0, 0, 1);
	vec3 cz = -n;

	float alpha = dot(n, u);
	vec3 v0 = u - alpha * n;
	vec3 cy = normalize(v0);

	// x, y, z:  x x y = z, y x z = x, z x x = y
	vec3 cx = cross(cy, cz);

	mat4 Rw(1.0);
	Rw[0][0] = cx.x;	Rw[0][1] = cy.x;	Rw[0][2] = cz.x;
	Rw[1][0] = cx.y;	Rw[1][1] = cy.y;	Rw[1][2] = cz.y;
	Rw[2][0] = cx.z;	Rw[2][1] = cy.z;	Rw[2][2] = cz.z;

	mat4 Rc(1.0);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Rc[i][j] = Rw[j][i];
	//Rc = transpose(Rw);
	mat4 Tc = Translate(-e.x, -e.y, -e.z);

	return Rc * Tc;
}

mat4 myOrtho(float left, float right, float bottom, float top,
	float depthNear, float depthFar)
{
	float xcen = (left + right) / 2;
	float ycen = (bottom + top) / 2;
	float zcen = -(depthNear + depthFar) / 2;

	float xsize = right - left;
	float ysize = top - bottom;
	float zsize = depthFar - depthNear;

	mat4 T = Translate(-xcen, -ycen, -zcen);
	mat4 S = Scale(2 / xsize, 2 / ysize, 2 / zsize);

	return S * T;
}

mat4 myPerspective(float fovy, float aspect, float zNear, float zFar)
{
	mat4 S; // view frustum -> canonical view volume
			// far = -1 enar = c = zNear/zFar
			// minX = -1, minY = -1, maxX = 1, maxY = 1

	float sZ = 1 / zFar;
	float sY = 1 / (tan(fovy / 2) * zFar);
	float sX = sY / aspect;

	S = Scale(sX, sY, sZ);

	float c = zNear / zFar;

	mat4 H(1.0f); // unhinging process: canaonical view volume -> screen
	H[2][2] = 1 / (1 + c);
	H[2][3] = -c / (1 + c);
	H[3][2] = -1.0f;
	H[3][3] = 0.0f;

	return H * S;
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

	mat4 V(1.0);						// View Transform
	mat4 P(1.0);						// Projection Transform

	vec3 eye = vec3(0, 0, 3);
	//vec3 eye = vec3(sin(g_Time * 3.141592 / 2),
	//	0.5 * cos(g_Time * 3.1415920) + 0.5,
	//	cos(g_Time * 3.141592 / 2));
	vec3 at = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);

	V = myLookAt(eye, at, up);

	float size = 2;
	//P = myOrtho(-size * g_Aspect, size * g_Aspect, -size, size, 0, 10);
	P = myPerspective(20, g_Aspect, 0.1, 10);


	mat4 M(1.0);	
	M = RotateY(90 * g_Time);// Model Transform
	glUniformMatrix4fv(uMat, 1, true, P * V * M);
	//pyramid.Draw(program);
	cube.Draw(program);

	glutSwapBuffers();
}

void myIdle()
{
	g_Time += 0.016f;
	Sleep(16);
	glutPostRedisplay();
}

void myReshape(int wx, int wy)
{
	//printf("wx=%d, wy=%d \n", wx, wy);
	// aspect ratio :  ��Ⱦ�� = wx/wy : 16:9, 4:3, 21:9
	g_Aspect = (float)wx / (float)wy;

	glViewport(0, 0, wx, wy);

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
	glutReshapeFunc(myReshape);

	glutMainLoop();

	return 0;
}