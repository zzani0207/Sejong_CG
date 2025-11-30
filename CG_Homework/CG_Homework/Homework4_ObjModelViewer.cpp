#pragma warning (disable:4996)

#include <vgl.h>
#include <InitShader.h>
#include <vec.h>
#include <mat.h>
#include <stdio.h>
#include <stdlib.h>

#include "MyObject.h"
#include "MyCube.h"

// 전역 변수 및 객체
MyObject object;
MyCube cube;

GLuint program;     // 축 그리기용 쉐이더
GLuint prog_phong;  // 물체 그리기용 쉐이더

GLuint uMat;
GLuint uColor;
mat4 g_Mat = mat4(1.0f);

int winWidth = 500;
int winHeight = 500;

// 회전 및 속도 제어
int rotateAxis = 1; // 0:Y, 1:X, 2:Z
float speed = 0;    // 0:Stop, 1:Rotate

// 조명 및 재질 파라미터
float Shine = 50.0f;
vec4 KSpc = vec4(0.4, 0.4, 0.4, 1.0);
vec4 KAmb = vec4(0.1, 0.1, 0.1, 1.0);
vec4 KDif = vec4(0.8, 0.8, 0.8, 1.0);
vec4 LPos = vec4(0.0, 0.0, 5.0, 1.0);
vec4 LCol = vec4(1.0, 1.0, 1.0, 1.0);

mat4 ModelMat;
float theta[3] = { 0, 0, 0 };
char objName[256];

// --- 수학 유틸리티 함수 ---
mat4 myLookAt(vec3 eye, vec3 at, vec3 up) {
    vec3 n = normalize(at - eye); n = at - eye; n /= length(n);
    float a = dot(up, n); vec3 v = up - a * n; v /= length(v);
    vec3 w = cross(n, v);
    mat4 Rw(1.0f);
    Rw[0][0] = w.x; Rw[0][1] = v.x; Rw[0][2] = -n.x;
    Rw[1][0] = w.y; Rw[1][1] = v.y; Rw[1][2] = -n.y;
    Rw[2][0] = w.z; Rw[2][1] = v.z; Rw[2][2] = -n.z;
    mat4 Rc(1.0f);
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) Rc[i][j] = Rw[j][i];
    mat4 Tc = Translate(-eye.x, -eye.y, -eye.z);
    return Rc * Tc;
}

mat4 myPerspective(float fovy, float aspect, float zNear, float zFar) {
    mat4 P(1.0f);
    float rad = fovy * 3.141592 / 180.0;
    float sz = 1 / zFar;
    float h = zFar * tan(rad / 2);
    float sy = 1 / h;
    float w = h * aspect;
    float sx = 1 / w;
    mat4 S = Scale(sx, sy, sz);
    mat4 M(1.0f);
    float c = -zNear / zFar;
    M[2][2] = 1 / (c + 1); M[2][3] = -c / (c + 1);
    M[3][2] = -1; M[3][3] = 0;
    return M * S;
}

void myInit() {
    cube.Init();
    object.InitBuffer();

    program = InitShader("vshader.glsl", "fshader.glsl");
    prog_phong = InitShader("vphong.glsl", "fphong.glsl");

    // 물체 크기 정규화 (화면에 꽉 차게)
    float maxDim = object.sizeXYZ.x;
    if (object.sizeXYZ.y > maxDim) maxDim = object.sizeXYZ.y;
    if (object.sizeXYZ.z > maxDim) maxDim = object.sizeXYZ.z;

    float scaling = (maxDim > 0) ? (2.0f / maxDim) : 1.0f;
    ModelMat = Scale(scaling, scaling, scaling) * Translate(-object.center);
}

void DrawAxis() {
    glUseProgram(program);
    uMat = glGetUniformLocation(program, "uMat");
    uColor = glGetUniformLocation(program, "uColor");

    // X축 (Red)
    mat4 x_a = Translate(1.5, 0.0, 0.0) * Scale(3.0, 0.02, 0.02);
    glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * x_a);
    glUniform4f(uColor, 1, 0, 0, 1);
    cube.Draw(program);

    // Y축 (Green)
    mat4 y_a = Translate(0.0, 1.5, 0.0) * Scale(0.02, 3.0, 0.02);
    glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * y_a);
    glUniform4f(uColor, 0, 1, 0, 1);
    cube.Draw(program);

    // Z축 (Blue)
    mat4 z_a = Translate(0.0, 0.0, 1.5) * Scale(0.02, 0.02, 3.0);
    glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * z_a);
    glUniform4f(uColor, 0, 0, 1, 1);
    cube.Draw(program);
}

void display() {
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = winWidth / static_cast<float>(winHeight);
    
    // 카메라 설정 (0,0,5)
    mat4 ViewMat = myLookAt(vec3(0, 0, 5), vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 ProjMat = myPerspective(45, aspect, 0.1, 100);

    // 회전 행렬 계산
    mat4 RotMat = RotateX(theta[1]) * RotateY(theta[0]) * RotateZ(theta[2]);

    // 1. 축 그리기 (물체와 같이 회전하도록 RotMat 적용)
    g_Mat = ProjMat * ViewMat * RotMat;
    DrawAxis();

    // 2. 물체 그리기
    glUseProgram(prog_phong);

    GLuint uProjMat = glGetUniformLocation(prog_phong, "uProjMat");
    GLuint uModelMat = glGetUniformLocation(prog_phong, "uModelMat");

    glUniformMatrix4fv(uProjMat, 1, GL_TRUE, ProjMat);
    glUniformMatrix4fv(uModelMat, 1, GL_TRUE, ViewMat * RotMat * ModelMat);

    // 조명 Uniform 전달
    glUniform4fv(glGetUniformLocation(prog_phong, "uLPos"), 1, LPos);
    glUniform4fv(glGetUniformLocation(prog_phong, "uLCol"), 1, LCol);
    glUniform4fv(glGetUniformLocation(prog_phong, "uKAmb"), 1, KAmb);
    glUniform4fv(glGetUniformLocation(prog_phong, "uKDif"), 1, KDif);
    glUniform4fv(glGetUniformLocation(prog_phong, "uKSpc"), 1, KSpc);
    glUniform1f(glGetUniformLocation(prog_phong, "uShine"), Shine);

    object.Draw(prog_phong);

    glutSwapBuffers();
}

void idle() {
    if (speed > 0) {
        if (rotateAxis == 0) theta[0] += 1.0f;
        if (rotateAxis == 1) theta[1] += 1.0f;
        if (rotateAxis == 2) theta[2] += 1.0f;
    }
    Sleep(16);
    glutPostRedisplay();
}

void reshape(int w, int h) {
    winWidth = w; winHeight = h;
    glViewport(0, 0, w, h);
    glutPostRedisplay();
}

void myKeyboard(unsigned char c, int x, int y) {
    if (c == ' ')
    {
        if(speed==0)
        {
            speed = 1;
            printf("Play!\n");
        }
        else 
        {
            speed = 0;
            printf("Stop!\n");
		}
    }   
    if (c == '1') 
    { 
        object.PhongShading(); 
        printf("Using Vertex Normal!\n"); 
    }
    if (c == '2') 
    { 
        object.FlatShading();
        printf("Using Surface Normal!\n"); 
    }   
    if (c == '3') 
    { 
        if (KSpc.x < 1.0) KSpc += vec4(0.05, 0.05, 0.05, 0.0); 
        printf("Increasing Specular Effect!\n"); 
    }
    if (c == '4') 
    { 
        if (KSpc.x > 0.0) KSpc -= vec4(0.05, 0.05, 0.05, 0.0); 
        printf("Decreasing Specular Effect!\n"); 
    }
    
    if (c == '5') 
    { 
        if (Shine < 100) Shine += 10; 
        printf("Increasing Shininess!\n"); 
    }
    if (c == '6')
    { 
        if (Shine > 10) Shine -= 10;
        printf("Decreasing Shininess!\n"); 
    }
    
    if (c == 'Q' || c == 'q') 
    { 
        printf("Exit the program\n"); 
        exit(0); 
    }
    
    glutPostRedisplay();
}

void processMouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) rotateAxis = 1;
        if (button == GLUT_MIDDLE_BUTTON) rotateAxis = 0;
        if (button == GLUT_RIGHT_BUTTON) rotateAxis = 2;
    }
}

int main(int argc, char** argv) {
    printf("SIMPLE OBJECT MODEL VIEWER\n");
	printf("Programming Assignment #4 for Computer Graphics.  Department of Software, Sejong University\n\n----------------------------------------------------------------\n");
    printf("Spacebar: starting / stoping rotation\n\n");
    printf("Left Mouse Button: rotating around x-axis\nMiddle Mouse Button: rotating around y-axis\nRight Mouse Button: rotating around z-axis\n\n");
    printf("'1' key: Using Vertex Normal for shading\n'2' key: Using Surface Normal for shading\n");
    printf("'3' key: Increasing Specular effect (ks)\n'4' key: Decreasing Specular effect (ks)\n");
    printf("'5' key: Increasing Shininess (n)\n'6' key: Decreasing Shininess (n)\n\n");
    printf("'Q' Key: Exit the program.\n----------------------------------------------------------------\n\n");

    do {
        printf("Input File Path: ");
        scanf("%s", objName);
    } while (object.Init(objName) != 0);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Simple Obj Viewer");

    glewExperimental = true;
    glewInit();

    myInit();

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    glutMouseFunc(processMouse);
    glutKeyboardFunc(myKeyboard);

    glutMainLoop();
    return 0;
}