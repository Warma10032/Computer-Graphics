#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "Utils.h"
using namespace std;

#define numVAOs 6  // 顶点数组对象数量
#define numVBOs 12 // 顶点缓冲对象数量

// 全局变量
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

int numSegments = 100;

// 封装绘制圆形的函数
void createCircle(GLuint vaoID, GLuint vboVerticesID, GLuint vboColorsID, float centerX, float centerY, float radius, float r, float g, float b, float alpha, int numSegments, float startAngle = 0.0f, float sweepAngle = 2.0f * 3.1415926f)
{
    // 动态申请空间
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

    // 绑定 VAO
    glBindVertexArray(vao[vaoID]);

    // 绑定顶点缓冲对象并加载数据
    glBindBuffer(GL_ARRAY_BUFFER, vbo[vboVerticesID]);
    glBufferData(GL_ARRAY_BUFFER, numSegments * 2 * sizeof(float), vertices, GL_STATIC_DRAW);
    GLuint vPositionLoc = glGetAttribLocation(renderingProgram, "vPosition");
    glVertexAttribPointer(vPositionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPositionLoc);

    // 绑定颜色缓冲对象并加载数据
    glBindBuffer(GL_ARRAY_BUFFER, vbo[vboColorsID]);
    glBufferData(GL_ARRAY_BUFFER, numSegments * 4 * sizeof(float), colors, GL_STATIC_DRAW);
    GLuint vColorLoc = glGetAttribLocation(renderingProgram, "vColor");
    glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vColorLoc);

    // 释放动态申请的空间
    delete[] vertices;
    delete[] colors;
}

// 封装绘制四边形的函数
void createQuad(GLuint vaoID, GLuint vboVerticesID, GLuint vboColorsID, float vertices[8], float colors[16])
{
    // 绑定 VAO
    glBindVertexArray(vao[vaoID]);

    // 绑定顶点缓冲对象并加载数据
    glBindBuffer(GL_ARRAY_BUFFER, vbo[vboVerticesID]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertices, GL_STATIC_DRAW);
    GLuint vPositionLoc = glGetAttribLocation(renderingProgram, "vPosition");
    glVertexAttribPointer(vPositionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPositionLoc);

    // 绑定颜色缓冲对象并加载数据
    glBindBuffer(GL_ARRAY_BUFFER, vbo[vboColorsID]);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), colors, GL_STATIC_DRAW);
    GLuint vColorLoc = glGetAttribLocation(renderingProgram, "vColor");
    glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vColorLoc);
}

// 导入着色器，初始化窗口
void init(GLFWwindow* window)
{
    renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

    glGenVertexArrays(numVAOs, vao);
    glGenBuffers(numVBOs, vbo);

    /////////// 长方体部分（海绵宝宝身体） //////////
    float bodyVertices[] = {
        // 矩形部分
        -0.5f, -0.5f,  // 左下
        0.5f, -0.5f,   // 右下
        0.5f, 0.5f,    // 右上
        -0.5f, 0.5f    // 左上
    };
    float bodyColors[] = {
        // 黄色
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f
    };

    createQuad(0, 0, 1, bodyVertices, bodyColors);

    /////////// 顶部平行四边形部分 //////////
    float topParallelogramVertices[] = {
        // 顶部平行四边形
        -0.5f, 0.5f,   // 矩形左上
        0.5f, 0.5f,    // 矩形右上
        0.8f, 0.8f,    // 右上偏移
        -0.2f, 0.8f    // 左上偏移
    };
    float topParallelogramColors[] = {
        // 浅黄色
        1.0f, 1.0f, 0.5f, 1.0f,
        1.0f, 1.0f, 0.5f, 1.0f,
        1.0f, 1.0f, 0.5f, 1.0f,
        1.0f, 1.0f, 0.5f, 1.0f
    };

    createQuad(1, 2, 3, topParallelogramVertices, topParallelogramColors);

    /////////// 右侧平行四边形部分 //////////
    float rightParallelogramVertices[] = {
        // 右侧平行四边形
        0.5f, -0.5f,   // 矩形右下
        0.8f, -0.2f,   // 右下偏移
        0.8f, 0.8f,    // 右上偏移
        0.5f, 0.5f     // 矩形右上
    };
    float rightParallelogramColors[] = {
        // 深黄色
        1.0f, 0.9f, 0.0f, 1.0f,
        1.0f, 0.9f, 0.0f, 1.0f,
        1.0f, 0.9f, 0.0f, 1.0f,
        1.0f, 0.9f, 0.0f, 1.0f
    };

    createQuad(2, 4, 5, rightParallelogramVertices, rightParallelogramColors);

    /////////// 右侧眼睛部分（黑色圆形） //////////
    createCircle(3, 6, 7, 0.2f, 0.2f, 0.1f, 0.0f, 0.0f, 0.0f, 1.0f, numSegments);

    /////////// 左侧眼睛部分（黑色圆形） //////////
    createCircle(4, 8, 9, -0.2f, 0.2f, 0.1f, 0.0f, 0.0f, 0.0f, 1.0f, numSegments);

    /////////// 半圆形嘴（粉红色） //////////
    createCircle(5, 10, 11, 0.0f, -0.2f, 0.1f, 1.0f, 0.75f, 0.8f, 1.0f, numSegments / 2, 3.1415926f, 3.1415926f);
}

// 绘制函数
void display(GLFWwindow* window, double currentTime)
{
    // 启动着色器
    glUseProgram(renderingProgram);

    // 用该颜色清空（覆盖）所有（即背景颜色）
    glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
    // 清除颜色缓冲区
    glClear(GL_COLOR_BUFFER_BIT);

    // 绘制长方体（海绵宝宝身体）
    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // 绘制顶部平行四边形
    glBindVertexArray(vao[1]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // 绘制右侧平行四边形
    glBindVertexArray(vao[2]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // 绘制右侧眼睛
    glBindVertexArray(vao[3]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments);

    // 绘制左侧眼睛
    glBindVertexArray(vao[4]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments);

    // 绘制嘴巴
    glBindVertexArray(vao[5]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments / 2);
}

// 回调函数，用于处理窗口大小调整时的事件
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(void)
{
    // glfw 初始化（创建窗口和处理 OpenGL 上下文的库）
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    // 窗口版本（设置 OpenGL 版本）
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 创建窗口 （长、宽，标题，不使用多显示器，不共享 OpenGL 上下文）
    GLFWwindow* window = glfwCreateWindow(800, 800, "SpongeBob", NULL, NULL);
    // 将刚刚创建的窗口设置为当前线程的 OpenGL 上下文，以便后续的 OpenGL 调用会影响到这个窗口。
    glfwMakeContextCurrent(window);

    // glew 初始化（加载所有 OpenGL 扩展函数）
    if (glewInit() != GLEW_OK)
    {
        exit(EXIT_FAILURE);
    }
    // 设置帧交换间隔
    glfwSwapInterval(1);

    // 窗口改变大小的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 导入着色器，创建和绑定 VAO 和 VBO
    init(window);

    // 事件循环
    // 检测是否关闭窗口，否则循环
    while (!glfwWindowShouldClose(window))
    {
        // 用于绘制当前帧的图形
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
