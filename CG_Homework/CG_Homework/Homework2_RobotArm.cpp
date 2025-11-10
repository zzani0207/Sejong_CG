#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include "MyCube.h"
#include "MyPyramid.h"
#include "MyTarget.h"
#include <math.h>

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

	M = Translate(0, 0, 0.075) * Scale(0.3, 0.2, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	M = Translate(0, 0, -0.075) * Scale(0.3, 0.2, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	// First Joint
	M = RotateX(90) * Scale(0.05, 0.22, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Upper Arm
	CTM *= RotateZ(ang1);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.5, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Second Joint
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang2);
	M = RotateX(90) * Scale(0.05, 0.22, 0.05);
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
	CTM *= Translate(0, 0.45, 0) * RotateZ(ang3);
	M = RotateX(90) * Scale(0.05, 0.22, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Hands
	M = Scale(0.35, 0.15, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	CTM = temp;
}

void computeAngle()
{
	// --- 1. 타겟 좌표 계산 ---
	// 이 과제의 특이한 좌표계 설정으로 인해,
	// 로봇팔이 쫓아야 할 2D 로컬 좌표는 
	// target.GetPosition()이 반환하는 값의 x, y와 동일합니다.
	vec3 targetWorldPos = target.GetPosition(g_time);
	vec2 targetPos(targetWorldPos.x, targetWorldPos.y);

	// --- 2. IK 계산 (2-Link Arm) ---
	const float L1 = 0.4f;    // 첫 번째 팔 (Upper Arm) 길이
	const float L2 = 0.45f;   // 두 번째 팔 (Lower Arm) 길이
	const float PI = 3.1415926535f;

	float tx = targetPos.x;
	float ty = targetPos.y;

	float dist_sq = tx * tx + ty * ty;

	// --- 3. 각도 계산 (코사인 법칙) ---
	// 타겟은 항상 팔이 닿는 거리에 있으므로, acos 입력값은 항상 -1과 1 사이입니다.
	float cos_ang2 = (dist_sq - L1 * L1 - L2 * L2) / (2 * L1 * L2);

	// (안정성을 위해 아주 작은 부동소수점 오류만 방지)
	if (cos_ang2 > 1.0f)  cos_ang2 = 1.0f;
	if (cos_ang2 < -1.0f) cos_ang2 = -1.0f;

	float ang2_rad = acos(cos_ang2); // "elbow up" 해 (0 ~ PI)

	// atan2를 사용한 안정적인 ang1 계산
	float k1 = L1 + L2 * cos(ang2_rad);
	float k2 = L2 * sin(ang2_rad);
	float ang1_rad = atan2(ty, tx) - atan2(k2, k1);

	// --- 4. 최종 각도 변환 (Radians to Degrees) ---
	ang1 = ang1_rad * 180.0f / PI;
	ang2 = ang2_rad * 180.0f / PI;

	// --- 5. ang3 (손목 각도) 계산 ---
	// 손(Hand)이 로봇팔의 기준 평면(바닥)과 평행을 유지하도록 설정
	ang3 = -(ang1 + ang2);
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