#include <vector>

class ImportedModel
{
private:
	int numVertices; // 顶点数量
	std::vector<glm::vec3> vertices; // 顶点坐标
	std::vector<glm::vec2> texCoords; // 纹理坐标
	std::vector<glm::vec3> normalVecs; // 法向量
public:
	ImportedModel();
	ImportedModel(const char *filePath);
	int getNumVertices();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec2> getTextureCoords();
	std::vector<glm::vec3> getNormals();
};

class ModelImporter
{
private:
	std::vector<float> vertVals;
	std::vector<float> triangleVerts;
	std::vector<float> textureCoords;
	std::vector<float> stVals;
	std::vector<float> normals;
	std::vector<float> normVals;
public:
	ModelImporter();
	void parseOBJ(const char *filePath);
	int getNumVertices();
	std::vector<float> getVertices();
	std::vector<float> getTextureCoordinates();
	std::vector<float> getNormals();
};