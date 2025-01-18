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

#include "Square.h"
#include "Sphere.h"
#include "ImportedModel.h"
#include "Utils.h"
using namespace std;

#define numVAOs 4
#define numVBOs 14

bool lightisRotating = false; // 控制是否旋转
float horizon = -90.0f;    // 初始水平角度，朝向 -Z 方向
float vertical = 0.0f;    // 初始垂直角度
float sensitivity = 1.0f; // 视角改变灵敏度
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // 摄像机初始朝向
float cameraSpeed = 0.05f; // 摄像机移动速度

float cameraX, cameraY, cameraZ;
float objLocX, objLocY, objLocZ;
// float lightLocX, lightLocY, lightLocZ;
GLuint renderingProgram;
GLuint skyboxProgram;        // 新增天空盒着色器程序
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint texture[numVAOs];
GLuint skyboxVAO, skyboxVBO;
GLuint skyboxTexture;

glm::vec3 lightLoc = glm::vec3(2.0f, 0.0f, 2.0f); // 初始光源位置
float amt = 0.0f;

// variable allocation for display
GLuint mvLoc, projLoc, nLoc, colorMapLoc, useLightLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat, rMat;
glm::vec3 currentLightPos, transformed; // currentLightPos：动态光源的位置
float lightPos[3]; // 光源位置

int shapeCount = 1;

float globalAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f }; // 全局环境光
float lightAmbient[4] = { 0.5f, 0.5f, 0.5f, 1.0f };  // 光源环境光
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };  // 光源漫反射光
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 光源镜面光

// gold material
float* gold_matAmb = Utils::goldAmbient();
float* gold_matDif = Utils::goldDiffuse();
float* gold_matSpe = Utils::goldSpecular();
float gold_matShi = Utils::goldShininess();

// 粉色高光材质
float pink_matAmb[4] = { 0.6f, 0.2f, 0.6f, 1.0f };    // 环境光，带有淡粉色
float pink_matDif[4] = { 1.0f, 0.4f, 1.0f, 1.0f };    // 漫反射光，较亮的粉色
float pink_matSpe[4] = { 1.0f, 0.8f, 1.0f, 1.0f };   // 镜面光，高光反射带有粉色调
float pink_matShi = 64.0f;                         // 高光度（较高值，表示更聚焦的反射光点）

// 银色材质
float* silver_matAmb = Utils::silverAmbient();
float* silver_matDif = Utils::silverDiffuse();
float* silver_matSpe = Utils::silverSpecular();
float silver_matShi = Utils::silverShininess();

ImportedModel myModel_1("Nugget.obj");
ImportedModel myModel_2("cylinder.obj");
Sphere myModel_3(48);
Square myModel_4(100);

float toRadians(float degrees)
{
	return (degrees * 2.0f * 3.14159f) / 360.0f;
}

void installLights(glm::mat4 vMatrix, float* matAmbient, float* matDiffuse, float* matSpecular, float matShininess) {
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// 获取光照和材质的 uniform 位置
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");
	mambLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mdiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mspecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mshiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	// 设置光照参数
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);

	// 设置材质参数
	glProgramUniform4fv(renderingProgram, mambLoc, 1, matAmbient);
	glProgramUniform4fv(renderingProgram, mdiffLoc, 1, matDiffuse);
	glProgramUniform4fv(renderingProgram, mspecLoc, 1, matSpecular);
	glProgramUniform1f(renderingProgram, mshiLoc, matShininess);
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
		pvalues1.push_back(vert1[i].x);
		pvalues1.push_back(vert1[i].y);
		pvalues1.push_back(vert1[i].z);
		tvalues1.push_back(tex1[i].s);
		tvalues1.push_back(tex1[i].t);
		nvalues1.push_back(norm1[i].x);
		nvalues1.push_back(norm1[i].y);
		nvalues1.push_back(norm1[i].z);
	}

	std::vector<glm::vec3> vert2 = myModel_2.getVertices();
	std::vector<glm::vec2> tex2 = myModel_2.getTextureCoords();
	std::vector<glm::vec3> norm2 = myModel_2.getNormals();

	std::vector<float> pvalues2;
	std::vector<float> tvalues2;
	std::vector<float> nvalues2;

	for (int i = 0; i < myModel_2.getNumVertices(); i++) {
		pvalues2.push_back(vert2[i].x);
		pvalues2.push_back(vert2[i].y);
		pvalues2.push_back(vert2[i].z);
		tvalues2.push_back(tex2[i].s);
		tvalues2.push_back(tex2[i].t);
		nvalues2.push_back(norm2[i].x);
		nvalues2.push_back(norm2[i].y);
		nvalues2.push_back(norm2[i].z);
	}

	std::vector<int> ind3 = myModel_3.getIndices();
	std::vector<glm::vec3> vert3 = myModel_3.getVertices();
	std::vector<glm::vec2> tex3 = myModel_3.getTexCoords();
	std::vector<glm::vec3> norm3 = myModel_3.getNormals();
	std::vector<glm::vec3> tang3 = myModel_3.getTangents();

	std::vector<float> pvalues3;
	std::vector<float> tvalues3;
	std::vector<float> nvalues3;
	std::vector<float> tanvalues3;

	for (int i = 0; i < myModel_3.getNumIndices(); i++) {
		pvalues3.push_back(vert3[ind3[i]].x);
		pvalues3.push_back(vert3[ind3[i]].y);
		pvalues3.push_back(vert3[ind3[i]].z);
		tvalues3.push_back(tex3[ind3[i]].s);
		tvalues3.push_back(tex3[ind3[i]].t);
		nvalues3.push_back(norm3[ind3[i]].x);
		nvalues3.push_back(norm3[ind3[i]].y);
		nvalues3.push_back(norm3[ind3[i]].z);
		tanvalues3.push_back((tang3[ind3[i]]).x);
		tanvalues3.push_back((tang3[ind3[i]]).y);
		tanvalues3.push_back((tang3[ind3[i]]).z);
	}

	std::vector<int> ind4 = myModel_4.getIndices();
	std::vector<glm::vec3> vert4 = myModel_4.getVertices();
	std::vector<glm::vec2> tex4 = myModel_4.getTexCoords();
	std::vector<glm::vec3> norm4 = myModel_4.getNormals();
	std::vector<glm::vec3> tang4 = myModel_4.getTangents();

	std::vector<float> pvalues4;
	std::vector<float> tvalues4;
	std::vector<float> nvalues4;
	std::vector<float> tanvalues4;

	for (int i = 0; i < myModel_4.getNumIndices(); i++) {
		pvalues4.push_back(vert4[ind4[i]].x);
		pvalues4.push_back(vert4[ind4[i]].y);
		pvalues4.push_back(vert4[ind4[i]].z);
		tvalues4.push_back(tex4[ind4[i]].s);
		tvalues4.push_back(tex4[ind4[i]].t);
		nvalues4.push_back(norm4[ind4[i]].x);
		nvalues4.push_back(norm4[ind4[i]].y);
		nvalues4.push_back(norm4[ind4[i]].z);
		tanvalues4.push_back((tang4[ind4[i]]).x);
		tanvalues4.push_back((tang4[ind4[i]]).y);
		tanvalues4.push_back((tang4[ind4[i]]).z);
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

	glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
	glBufferData(GL_ARRAY_BUFFER, tanvalues3.size() * 4, &tanvalues3[0], GL_STATIC_DRAW);

	glBindVertexArray(vao[3]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
	glBufferData(GL_ARRAY_BUFFER, pvalues4.size() * 4, &pvalues4[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
	glBufferData(GL_ARRAY_BUFFER, tvalues4.size() * 4, &tvalues4[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);
	glBufferData(GL_ARRAY_BUFFER, nvalues4.size() * 4, &nvalues4[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[13]);
	glBufferData(GL_ARRAY_BUFFER, tanvalues4.size() * 4, &tanvalues4[0], GL_STATIC_DRAW);
}


// 导入着色器，初始化窗口
void init(GLFWwindow* window)
{
	renderingProgram = Utils::createShaderProgram("./vertShader.glsl", "./fragShader.glsl");
	skyboxProgram = Utils::createShaderProgram("./skyboxVertShader.glsl", "./skyboxFragShader.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 1.0f;
	objLocX = 0.0f; objLocY = 0.0f; objLocZ = 0.0f;

	// 获取窗口的帧大小
	glfwGetFramebufferSize(window, &width, &height);
	// 计算宽高比并设置投影矩阵
	aspect = (float)width / (float)height;
	// 计算宽高比并检查有效性
	aspect = (float)width / (float)height;
	if (aspect > 0.0f) {
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	}

	setupVertices();

	// 加载纹理
	texture[0] = Utils::loadTexture("./Nugget/ChikinTextures/chikin_main_BaseColor.png");
	texture[1] = Utils::loadTexture("./糖.jpg");
	texture[2] = Utils::loadTexture("./金属.jpg");
	texture[3] = Utils::loadTexture("./背景.jpg");

	// 天空盒顶点数据
	float skyboxVertices[] = {
		-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};

	// 创建和设置天空盒VAO/VBO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);


	skyboxTexture = Utils::loadCubeMap("./skybox1"); //包含天空盒纹理的文件夹
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);// 混合立方体相邻的边以减少或消除接缝
}

void displayLollipop(glm::vec3 r, double currentTime)
{
	// 绕y轴公转操作
	glm::mat4 parentModelMat = glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::mat4(1.0f), r);
	
	// —————— 棒棒糖（球体） ——————
	mMat = glm::mat4(1.0f);
	mMat = glm::translate(mMat, glm::vec3(objLocX, objLocY, objLocZ)); // 平移
	//mMat = mMat * parentModelMat;
	mMat = glm::translate(mMat, r);
	mMat = glm::scale(mMat, glm::vec3(0.05f, 0.05f, 0.05f)); // 缩小

	installLights(vMat, pink_matAmb, pink_matDif, pink_matSpe, pink_matShi);
	// 组合变换
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	// 传递矩阵到着色器
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	// 绑定和设置顶点属性
	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);

	// 激活并绑定法线贴图和颜色纹理
	glActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glUniform1i(colorMapLoc, 1);

	glUniform1i(useLightLoc, 1);  // 开启光照
	// 绘制模型
	glDrawArrays(GL_TRIANGLES, 0, myModel_3.getNumIndices());

	// —————— 棒棒糖杆（圆柱） ——————
	mMat = glm::mat4(1.0f);
	mMat = glm::translate(mMat, glm::vec3(objLocX, objLocY, objLocZ)); // 平移
	//mMat = mMat * parentModelMat; // 绕y轴公转
	//mMat = glm::rotate(mMat, (float)currentTime, glm::vec3(0.0f, 0.0f, 1.0f)); // 绕z轴旋转
	mMat = glm::translate(mMat, r);
	mMat = glm::scale(mMat, glm::vec3(1.0f, -8.0f, 1.0f)); // 拉长
	mMat = glm::scale(mMat, glm::vec3(0.02f, 0.02f, 0.02f)); // 缩小

	installLights(vMat, silver_matAmb, silver_matDif, silver_matSpe, silver_matShi);
	// 组合变换
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	// 传递矩阵到着色器
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	// 绑定和设置顶点属性
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// 激活并绑定法线贴图和颜色纹理
	glActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glUniform1i(colorMapLoc, 1);

	glUniform1i(useLightLoc, 1);  // 开启光照
	// 绘制模型
	glDrawArrays(GL_TRIANGLES, 0, myModel_2.getNumVertices());
}

void display(GLFWwindow* window, double currentTime)
{
	// 清除缓冲区
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glDisable(GL_DEPTH_TEST);// 取消深度测试绘制天空盒

	// 先使用天空盒着色器进行渲染
	glUseProgram(skyboxProgram);

	// 获取天空盒着色器中uniform变量的位置
	GLuint skyboxViewLoc = glGetUniformLocation(skyboxProgram, "view");
	GLuint skyboxProjLoc = glGetUniformLocation(skyboxProgram, "projection");
	GLuint skyboxTexLoc = glGetUniformLocation(skyboxProgram, "skybox");

	// 视图矩阵，将摄像机位置负值应用于单位矩阵，形成从相机到世界坐标系的视图。
	// 动态更新视图矩阵（使用摄像机的位置和方向）
	vMat = glm::lookAt(
		glm::vec3(cameraX, cameraY, cameraZ),                // 摄像机位置
		glm::vec3(cameraX, cameraY, cameraZ) + cameraFront, // 目标点
		glm::vec3(0.0f, 1.0f, 0.0f)                         // 上方向
	);

	// 设置观察矩阵 - 移除位移部分
	glm::mat4 skyboxView = glm::mat4(glm::mat3(vMat)); // 移除vMat位移，让天空盒渲染不受相机移动影响
	glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(skyboxView));
	glUniformMatrix4fv(skyboxProjLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniform1i(skyboxTexLoc, 0);

	// 绘制天空盒
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 启用深度测试
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// 使用着色器程序
	glUseProgram(renderingProgram);
	glDepthFunc(GL_LESS);
	// 获取着色器中Uniform变量的位置
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");
	colorMapLoc = glGetUniformLocation(renderingProgram, "colorMap");
	useLightLoc = glGetUniformLocation(renderingProgram, "useLight");



	currentLightPos = glm::vec3(lightLoc.x, lightLoc.y, lightLoc.z); // 初始化光源位置
	if (lightisRotating)
	{
		amt += 0.5f; // 更新光源旋转角度
	}
	rMat = glm::rotate(glm::mat4(1.0f), toRadians(amt), glm::vec3(0.0f, 1.0f, 0.0f)); // 计算旋转矩阵
	currentLightPos = glm::vec3(rMat * glm::vec4(currentLightPos, 1.0f)); // 应用旋转矩阵

	// —————— 中心鸡块模型 ——————
	// 模型矩阵，确定物体在场景中的位置。
	mMat = glm::mat4(1.0f);
	// 平移
	mMat = glm::translate(mMat, glm::vec3(objLocX, objLocY, objLocZ));
	// glm::rotate(原始矩阵, 旋转角度, 旋转轴向量);
	mMat = glm::rotate(mMat, (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::rotate(mMat, toRadians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mMat = glm::scale(mMat, glm::vec3(0.0001f, 0.0001f, 0.0001f)); // 缩小

	installLights(vMat, gold_matAmb, gold_matDif, gold_matSpe, gold_matShi);
	// 组合变换
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	// 传递矩阵到着色器
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	// 绑定和设置顶点属性
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// 激活并绑定法线贴图和颜色纹理
	glActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glUniform1i(colorMapLoc, 1);

	glUniform1i(useLightLoc, 1);  // 开启光照
	// 绘制模型
	glDrawArrays(GL_TRIANGLES, 0, myModel_1.getNumVertices());

	// —————— 背景天空模型 ——————
	//// 绑定和设置顶点属性
	//glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(1);

	//glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(2);

	//glBindBuffer(GL_ARRAY_BUFFER, vbo[13]);
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(3);

	//// 激活并绑定法线贴图和颜色纹理
	//glActiveTexture(GL_TEXTURE0);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, texture[3]);
	//glUniform1i(colorMapLoc, 1);

	//glUniform1i(useLightLoc, 0); // 关闭光照

	/*背景图片滚动*/
	//float xOffset = fmod((float)currentTime, 10.0f) + 5.0f;
	//// 绘制第一个正方形
	//mMat = glm::mat4(1.0f);
	//mMat = glm::translate(mMat, glm::vec3(xOffset, objLocY, -5.0f));
	//mMat = glm::scale(mMat, glm::vec3(5.0f, 5.0f, 5.0f));

	//mvMat = vMat * mMat;
	//invTrMat = glm::transpose(glm::inverse(mvMat));

	//glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	//glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	//glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	//glDrawArrays(GL_TRIANGLES, 0, myModel_4.getNumIndices());

	//// 绘制第二个正方形
	//mMat = glm::mat4(1.0f);
	//mMat = glm::translate(mMat, glm::vec3(xOffset - 10.0f, objLocY, -5.0f)); // 左侧
	//mMat = glm::scale(mMat, glm::vec3(5.0f, 5.0f, 5.0f));

	//mvMat = vMat * mMat;
	//glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));

	//glDrawArrays(GL_TRIANGLES, 0, myModel_4.getNumIndices());

	//// 绘制第三个正方形
	//mMat = glm::mat4(1.0f);
	//mMat = glm::translate(mMat, glm::vec3(xOffset - 20.0f, objLocY, -5.0f)); // 更左侧
	//mMat = glm::scale(mMat, glm::vec3(5.0f, 5.0f, 5.0f));

	//mvMat = vMat * mMat;
	//glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));

	//glDrawArrays(GL_TRIANGLES, 0, myModel_4.getNumIndices());


	// —————— 棒棒糖模型 ——————
	vector<glm::vec3> v = { glm::vec3(0.5f, 0.0f, 0.0f) ,
						  glm::vec3(-0.5f, 0.0f, 0.0f) ,
						  glm::vec3(0.0f, 0.0f, 0.5f) ,
						  glm::vec3(0.0f, 0.0f, -0.5f) };
	for (int i = 0; i < shapeCount; i++)
	{
		displayLollipop(v[i], currentTime);
	}

}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT ) {
		if (key == GLFW_KEY_W) {
			// 前进
			cameraX += cameraSpeed * cameraFront.x;
			cameraY += cameraSpeed * cameraFront.y;
			cameraZ += cameraSpeed * cameraFront.z;
		}
		else if (key == GLFW_KEY_S) {
			// 后退
			cameraX -= cameraSpeed * cameraFront.x;
			cameraY -= cameraSpeed * cameraFront.y;
			cameraZ -= cameraSpeed * cameraFront.z;
		}
		else if (key == GLFW_KEY_A) {
			// 左移
			glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
			cameraX -= cameraSpeed * cameraRight.x;
			cameraY -= cameraSpeed * cameraRight.y;
			cameraZ -= cameraSpeed * cameraRight.z;
		}
		else if (key == GLFW_KEY_D) {
			// 右移
			glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
			cameraX += cameraSpeed * cameraRight.x;
			cameraY += cameraSpeed * cameraRight.y;
			cameraZ += cameraSpeed * cameraRight.z;
		}
		if (key == GLFW_KEY_UP) {
			// 增加 vertical（向上看）
			vertical += sensitivity;
			if (vertical > 89.0f) vertical = 89.0f; // 限制 vertical 范围
		}
		else if (key == GLFW_KEY_DOWN) {
			// 减少 vertical（向下看）
			vertical -= sensitivity;
			if (vertical < -89.0f) vertical = -89.0f; // 限制 vertical 范围
		}
		else if (key == GLFW_KEY_LEFT) {
			// 减少 horizon（向左看）
			horizon -= sensitivity;
		}
		else if (key == GLFW_KEY_RIGHT) {
			// 增加 horizon（向右看）
			horizon += sensitivity;
		}
		else if (key == GLFW_KEY_SPACE) {
			// 向上移动
			cameraY += cameraSpeed;
		}
		else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			// 向下移动
			cameraY -= cameraSpeed;
		}
		else if (key == GLFW_KEY_KP_ADD) {
			if (shapeCount < 4) {
				shapeCount++;
			}
		}
		else if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS) {
			if (shapeCount > 1) {
				shapeCount--;
			}
		}

		// 更新摄像机的方向向量
		cameraFront.x = cos(glm::radians(horizon)) * cos(glm::radians(vertical));
		cameraFront.y = sin(glm::radians(vertical));
		cameraFront.z = sin(glm::radians(horizon)) * cos(glm::radians(vertical));
		cameraFront = glm::normalize(cameraFront); // 保证方向向量的长度为 1
	}
	if (action == GLFW_PRESS || action == GLFW_REPEAT || action == GLFW_RELEASE) {
		if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
			lightisRotating = true; // 光源开始旋转
		}
		else if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE) {
			lightisRotating = false; // 光源停止旋转
		}
	}
}


void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	// 确保宽高至少为1，避免除以0
	width = std::max(1, newWidth);
	height = std::max(1, newHeight);
	aspect = (float)width / (float)height;

	glViewport(0, 0, width, height);
	// 添加错误检查
	if (aspect > 0.0f) {
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	}
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

	// 注册按键回调函数
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