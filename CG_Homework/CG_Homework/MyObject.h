#pragma once
#pragma warning(disable : 4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <vgl.h>
#include <vec.h>

struct MyObjectVertex {
    vec4 position;
    vec4 color;
    vec3 normal;
};

class MyObject 
{
public:
    MyObject() : bInitialized(false), NumVertices(0), Vertices(NULL),
        verticeArray(NULL), normalArrayFlat(NULL), normalArrayPhong(NULL), verticesIdx(NULL),
        vao(0), bufferID(0) {
        maxXYZ = vec3(-10000.0f, -10000.0f, -10000.0f);
        minXYZ = vec3(10000.0f, 10000.0f, 10000.0f);
    }

    ~MyObject() {
        CleanUp();
    }

    void CleanUp() {
        if (Vertices) delete[] Vertices;
        if (verticesIdx) delete[] verticesIdx;
        if (verticeArray) delete[] verticeArray;
        if (normalArrayFlat) delete[] normalArrayFlat;
        if (normalArrayPhong) delete[] normalArrayPhong;

        Vertices = NULL; verticesIdx = NULL; verticeArray = NULL;
        normalArrayFlat = NULL; normalArrayPhong = NULL;
        bInitialized = false;
    }

    // 파일 읽기
    int Init(const char* objName) {
        CleanUp();

        FILE* fp = fopen(objName, "r");
        if (fp == NULL) {
            printf("File not found!\n");
            return -1;
        }

        int vCount = 0;
        int fCount = 0;
        char buffer[1024];

        // 1. 정점 및 면 개수 파악
        while (fgets(buffer, sizeof(buffer), fp) != NULL) 
        {
            if (strlen(buffer) < 2) continue;
            if (buffer[0] == 'v' && buffer[1] == ' ') vCount++;
            else if (buffer[0] == 'f' && buffer[1] == ' ' ) fCount++;
        }

        // 예외 처리
        if (vCount == 0 || fCount == 0) 
        {
            fclose(fp);
            return -1;
        }

        NumVertices = fCount * 3;

        // 초기화
        verticeArray = new vec3[vCount];
        normalArrayPhong = new vec3[vCount];
        for (int i = 0; i < vCount; i++) 
        {
            normalArrayPhong[i] = vec3(0, 0, 0);
            verticeArray[i] = vec3(0, 0, 0);
        }

        normalArrayFlat = new vec3[NumVertices];
        Vertices = new MyObjectVertex[NumVertices];
        verticesIdx = new int[NumVertices];

        maxXYZ = vec3(-10000.0f, -10000.0f, -10000.0f);
        minXYZ = vec3(10000.0f, 10000.0f, 10000.0f);

        // 2. 데이터 파싱
        rewind(fp);
        int vertexIndex = 0;
        int faceIndex = 0;
        vec4 defaultColor = vec4(1.0, 1.0, 1.0, 1.0);

        while (fgets(buffer, sizeof(buffer), fp) != NULL) 
        {
            if (strlen(buffer) < 2) continue;

            if (buffer[0] == 'v' && buffer[1] == ' ' )
            {
                if (vertexIndex >= vCount) continue;
                float x, y, z;
                if (sscanf(buffer + 2, "%f %f %f", &x, &y, &z) == 3) 
                {
                    verticeArray[vertexIndex] = vec3(x, y, z);

                    if (x > maxXYZ.x) maxXYZ.x = x; if (x < minXYZ.x) minXYZ.x = x;
                    if (y > maxXYZ.y) maxXYZ.y = y; if (y < minXYZ.y) minXYZ.y = y;
                    if (z > maxXYZ.z) maxXYZ.z = z; if (z < minXYZ.z) minXYZ.z = z;

                    vertexIndex++;
                }
            }
            else if (buffer[0] == 'f' && buffer[1] == ' ') 
            {
                char* token = strtok(buffer + 2, " \t\n");
                int vIdx[3] = { 0 };
                int matches = 0;
                while (token != NULL && matches < 3) 
                {
                    vIdx[matches++] = atoi(token);
                    token = strtok(NULL, " \t\n");
                }
                if (matches < 3) continue;

                int idx0 = vIdx[0] - 1; int idx1 = vIdx[1] - 1; int idx2 = vIdx[2] - 1;
                if (idx0 < 0 || idx0 >= vCount || idx1 < 0 || idx1 >= vCount || idx2 < 0 || idx2 >= vCount) continue;
                if (faceIndex + 2 >= NumVertices) break;

                vec3 v1 = verticeArray[idx0];
                vec3 v2 = verticeArray[idx1];
                vec3 v3 = verticeArray[idx2];
                vec3 faceNormal = normalize(cross(v2 - v1, v3 - v1));

                normalArrayPhong[idx0] += faceNormal;
                normalArrayPhong[idx1] += faceNormal;
                normalArrayPhong[idx2] += faceNormal;

                Vertices[faceIndex].position = v1; Vertices[faceIndex].color = defaultColor; normalArrayFlat[faceIndex] = faceNormal; verticesIdx[faceIndex++] = idx0;
                Vertices[faceIndex].position = v2; Vertices[faceIndex].color = defaultColor; normalArrayFlat[faceIndex] = faceNormal; verticesIdx[faceIndex++] = idx1;
                Vertices[faceIndex].position = v3; Vertices[faceIndex].color = defaultColor; normalArrayFlat[faceIndex] = faceNormal; verticesIdx[faceIndex++] = idx2;
            }
        }
        fclose(fp);

        NumVertices = faceIndex;
        sizeXYZ = maxXYZ - minXYZ;
        center = (maxXYZ + minXYZ) * 0.5f;

        return 0;
    }

    // GPU 버퍼 생성
    void InitBuffer() 
    {
        if (NumVertices == 0) return;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &bufferID);
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(MyObjectVertex) * NumVertices, Vertices, GL_STATIC_DRAW);

        bInitialized = true;
        PhongShading();
    }

    void FlatShading()
    {
        if (!bInitialized) return;

        for (int i = 0; i < NumVertices; ++i) 
            Vertices[i].normal = normalArrayFlat[i];
        UpdateBuffer();
    }

    void PhongShading()
    {
        if (!bInitialized) return;

        for (int i = 0; i < NumVertices; ++i) 
            Vertices[i].normal = normalize(normalArrayPhong[verticesIdx[i]]);
        UpdateBuffer();
    }

    void UpdateBuffer()
    {
        if (!bInitialized) return;

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(MyObjectVertex) * NumVertices, Vertices);
    }

    void Draw(GLuint program)
    {
        if (!bInitialized) return;

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);

        GLuint posAttrib = glGetAttribLocation(program, "vPosition");
        if (posAttrib != -1) 
        {
            glEnableVertexAttribArray(posAttrib);
            glVertexAttribPointer(posAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(MyObjectVertex), BUFFER_OFFSET(0));
        }

        GLuint colorAttrib = glGetAttribLocation(program, "vColor");
        if (colorAttrib != -1) 
        {
            glEnableVertexAttribArray(colorAttrib);
            glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(MyObjectVertex), BUFFER_OFFSET(sizeof(vec4)));
        }

        GLuint normalAttrib = glGetAttribLocation(program, "vNormal");
        if (normalAttrib != -1)
        {
            glEnableVertexAttribArray(normalAttrib);
            glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_TRUE, sizeof(MyObjectVertex), BUFFER_OFFSET(sizeof(vec4) * 2));
        }

        glDrawArrays(GL_TRIANGLES, 0, NumVertices);
    }

    GLuint vao, bufferID;
    bool bInitialized;
    int NumVertices;

    vec3* verticeArray;
    vec3* normalArrayFlat;
    vec3* normalArrayPhong;
    MyObjectVertex* Vertices;
    int* verticesIdx;

    vec3 maxXYZ, minXYZ, sizeXYZ, center;
};