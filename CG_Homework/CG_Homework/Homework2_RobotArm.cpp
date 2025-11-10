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
	// --- 1. 상수 정의 ---
	const float L1 = 0.4f;    // 첫 번째 팔 (Upper Arm) 길이
	const float L2 = 0.45f;   // 두 번째 팔 (Lower Arm) 길이
	const float L3 = 0.175f;  // 손의 X축 길이 (Scale(0.35) * 0.5)
	const float PI = 3.1415926535f;

	// (수정) ang3 관절 제한 (0도 ~ 90도)
	const float MIN_ANG3_DEG = 0.0f;
	const float MAX_ANG3_DEG = 90.0f;

	// --- 2. 람다 함수 정의 ---

	// 람다 1: 정운동학(FK) 함수 - "손 끝(Hand Tip)"의 위치를 반환
	auto getEndEffector =
		[&](float ang1_deg, float ang2_deg, float ang3_deg) -> vec3
		{
			float a1_rad = ang1_deg * PI / 180.0f;
			float a2_rad = ang2_deg * PI / 180.0f;
			float a3_rad = ang3_deg * PI / 180.0f;

			// 1. 손목(Wrist)의 위치 (P_wrist)
			// Y축 기준 FK: glRotateZ(a) * Translate(0,L) = (-L*sin(a), L*cos(a))
			float P_wrist_x = -L1 * sin(a1_rad) - L2 * sin(a1_rad + a2_rad);
			float P_wrist_y = L1 * cos(a1_rad) + L2 * cos(a1_rad + a2_rad);

			// 2. 손의 방향 (Hand Direction)
			float a_total_rad = a1_rad + a2_rad + a3_rad;

			// 손의 로컬 X축(1,0)이 월드에서 어느 방향을 가리키는지 계산
			// glRotateZ(a)는 (1,0)을 (cos(a), sin(a))로 매핑
			float Hand_dir_x = cos(a_total_rad);
			float Hand_dir_y = sin(a_total_rad);

			// 3. 최종 "손 끝(Tip)"의 위치
			float P_tip_x = P_wrist_x + L3 * Hand_dir_x;
			float P_tip_y = P_wrist_y + L3 * Hand_dir_y;

			// 4. (롤백) 좌표계 보정 없이 원본 FK 값을 반환
			return vec3(P_tip_x, P_tip_y, 0);
		};

	// 람다 2: 거리 계산 함수
	auto computeDistance =
		[](const vec3& p1, const vec3& p2) -> float
		{
			return sqrt((p1.x - p2.x) * (p1.x - p2.x) +
				(p1.y - p2.y) * (p1.y - p2.y) +
				(p1.z - p2.z) * (p1.z - p2.z));
		};

	// --- 3. 경사 하강법(Hill Climbing) 설정 ---
	vec3 targetPos = target.GetPosition(g_time);
	float delta = 0.1f;

	// --- 4. 경사 하강법 루프 (ang3 클램프 0도~90도 적용) ---
	while (computeDistance(getEndEffector(ang1, ang2, ang3), targetPos) >= 0.01f)
	{
		vec3 currentPos = getEndEffector(ang1, ang2, ang3);
		float minDistance = computeDistance(currentPos, targetPos);
		int minDistanceIndex = -1;

		// 테스트할 6방향의 거리. 현재 거리(minDistance)로 초기화
		float angleChangeDistance[6] = { minDistance, minDistance, minDistance,
										 minDistance, minDistance, minDistance };

		// 1. 6방향 테스트 (ang1, ang2는 항상 테스트)
		angleChangeDistance[3] = computeDistance(getEndEffector(ang1, ang2 - delta, ang3), targetPos);
		angleChangeDistance[2] = computeDistance(getEndEffector(ang1, ang2 + delta, ang3), targetPos);
		angleChangeDistance[1] = computeDistance(getEndEffector(ang1 - delta, ang2, ang3), targetPos);
		angleChangeDistance[0] = computeDistance(getEndEffector(ang1 + delta, ang2, ang3), targetPos);

		// (핵심) ang3는 클램프 범위 내일 경우에만 테스트
		if (ang3 + delta <= MAX_ANG3_DEG)
			angleChangeDistance[4] = computeDistance(getEndEffector(ang1, ang2, ang3 + delta), targetPos);

		if (ang3 - delta >= MIN_ANG3_DEG) // 0도보다 크거나 같을 때만 테스트
			angleChangeDistance[5] = computeDistance(getEndEffector(ang1, ang2, ang3 - delta), targetPos);


		// 2. 가장 가까워지는 방향 찾기
		for (int i = 0; i < 6; i++)
		{
			if (minDistance > angleChangeDistance[i])
			{
				minDistance = angleChangeDistance[i];
				minDistanceIndex = i;
			}
		}

		// 3. 가장 좋은 방향으로 각도 수정
		if (minDistanceIndex == -1)
		{
			break; // 더 이상 가까워질 수 없음
		}

		switch (minDistanceIndex)
		{
		case 0: ang1 += delta; break;
		case 1: ang1 -= delta; break;
		case 2: ang2 += delta; break;
		case 3: ang2 -= delta; break;
		case 4: ang3 += delta; break;
		case 5: ang3 -= delta; break;
		}
	} // end of while
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