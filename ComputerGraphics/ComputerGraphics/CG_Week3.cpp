#include <vgl.h>
#include <math.h>
#include <stdio.h>
#include <InitShader.h>

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
	// 1. Data is ready in CPU. Transfer it to GPU if needed.
	// 1-1. Create a Verttex Array Object
	//unsigned int VAO; //but we don't use it
	GLuint VAO; //it is same as unsigned int but it is more general
	glGenVertexArrays(1, &VAO); //return is pointer that is virtual address and it is for this program
	//nuber key should be positive

	glBindVertexArray(VAO);

	// 1-2. Create a Vertex Buffer Object in the Vertex Array
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //VBO is bound to the target

	// 1-3. Transfer the Data to the Buffer Object
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW); //GL_STATIC_DRAW: data will not be changed

	// 2. Load your Shader Programs
	GLuint prog;
	prog = InitShader("vshader.glsl", "fshader.glsl"); //compile and upload to GPU
	glUseProgram(prog); //set the program to be used

	// 3. Connect your data with the Shaders
	GLuint vPosition = 0;
	glEnableVertexAttribArray(vPosition); //enable the attribute
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); //set the attribute
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_POINTS, 0, numPoints);

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
	myInit();

	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}