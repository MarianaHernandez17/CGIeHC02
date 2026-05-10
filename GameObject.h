#pragma once

#include <string>
#include <memory>
#include <vector>

#include <glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "Transform.h"

class Shader;
class Material;
class Model;
class PointLight;
class SpotLight;

enum class GameObjectType
{
	EMPTY,
	MESH,
	MODEL,
	POINT_LIGHT,
	SPOT_LIGHT
};

struct MeshData
{
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
};

class GameObject
{
public:
	explicit GameObject(const std::string& name, GameObjectType type = GameObjectType::EMPTY);

	const std::string& getName() const { return name; }

	Transform transform;

	void addChild(std::shared_ptr<GameObject> child);
	const std::vector<std::shared_ptr<GameObject>>& getChildren() const { return children; }

	void loadMesh(const MeshData& meshData);
	void setModel(Model* model);

	void setMaterial(Material* material);
	void setPointLight(PointLight* pointLight);
	void setSpotLight(SpotLight* spotLight);

	void setTextureID(GLuint id);

	bool isVisible() const { return visible; }
	void setVisible(bool visible) { this->visible = visible; }

	void draw(Shader& shader, const glm::mat4& parentMatrix = glm::mat4(1.0f));

	~GameObject();

private:
	std::string name;
	GameObjectType type;
	bool visible = true;

	std::vector<std::shared_ptr<GameObject>> children;

	GLuint VAO = 0, VBO = 0, EBO = 0;
	GLsizei indexCount = 0;
	bool hasMesh = false;

	Model* model = nullptr;
	Material* material = nullptr;
	PointLight* pointLight = nullptr;
	SpotLight* spotLight = nullptr;
	GLuint textureID = 0;
	bool hasTexture = false;

	void setupMesh(const MeshData& meshData);
	void freeMesh();
	void drawSelf(Shader& shader, const glm::mat4& worldMatrix);
	void syncLightPosition(const glm::mat4& worldMatrix);
};