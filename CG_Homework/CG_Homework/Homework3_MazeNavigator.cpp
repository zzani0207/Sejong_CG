#define _CRT_SECURE_NO_WARNINGS

#include <vgl.h>
#include <InitShader.h>
#include "MyCube.h"
#include "MyUtil.h"
#include <stdbool.h>

#include <vec.h>
#include <mat.h>

#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

#define MAZE_FILE	"maze2.txt"

using namespace std;

struct Node
{
	int x, z;
	float g, h;
	Node* parent;

	// 우선순위 큐 구현을 위한 연산자 오버로딩
	bool operator>(const Node& n) const
	{
		return (g + h) > (n.g + n.h);
	}
};

vector<vec3> path;
bool isAutoNavigating = false;
int pathIndex = 0;

MyCube cube;
GLuint program;

mat4 g_Mat = mat4(1.0f);
GLuint uMat;
GLuint uColor;

float wWidth = 1000;
float wHeight = 500;

vec3 cameraPos = vec3(0, 0, 0);
vec3 viewDirection = vec3(0, 0, -1);
vec3 goalPos = vec3(0, 0, 0);

int MazeSize;
char maze[255][255] = { 0 };

float cameraSpeed = 0.1;
float rotationSpeed = 8.0;
float cameraAngle = 0;

bool isKeyPressed = false;

float g_time = 0;

void getIndexFromPosition(const vec3& position, int& i, int& j)
{
	float unit = 1;
	i = round(position.x + MazeSize / 2.0 - unit / 2);
	j = round(position.z + MazeSize / 2.0 - unit / 2);
}

// 인덱스(i, j)로부터 월드 좌표 얻기
inline vec3 getPositionFromIndex(int i, int j)
{
	float unit = 1;
	vec3 leftTopPosition = vec3(-MazeSize / 2.0 + unit / 2, 0, -MazeSize / 2.0 + unit / 2);
	vec3 xDir = vec3(1, 0, 0);
	vec3 zDir = vec3(0, 0, 1);
	return leftTopPosition + i * xDir + j * zDir;
}

// 미로 파일 로드
void LoadMaze()
{
	FILE* file = fopen(MAZE_FILE, "r");
	if (file == NULL)
	{
		printf("Cannot open maze file!\n");
		return;
	}
	char buf[255];
	fgets(buf, 255, file);
	sscanf(buf, "%d", &MazeSize);
	for (int j = 0; j < MazeSize; j++)
	{
		fgets(buf, 255, file);
		for (int i = 0; i < MazeSize; i++)
		{
			maze[i][j] = buf[i];
			if (maze[i][j] == 'C')				// Setup Camera Position
				cameraPos = getPositionFromIndex(i, j);
			if (maze[i][j] == 'G')				// Setup Goal Position
				goalPos = getPositionFromIndex(i, j);
		}
	}
	fclose(file);
}

void FindShortestPath()
{
	int startI, startJ, goalI, goalJ;
	getIndexFromPosition(cameraPos, startI, startJ);
	getIndexFromPosition(goalPos, goalI, goalJ);

	if (startI == goalI && startJ == goalJ)
		return; // 현재 위치가 목표 위치인 경우

	path.clear();
	// A* 알고리즘을 위한 우선순위 큐, 구조체에서 만든 연산자 오버로딩 사용
	priority_queue<Node, vector<Node>, greater<Node>> openList;
	bool closedList[255][255] = { false };
	Node* nodeMap[255][255] = { nullptr }; // 노드 맵 초기화

	// 시작 노드 초기화 및 큐에 추가
	Node* startNode = new Node{ startI, startJ, 0, 0, nullptr };
	startNode->h = abs(goalI - startI) + abs(goalJ - startJ); // 맨해튼 거리로 휴리스틱 계산
	openList.push(*startNode);
	nodeMap[startI][startJ] = startNode;

	int dx[] = { 0, 0, -1, 1 };
	int dz[] = { -1, 1, 0, 0 };

	Node* goalNode = nullptr;

	while (!openList.empty())
	{
		Node currentNode = openList.top();
		openList.pop();

		int cx = currentNode.x;
		int cz = currentNode.z;

		if (closedList[cx][cz])
			continue;
		closedList[cx][cz] = true;

		// 목표 노드에 도달한 경우
		if (cx == goalI && cz == goalJ)
		{
			goalNode = nodeMap[cx][cz];
			break;
		}

		// 인접 노드 탐색
		for (int i = 0; i < 4; i++)
		{
			int nx = cx + dx[i];
			int nz = cz + dz[i];

			// 유효한 위치인지 확인
			if (nx < 0 || nx >= MazeSize || nz < 0 || nz >= MazeSize)
				continue;
			if (maze[nx][nz] == '*') // 벽인 경우
				continue;
			if (closedList[nx][nz]) // 이미 닫힌 리스트에 있는 경우
				continue;

			float newG = currentNode.g + 1; // 이동 비용 계산

			// 새로운 노드이거나 더 짧은 경로인 경우
			if (nodeMap[nx][nz] == nullptr || newG < nodeMap[nx][nz]->g)
			{
				Node* neighborNode = new Node{ nx, nz, newG, 0, nodeMap[cx][cz] };
				neighborNode->h = abs(goalI - nx) + abs(goalJ - nz); // 휴리스틱 계산
				openList.push(*neighborNode);
				nodeMap[nx][nz] = neighborNode;
			}
		}
	}

	// 경로 재구성
	if (goalNode != nullptr)
	{
		Node* currentNode = goalNode;
		while (currentNode != nullptr)
		{
			path.push_back(getPositionFromIndex(currentNode->x, currentNode->z));
			currentNode = currentNode->parent;
		}

		reverse(path.begin(), path.end()); // 경로를 시작에서 목표로 순서로 뒤집기
	}
}

void DrawPath()
{
	if (path.empty()) return;

	for (int i = 0; i < path.size() - 1; i++)
	{
		float wave = sin(i * 0.5f - g_time * 0.2f);
		wave = (wave + 1) / 2.0f; // 0 ~ 1 사이 값으로 변환

		vec3 currentPos = path[i];
		vec3 nextPos = path[i + 1];

		vec3 center = (currentPos + nextPos) / 2.0f;

		vec3 diff = nextPos - currentPos;

		vec3 color = vec3(wave, 0, 0);
		mat4 ModelMat;

		if(abs(diff.x) > 0.1f)
			ModelMat = Translate(center) * Translate(0, -0.5, 0) * Scale(vec3(1.0f, 0.1f, 0.1f));
		else
			ModelMat = Translate(center) * Translate(0, -0.5, 0) * Scale(vec3(0.1f, 0.1f, 1.0f));
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
		glUniform4f(uColor, color.x, color.y, color.z, 1);
		cube.Draw(program);
	}
}

bool checkIsWall(vec3 position)
{
	int i, j;

	getIndexFromPosition(position, i, j);

	float threshold = 0.75f;


	for (int di = -1; di <= 1; di++)
	{
		for (int dj = -1; dj <= 1; dj++)
		{
			int ni = i + di;
			int nj = j + dj;

			if (ni < 0 || ni >= MazeSize || nj < 0 || nj >= MazeSize)
				continue;

			if (maze[ni][nj] == '*')
			{
				vec3 wallPos = getPositionFromIndex(ni, nj);

				float distGapX = abs(position.x - wallPos.x);
				float distGapZ = abs(position.z - wallPos.z);
				if (distGapX < threshold && distGapZ < threshold)
				{
					//isCollided = true;
					//collisionPos = wallPos;
					return true;
				}
			}
		}
	}

	return false;
}
// 미로 벽 생성
void DrawMaze()
{
	float Threshold = 0.85f;

	for (int j = 0; j < MazeSize; j++)
		for (int i = 0; i < MazeSize; i++)
			if (maze[i][j] == '*')
			{
				vec3 color;
				vec3 wallPos = getPositionFromIndex(i, j);

				float distGpaX = abs(cameraPos.x - wallPos.x);
				float distGpaZ = abs(cameraPos.z - wallPos.z);

				if (distGpaX < Threshold && distGpaZ < Threshold && isKeyPressed)
					color = vec3(1, 0, 0);
				else
					color = vec3(i / (float)MazeSize, j / (float)MazeSize, 1);

				mat4 ModelMat = Translate(getPositionFromIndex(i, j));
				glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
				glUniform4f(uColor, color.x, color.y, color.z, 1);
				cube.Draw(program);
			}
}

void myInit()
{
	LoadMaze();
	cube.Init();
	program = InitShader("vshader.glsl", "fshader.glsl");

}

void DrawGrid()
{
	float n = 40;
	float w = MazeSize;
	float h = MazeSize;

	// 가로 격자
	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(0, -0.5, -h / 2 + h / n * i) * Scale(w, 0.02, 0.02);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}

	// 세로 격자
	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(-w / 2 + w / n * i, -0.5, 0) * Scale(0.02, 0.02, h);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
}

void drawCamera()
{
	float cameraSize = 0.5;

	mat4 ModelMat = Translate(cameraPos) * RotateY(cameraAngle) * Scale(vec3(cameraSize));
	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	ModelMat = Translate(cameraPos + viewDirection * cameraSize / 2) * RotateY(cameraAngle) * Scale(vec3(cameraSize / 2));
	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);
}

void drawGoal()
{
	glUseProgram(program);
	float GoalSize = 0.7;

	mat4 ModelMat = Translate(goalPos) * RotateY(g_time * 3) * Scale(vec3(GoalSize));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);

	ModelMat = Translate(goalPos) * RotateY(g_time * 3 + 45) * Scale(vec3(GoalSize));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);
}

void drawScene(bool bDrawCamera = true)
{
	glUseProgram(program);
	uMat = glGetUniformLocation(program, "uMat");
	uColor = glGetUniformLocation(program, "uColor");

	DrawGrid();
	DrawPath();
	DrawMaze();
	drawGoal();

	if (bDrawCamera)
		drawCamera();
}

void display()
{
	glEnable(GL_DEPTH_TEST);

	float vWidth = wWidth / 2;
	float vHeight = wHeight;

	// LEFT SCREEN : View From Camera (Perspective Projection)
	glViewport(0, 0, vWidth, vHeight);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	float h = 4;
	float aspectRatio = vWidth / vHeight;
	float w = aspectRatio * h;
	mat4 ViewMat = myLookAt(cameraPos, cameraPos + viewDirection, vec3(0, 1, 0));
	mat4 ProjMat = myPerspective(45, aspectRatio, 0.01, 20);

	g_Mat = ProjMat * ViewMat;
	drawScene(false);							// drawing scene except the camera


	// RIGHT SCREEN : View from above (Orthographic parallel projection)
	glViewport(vWidth, 0, vWidth, vHeight);
	h = MazeSize;
	w = aspectRatio * h;
	ViewMat = myLookAt(vec3(0, 5, 0), vec3(0, 0, 0), vec3(0, 0, -1));
	ProjMat = myOrtho(-w / 2, w / 2, -h / 2, h / 2, 0, 20);

	g_Mat = ProjMat * ViewMat;
	drawScene(true);


	glutSwapBuffers();
}

void cameraRotate(float angle)
{
	cameraAngle += angle;
	viewDirection.x = -sin(cameraAngle * 3.141592 / 180.0f);
	viewDirection.z = -cos(cameraAngle * 3.141592 / 180.0f);
}

void idle()
{
	g_time += 1;

	vec3 nextPos = vec3(0, 0, 0);
	isKeyPressed = false;

	if ((GetAsyncKeyState('Q') & 0x8000) == 0x8000)		// if "Q" key is pressed	: Find Shortest Path to the Goal
	{
		FindShortestPath();
		isAutoNavigating = false;
		//pathIndex = 0;
		Sleep(200);
	}
	if ((GetAsyncKeyState(VK_SPACE) & 0x8000) == 0x8000)		// if "Spacebar" key is pressed	: Stop Moving
	{
		isAutoNavigating = !isAutoNavigating;
		if (isAutoNavigating && !path.empty())
		{
			cameraPos = path[0];
			pathIndex = 1;

			vec3 startDir = normalize(path[1] - path[0]);
			viewDirection = startDir;

			cameraAngle = atan2(-viewDirection.x, -viewDirection.z) * 180.0f / 3.141592f;
		}

		Sleep(200);
	}

	if (isAutoNavigating && !path.empty())
	{
		if (pathIndex >= path.size())
		{
			isAutoNavigating = false; // 도착
			return;
		}

		vec3 targetPos = path[pathIndex];
		vec3 dir = targetPos - cameraPos;
		float distance = length(dir);

		if (distance < 0.1f)
		{
			cameraPos = targetPos;
			pathIndex++;
			return;
		}

		vec3 targetDir = normalize(dir);
		vec3 currentDir = normalize(viewDirection);

		float dotProduct = dot(currentDir, targetDir);
		float alignmentThreshold = 0.995f;
		if(dotProduct < alignmentThreshold)
		{
			vec3 crossProduct = cross(currentDir, targetDir);
			
			if(crossProduct.y > 0)
				cameraRotate(rotationSpeed);
			else
				cameraRotate(-rotationSpeed);
		}
		else
		{
			cameraPos += cameraSpeed * targetDir;
		}
	}
	else
	{
		if ((GetAsyncKeyState('A') & 0x8000) == 0x8000)		// if "A" key is pressed	: Turn Left
		{
			cameraRotate(rotationSpeed);
		}
		if ((GetAsyncKeyState('D') & 0x8000) == 0x8000)		// if "D" key is pressed	: Turn Right
		{
			cameraRotate(-rotationSpeed);
		}
		if ((GetAsyncKeyState('W') & 0x8000) == 0x8000)		// if "W" key is pressed	: Go Forward
		{
			isKeyPressed = true;
			nextPos += cameraSpeed * normalize(viewDirection);
		}
		if ((GetAsyncKeyState('S') & 0x8000) == 0x8000)		// if "S" key is pressed	: Go Backward
		{
			isKeyPressed = true;
			nextPos -= cameraSpeed * normalize(viewDirection);
		}

		if (isKeyPressed && (length(nextPos) > 0))
		{
			vec3 nextPosX = cameraPos;
			nextPosX.x += nextPos.x;

			if (!checkIsWall(nextPosX))
				cameraPos.x = nextPosX.x;

			vec3 nextPosZ = cameraPos;
			nextPosZ.z += nextPos.z;

			if (!checkIsWall(nextPosZ))
				cameraPos.z = nextPosZ.z;

		}
	}

	//if(!isKeyPressed&& isMoving)
	//{
	//	isMoving = false;
	//	isCollided = false;

	//	int i, j;
	//	getIndexFromPosition(cameraPos, i, j);
	//	if(maze[i][j] != '*')
	//		cameraPos = getPositionFromIndex(i, j);
	//}

	//if(!checkIsWall(nextPos))
	//	cameraPos = nextPos;

	Sleep(16);											// for vSync
	glutPostRedisplay();
}

void reshape(int wx, int wy)
{
	//printf("%d %d \n", wx, wy);
	wWidth = wx;
	wHeight = wy;
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(wWidth, wHeight);

	glutCreateWindow("Homework3 (Maze Navigator)");

	glewExperimental = true;
	glewInit();

	//printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		//glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("A Maze Navigator\n");
	printf("Programming Assignment #3 for Computer Graphics.Department of Software, Sejong University\n");
	printf("\n----------------------------------------------------------------\n");
	printf("`W' key: Go Forward\n");
	printf("`S' key : Go Backward\n");
	printf("'A' key : Turn Left\n");
	printf("'D' key : Turn Right\n");
	printf("'Q' key : Find Shortest Path to the Goal\n");
	printf("'Spacebar' : Start following the Path\n");
	printf("\n----------------------------------------------------------------\n");

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}