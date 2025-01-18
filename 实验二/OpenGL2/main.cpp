#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2\soil2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "ImportedModel.h"
#include "Utils.h"
using namespace std;

#define numVAOs 3
#define numVBOs 9

float cameraX, cameraY, cameraZ;
float objLocX, objLocY, objLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint texture[numVAOs];

// variable allocation for display
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;
int shapeCount = 1;

ImportedModel myModel_1("Nugget.obj");
ImportedModel myModel_2("cylinder.obj");
ImportedModel myModel_3("ball.obj");

float toRadians(float degrees) 
{ 
	return (degrees * 2.0f * 3.14159f) / 360.0f;
}

void setupVertices(void)
{
	std::vector<glm::vec3> vert1 = myModel_1.getVertices();
	std::vector<glm::vec2> tex1 = myModel_1.getTextureCoords();
	std::vector<glm::vec3> norm1 = myModel_1.getNormals();

	std::vector<float> pvalues1;
	std::vector<float> tvalues1;
	std::vector<float> nvalues1;

	for (int i = 0; i < myModel_1.getNumVertices(); i++) {
		pvalues1.push_back((vert1[i]).x);
		pvalues1.push_back((vert1[i]).y);
		pvalues1.push_back((vert1[i]).z);
		tvalues1.push_back((tex1[i]).s);
		tvalues1.push_back((tex1[i]).t);
		nvalues1.push_back((norm1[i]).x);
		nvalues1.push_back((norm1[i]).y);
		nvalues1.push_back((norm1[i]).z);
	}

	std::vector<glm::vec3> vert2 = myModel_2.getVertices();
	std::vector<glm::vec2> tex2 = myModel_2.getTextureCoords();
	std::vector<glm::vec3> norm2 = myModel_2.getNormals();

	std::vector<float> pvalues2;
	std::vector<float> tvalues2;
	std::vector<float> nvalues2;

	for (int i = 0; i < myModel_2.getNumVertices(); i++) {
		pvalues2.push_back((vert2[i]).x);
		pvalues2.push_back((vert2[i]).y);
		pvalues2.push_back((vert2[i]).z);
		tvalues2.push_back((tex2[i]).s);
		tvalues2.push_back((tex2[i]).t);
		nvalues2.push_back((norm2[i]).x);
		nvalues2.push_back((norm2[i]).y);
		nvalues2.push_back((norm2[i]).z);
	}

	std::vector<glm::vec3> vert3 = myModel_3.getVertices();
	std::vector<glm::vec2> tex3 = myModel_3.getTextureCoords();
	std::vector<glm::vec3> norm3 = myModel_3.getNormals();

	std::vector<float> pvalues3;
	std::vector<float> tvalues3;
	std::vector<float> nvalues3;

	for (int i = 0; i < myModel_3.getNumVertices(); i++) {
		pvalues3.push_back((vert3[i]).x);
		pvalues3.push_back((vert3[i]).y);
		pvalues3.push_back((vert3[i]).z);
		tvalues3.push_back((tex3[i]).s);
		tvalues3.push_back((tex3[i]).t);
		nvalues3.push_back((norm3[i]).x);
		nvalues3.push_back((norm3[i]).y);
		nvalues3.push_back((norm3[i]).z);
	}

	glGenVertexArrays(numVAOs, vao);
	glGenBuffers(numVBOs, vbo);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues1.size() * 4, &pvalues1[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues1.size() * 4, &tvalues1[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues1.size() * 4, &nvalues1[0], GL_STATIC_DRAW);

	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, pvalues2.size() * 4, &pvalues2[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, tvalues2.size() * 4, &tvalues2[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, nvalues2.size() * 4, &nvalues2[0], GL_STATIC_DRAW);	

	glBindVertexArray(vao[2]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, pvalues3.size() * 4, &pvalues3[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glBufferData(GL_ARRAY_BUFFER, tvalues3.size() * 4, &tvalues3[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glBufferData(GL_ARRAY_BUFFER, nvalues3.size() * 4, &nvalues3[0], GL_STATIC_DRAW);
}

// ������ɫ������ʼ������
void init(GLFWwindow* window) 
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 1.0f;
	objLocX = 0.0f; objLocY = 0.0f; objLocZ = 0.0f;

	// ��ȡ���ڵ�֡��С
	glfwGetFramebufferSize(window, &width, &height);
	// �����߱Ȳ�����ͶӰ����
	aspect = (float)width / (float)height;
	pMat = glm::perspective(toRadians(60.0f), aspect, 0.1f, 1000.0f);

	setupVertices();

	// ��������
	texture[0] = Utils::loadTexture("Nugget/ChikinTextures/chikin_main_BaseColor.png");
	texture[1] = Utils::loadTexture("��.jpg");
	texture[2] = Utils::loadTexture("����.jpg");

}

void displayLollipop(glm::vec3 r, double currentTime)
{
	// ������������ �����ǣ����壩 ������������
    // ��y�ṫת����
	glm::mat4 parentModelMat = glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::mat4(1.0f), r);

	mMat = glm::mat4(1.0f);
	mMat = glm::translate(mMat, glm::vec3(objLocX, objLocY, objLocZ)); // ƽ��
	mMat = mMat * parentModelMat; // ��y�ṫת
	mMat = glm::translate(mMat, glm::vec3(0.1f, -0.05f, -0.1f)); // �ƶ���ԭ��
	mMat = glm::scale(mMat, glm::vec3(0.1f, 0.1f, 0.1f)); // ��С

	// ��ϱ任
	mvMat = vMat * mMat;

	// ���ݾ�����ɫ��
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// �󶨺����ö�������
	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// ����Ͱ�����
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	// ����ģ��
	glDrawArrays(GL_TRIANGLES, 0, myModel_3.getNumVertices());

	// ������������ �����Ǹˣ�Բ���� ������������
	mMat = glm::mat4(1.0f);
	mMat = glm::translate(mMat, glm::vec3(objLocX, objLocY, objLocZ)); // ƽ��
	mMat = mMat * parentModelMat; // ��y�ṫת
	mMat = glm::rotate(mMat, (float)currentTime, glm::vec3(0.0f, 0.0f, 1.0f)); // ��z����ת
	mMat = glm::scale(mMat, glm::vec3(1.0f, 8.0f, 1.0f)); // ����
	mMat = glm::scale(mMat, glm::vec3(0.02f, 0.02f, 0.02f)); // ��С

	// ��ϱ任
	mvMat = vMat * mMat;

	// ���ݾ�����ɫ��
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// �󶨺����ö�������
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// ����Ͱ�����
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[2]);

	// ����ģ��
	glDrawArrays(GL_TRIANGLES, 0, myModel_2.getNumVertices());
}

void display(GLFWwindow* window, double currentTime) 
{
	// ���������
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// ������Ȳ��Բ�������Ⱥ���
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// ʹ����ɫ������
	glUseProgram(renderingProgram);

	// ��ȡ��ɫ����Uniform������λ��
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	// ��ͼ���󣬽������λ�ø�ֵӦ���ڵ�λ�����γɴ��������������ϵ����ͼ��
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	// ������������ ���ļ���ģ�� ������������
	// ģ�;���ȷ�������ڳ����е�λ�á�
	mMat = glm::mat4(1.0f);
    // ƽ��
	mMat = glm::translate(mMat, glm::vec3(objLocX, objLocY, objLocZ));
	// glm::rotate(ԭʼ����, ��ת�Ƕ�, ��ת������);
	mMat = glm::rotate(mMat, (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::rotate(mMat, toRadians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mMat = glm::scale(mMat, glm::vec3(0.0001f, 0.0001f, 0.0001f)); // ��С

	// ��ϱ任
	mvMat = vMat * mMat;

	// ���ݾ�����ɫ��
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// �󶨺����ö�������
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// ����Ͱ�����
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	// ����ģ��
	glDrawArrays(GL_TRIANGLES, 0, myModel_1.getNumVertices());

	vector<glm::vec3> v = { glm::vec3(0.5f, 0.0f, 0.0f) ,
						  glm::vec3(-0.5f, 0.0f, 0.0f) ,
	                      glm::vec3(0.0f, 0.0f, 0.5f) ,
	                      glm::vec3(0.0f, 0.0f, -0.5f) };
	for (int i = 0; i <shapeCount; i++)
	{
		displayLollipop(v[i], currentTime);
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_RIGHT) {
			// ���ӻ��Ƶ�ͼ������
			if (shapeCount < 4){
				shapeCount++;
				std::cout << "�ҷ��������. ������������: " << shapeCount << std::endl;
			}
		}
		else if (key == GLFW_KEY_LEFT) {
			// ���ٻ��Ƶ�ͼ��������ȷ����С�� 1
			if (shapeCount > 1) {
				shapeCount--;
				std::cout << "���������. ������������: " << shapeCount << std::endl;
			}
		}
		else if (key == GLFW_KEY_UP) {
			// �Ϸ���������������ƶ�
			objLocY += 0.1f;
			std::cout << "�Ϸ��������. ��������: " << objLocY << std::endl;
		}
		else if (key == GLFW_KEY_DOWN) {
			// �·���������������ƶ�
			objLocY -= 0.1f;
			std::cout << "�·��������. ��������: " << objLocY << std::endl;
		}
	}
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight)
{
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void)
{
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Nugget", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	// ע�ᰴ���ص�����
	glfwSetKeyCallback(window, keyCallback);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}