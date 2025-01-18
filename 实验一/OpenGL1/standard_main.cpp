#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "Utils.h"
using namespace std;

#define numVAOs 2  // 顶点数组对象数量
#define numVBOs 4 // 顶点缓冲对象数量
#define numVEOs 1 // 顶点元素对象数量

//全局变量
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint veo[numVEOs];

//导入着色器，初始化窗口
void init(GLFWwindow* window) 
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	glGenVertexArrays(numVAOs, vao);
	glGenBuffers(numVBOs, vbo);

	///////////三角形//////////////
	float vertices[6] = {
		-1.0f, -1.0f,
		0.0f, -1.0f,
		-0.5f, 1.0f
	};
	float vertexColors[12] = {
		1.0f, 0.0f, 0.0f, 1.0f,  // red
		0.0f, 1.0f, 0.0f, 1.0f,  // green
		0.0f, 0.0f, 1.0f, 1.0f   // blue
	};
	//绑定当前VAO
	glBindVertexArray(vao[0]);
	// 绑定VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// Load the data into the GPU（加载VBO地址） 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//将VBO关联给顶点着色器中相应的顶点属性
	GLuint vPositionLoc = glGetAttribLocation(renderingProgram, "vPosition"); // 获取着色器中vPosition变量的地址
	// 规定该地址下的数据存储形式，vertices等的定义有关（变量地址，2维度，浮点型，不规范化转换，步长，偏移量）
	glVertexAttribPointer(vPositionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(vPositionLoc);

	// Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_STATIC_DRAW);
	//将VBO关联给顶点着色器中相应的顶点属性
	GLuint vColorLoc = glGetAttribLocation(renderingProgram, "vColor");
	glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColorLoc);


	///////////正方形//////////////
	float vertices_square[12] = {
		0.0f, -0.5f,
		1.0f, -0.5f,
		0.0f, 0.5f,
		0.0f, 0.5f,
		1.0f, -0.5f,
		1.0f, 0.5f
	};
	float color_square[24] = {
		1.0f, 0.0f, 0.0f, 1.0f,  // red
		0.0f, 0.0f, 1.0f, 1.0f,   // blue
		0.0f, 0.0f, 1.0f, 1.0f,   // blue
		0.0f, 0.0f, 1.0f, 1.0f,   // blue
		0.0f, 0.0f, 1.0f, 1.0f,   // blue
		0.0f, 1.0f, 0.0f, 1.0f   // green
	};

	//绑定当前VAO
	glBindVertexArray(vao[1]);
	// Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_square), vertices_square, GL_STATIC_DRAW);
	//将VBO关联给顶点着色器中相应的顶点属性
	glVertexAttribPointer(vPositionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPositionLoc);

	// Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_square), color_square, GL_STATIC_DRAW);
	//将VBO关联给顶点着色器中相应的顶点属性
	glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColorLoc);
}
//绘制函数
void display(GLFWwindow* window, double currentTime) 
{
	//启动着色器
	glUseProgram(renderingProgram); 

	// 用该颜色清空（覆盖）所有（即背景颜色）
	glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
	// 清除颜色缓冲区
	glClear(GL_COLOR_BUFFER_BIT);
	//绑定三角形VAO
	glBindVertexArray(vao[0]);
	// 绘制三角形（基本图元，起始顶点，顶点数）
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//绘制正方形
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
//回调函数，用于处理窗口大小调整时的事件
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main(void) 
{
	//glfw初始化（创建窗口和处理 OpenGL 上下文的库）
	if (!glfwInit()) 
	{ 
		exit(EXIT_FAILURE); 
	}
	//窗口版本（设置 OpenGL 版本）
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//创建窗口 （长、宽，标题，不使用多显示器，不共享 OpenGL 上下文）
	GLFWwindow* window = glfwCreateWindow(800, 800, "triangle and square", NULL, NULL);
	// 将刚刚创建的窗口设置为当前线程的 OpenGL 上下文，以便后续的 OpenGL 调用会影响到这个窗口。
	glfwMakeContextCurrent(window);

	//glew初始化（加载所有 OpenGL 扩展函数）
	if (glewInit() != GLEW_OK)
	{ 
		exit(EXIT_FAILURE);
	}
	// 设置帧交换间隔
	glfwSwapInterval(1);

	//窗口改变大小的回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//导入着色器，创建和绑定VAO和VBO
	init(window);

	//事件循环
	// 检测是否关闭窗口，否则循环
	while (!glfwWindowShouldClose(window)) 
	{
		// 用于绘制当前帧的图形
		display(window, glfwGetTime());
		// 用于交换前后缓冲区，显示新渲染的图像。
		// 通常使用双缓冲技术来进行渲染，当前显示的帧存储在前缓冲区中，GPU 渲染的下一帧图像存储在后缓冲区中。
		glfwSwapBuffers(window);
		// 用于处理所有未处理的事件（例如键盘输入、鼠标移动等）
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}