#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "Utils.h"
using namespace std;

#define numVAOs 6  // ���������������
#define numVBOs 12 // ���㻺���������

// ȫ�ֱ���
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

int numSegments = 100;

// ��װ����Բ�εĺ���
void createCircle(GLuint vaoID, GLuint vboVerticesID, GLuint vboColorsID, float centerX, float centerY, float radius, float r, float g, float b, float alpha, int numSegments, float startAngle = 0.0f, float sweepAngle = 2.0f * 3.1415926f)
{
    // ��̬����ռ�
    float* vertices = new float[numSegments * 2];
    float* colors = new float[numSegments * 4];

    for (int i = 0; i < numSegments; ++i) {
        float angle = startAngle + sweepAngle * float(i) / float(numSegments - 1);
        vertices[2 * i] = radius * cosf(angle) + centerX;
        vertices[2 * i + 1] = radius * sinf(angle) + centerY;

        colors[4 * i] = r;
        colors[4 * i + 1] = g;
        colors[4 * i + 2] = b;
        colors[4 * i + 3] = alpha;
    }

    // �� VAO
    glBindVertexArray(vao[vaoID]);

    // �󶨶��㻺����󲢼�������
    glBindBuffer(GL_ARRAY_BUFFER, vbo[vboVerticesID]);
    glBufferData(GL_ARRAY_BUFFER, numSegments * 2 * sizeof(float), vertices, GL_STATIC_DRAW);
    GLuint vPositionLoc = glGetAttribLocation(renderingProgram, "vPosition");
    glVertexAttribPointer(vPositionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPositionLoc);

    // ����ɫ������󲢼�������
    glBindBuffer(GL_ARRAY_BUFFER, vbo[vboColorsID]);
    glBufferData(GL_ARRAY_BUFFER, numSegments * 4 * sizeof(float), colors, GL_STATIC_DRAW);
    GLuint vColorLoc = glGetAttribLocation(renderingProgram, "vColor");
    glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vColorLoc);

    // �ͷŶ�̬����Ŀռ�
    delete[] vertices;
    delete[] colors;
}

// ��װ�����ı��εĺ���
void createQuad(GLuint vaoID, GLuint vboVerticesID, GLuint vboColorsID, float vertices[8], float colors[16])
{
    // �� VAO
    glBindVertexArray(vao[vaoID]);

    // �󶨶��㻺����󲢼�������
    glBindBuffer(GL_ARRAY_BUFFER, vbo[vboVerticesID]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertices, GL_STATIC_DRAW);
    GLuint vPositionLoc = glGetAttribLocation(renderingProgram, "vPosition");
    glVertexAttribPointer(vPositionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPositionLoc);

    // ����ɫ������󲢼�������
    glBindBuffer(GL_ARRAY_BUFFER, vbo[vboColorsID]);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), colors, GL_STATIC_DRAW);
    GLuint vColorLoc = glGetAttribLocation(renderingProgram, "vColor");
    glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vColorLoc);
}

// ������ɫ������ʼ������
void init(GLFWwindow* window)
{
    renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

    glGenVertexArrays(numVAOs, vao);
    glGenBuffers(numVBOs, vbo);

    /////////// �����岿�֣����౦�����壩 //////////
    float bodyVertices[] = {
        // ���β���
        -0.5f, -0.5f,  // ����
        0.5f, -0.5f,   // ����
        0.5f, 0.5f,    // ����
        -0.5f, 0.5f    // ����
    };
    float bodyColors[] = {
        // ��ɫ
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f
    };

    createQuad(0, 0, 1, bodyVertices, bodyColors);

    /////////// ����ƽ���ı��β��� //////////
    float topParallelogramVertices[] = {
        // ����ƽ���ı���
        -0.5f, 0.5f,   // ��������
        0.5f, 0.5f,    // ��������
        0.8f, 0.8f,    // ����ƫ��
        -0.2f, 0.8f    // ����ƫ��
    };
    float topParallelogramColors[] = {
        // ǳ��ɫ
        1.0f, 1.0f, 0.5f, 1.0f,
        1.0f, 1.0f, 0.5f, 1.0f,
        1.0f, 1.0f, 0.5f, 1.0f,
        1.0f, 1.0f, 0.5f, 1.0f
    };

    createQuad(1, 2, 3, topParallelogramVertices, topParallelogramColors);

    /////////// �Ҳ�ƽ���ı��β��� //////////
    float rightParallelogramVertices[] = {
        // �Ҳ�ƽ���ı���
        0.5f, -0.5f,   // ��������
        0.8f, -0.2f,   // ����ƫ��
        0.8f, 0.8f,    // ����ƫ��
        0.5f, 0.5f     // ��������
    };
    float rightParallelogramColors[] = {
        // ���ɫ
        1.0f, 0.9f, 0.0f, 1.0f,
        1.0f, 0.9f, 0.0f, 1.0f,
        1.0f, 0.9f, 0.0f, 1.0f,
        1.0f, 0.9f, 0.0f, 1.0f
    };

    createQuad(2, 4, 5, rightParallelogramVertices, rightParallelogramColors);

    /////////// �Ҳ��۾����֣���ɫԲ�Σ� //////////
    createCircle(3, 6, 7, 0.2f, 0.2f, 0.1f, 0.0f, 0.0f, 0.0f, 1.0f, numSegments);

    /////////// ����۾����֣���ɫԲ�Σ� //////////
    createCircle(4, 8, 9, -0.2f, 0.2f, 0.1f, 0.0f, 0.0f, 0.0f, 1.0f, numSegments);

    /////////// ��Բ���죨�ۺ�ɫ�� //////////
    createCircle(5, 10, 11, 0.0f, -0.2f, 0.1f, 1.0f, 0.75f, 0.8f, 1.0f, numSegments / 2, 3.1415926f, 3.1415926f);
}

// ���ƺ���
void display(GLFWwindow* window, double currentTime)
{
    // ������ɫ��
    glUseProgram(renderingProgram);

    // �ø���ɫ��գ����ǣ����У���������ɫ��
    glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
    // �����ɫ������
    glClear(GL_COLOR_BUFFER_BIT);

    // ���Ƴ����壨���౦�����壩
    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // ���ƶ���ƽ���ı���
    glBindVertexArray(vao[1]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // �����Ҳ�ƽ���ı���
    glBindVertexArray(vao[2]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // �����Ҳ��۾�
    glBindVertexArray(vao[3]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments);

    // ��������۾�
    glBindVertexArray(vao[4]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments);

    // �������
    glBindVertexArray(vao[5]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments / 2);
}

// �ص����������ڴ����ڴ�С����ʱ���¼�
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(void)
{
    // glfw ��ʼ�����������ںʹ��� OpenGL �����ĵĿ⣩
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    // ���ڰ汾������ OpenGL �汾��
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // �������� �����������⣬��ʹ�ö���ʾ���������� OpenGL �����ģ�
    GLFWwindow* window = glfwCreateWindow(800, 800, "SpongeBob", NULL, NULL);
    // ���ոմ����Ĵ�������Ϊ��ǰ�̵߳� OpenGL �����ģ��Ա������ OpenGL ���û�Ӱ�쵽������ڡ�
    glfwMakeContextCurrent(window);

    // glew ��ʼ������������ OpenGL ��չ������
    if (glewInit() != GLEW_OK)
    {
        exit(EXIT_FAILURE);
    }
    // ����֡�������
    glfwSwapInterval(1);

    // ���ڸı��С�Ļص�����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ������ɫ���������Ͱ� VAO �� VBO
    init(window);

    // �¼�ѭ��
    // ����Ƿ�رմ��ڣ�����ѭ��
    while (!glfwWindowShouldClose(window))
    {
        // ���ڻ��Ƶ�ǰ֡��ͼ��
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
