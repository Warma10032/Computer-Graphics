#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "Utils.h"
using namespace std;

#define numVAOs 2  // ���������������
#define numVBOs 4 // ���㻺���������
#define numVEOs 1 // ����Ԫ�ض�������

//ȫ�ֱ���
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint veo[numVEOs];

//������ɫ������ʼ������
void init(GLFWwindow* window) 
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	glGenVertexArrays(numVAOs, vao);
	glGenBuffers(numVBOs, vbo);

	///////////������//////////////
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
	//�󶨵�ǰVAO
	glBindVertexArray(vao[0]);
	// ��VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// Load the data into the GPU������VBO��ַ�� 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//��VBO������������ɫ������Ӧ�Ķ�������
	GLuint vPositionLoc = glGetAttribLocation(renderingProgram, "vPosition"); // ��ȡ��ɫ����vPosition�����ĵ�ַ
	// �涨�õ�ַ�µ����ݴ洢��ʽ��vertices�ȵĶ����йأ�������ַ��2ά�ȣ������ͣ����淶��ת����������ƫ������
	glVertexAttribPointer(vPositionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(vPositionLoc);

	// Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_STATIC_DRAW);
	//��VBO������������ɫ������Ӧ�Ķ�������
	GLuint vColorLoc = glGetAttribLocation(renderingProgram, "vColor");
	glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColorLoc);


	///////////������//////////////
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

	//�󶨵�ǰVAO
	glBindVertexArray(vao[1]);
	// Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_square), vertices_square, GL_STATIC_DRAW);
	//��VBO������������ɫ������Ӧ�Ķ�������
	glVertexAttribPointer(vPositionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPositionLoc);

	// Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_square), color_square, GL_STATIC_DRAW);
	//��VBO������������ɫ������Ӧ�Ķ�������
	glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColorLoc);
}
//���ƺ���
void display(GLFWwindow* window, double currentTime) 
{
	//������ɫ��
	glUseProgram(renderingProgram); 

	// �ø���ɫ��գ����ǣ����У���������ɫ��
	glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
	// �����ɫ������
	glClear(GL_COLOR_BUFFER_BIT);
	//��������VAO
	glBindVertexArray(vao[0]);
	// ���������Σ�����ͼԪ����ʼ���㣬��������
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//����������
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
//�ص����������ڴ����ڴ�С����ʱ���¼�
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main(void) 
{
	//glfw��ʼ�����������ںʹ��� OpenGL �����ĵĿ⣩
	if (!glfwInit()) 
	{ 
		exit(EXIT_FAILURE); 
	}
	//���ڰ汾������ OpenGL �汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//�������� �����������⣬��ʹ�ö���ʾ���������� OpenGL �����ģ�
	GLFWwindow* window = glfwCreateWindow(800, 800, "triangle and square", NULL, NULL);
	// ���ոմ����Ĵ�������Ϊ��ǰ�̵߳� OpenGL �����ģ��Ա������ OpenGL ���û�Ӱ�쵽������ڡ�
	glfwMakeContextCurrent(window);

	//glew��ʼ������������ OpenGL ��չ������
	if (glewInit() != GLEW_OK)
	{ 
		exit(EXIT_FAILURE);
	}
	// ����֡�������
	glfwSwapInterval(1);

	//���ڸı��С�Ļص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//������ɫ���������Ͱ�VAO��VBO
	init(window);

	//�¼�ѭ��
	// ����Ƿ�رմ��ڣ�����ѭ��
	while (!glfwWindowShouldClose(window)) 
	{
		// ���ڻ��Ƶ�ǰ֡��ͼ��
		display(window, glfwGetTime());
		// ���ڽ���ǰ�󻺳�������ʾ����Ⱦ��ͼ��
		// ͨ��ʹ��˫���弼����������Ⱦ����ǰ��ʾ��֡�洢��ǰ�������У�GPU ��Ⱦ����һ֡ͼ��洢�ں󻺳����С�
		glfwSwapBuffers(window);
		// ���ڴ�������δ������¼�������������롢����ƶ��ȣ�
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}