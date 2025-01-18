#include <cmath>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "Square.h"

using namespace std;

Square::Square() {
    init(48);
}

Square::Square(int prec) {
    init(prec);
} 


void Square::init(int prec) {
    numVertices = (prec+1)*(prec+1);
    numIndices = prec * prec* 6;
    for (int i = 0; i < numVertices; i++) {
        vertices.push_back(glm::vec3());
        texCoords.push_back(glm::vec2());
        normals.push_back(glm::vec3());
        tangents.push_back(glm::vec3());
    }
    for (int i = 0; i < numIndices; i++) {indices.push_back(0);}


    // calculate vertices
    for (int i = 0; i <= prec; i++) {
        for (int j = 0; j <= prec; j++) {
            float x = -1.0f + 2.0f * (float)j / prec;
            float y = -1.0f + 2.0f * (float)i / prec;

            vertices[i * (prec + 1) + j] = glm::vec3(x, y, 0.0f);
            texCoords[i * (prec + 1) + j] = glm::vec2(x * 0.5f + 0.5f, y * 0.5f + 0.5f);
            normals[i * (prec + 1) + j] = glm::vec3(0.0f, 0.0f, 1.0f);  
            tangents[i * (prec + 1) + j] = glm::vec3(1.0f, 0.0f, 0.0f); 
        }
    }

    //calculate indices
    for (int i = 0; i < prec; i++) {
        for (int j = 0; j < prec; j++) {
            indices[6 * (i * prec + j) + 0] = i * (prec + 1) + j;
            indices[6 * (i * prec + j) + 1] = i * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 2] = (i + 1) * (prec + 1) + j;
            indices[6 * (i * prec + j) + 3] = i * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 4] = (i + 1) * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 5] = (i + 1) * (prec + 1) + j;
        }
    }
}

int Square::getNumVertices() { return numVertices; }
int Square::getNumIndices() { return numIndices; }
std::vector<int> Square::getIndices() { return indices; }
std::vector<glm::vec3> Square::getVertices() { return vertices; }
std::vector<glm::vec2> Square::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Square::getNormals() { return normals; }
std::vector<glm::vec3> Square::getTangents() { return tangents; }
