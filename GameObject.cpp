#include "GameObject.h"

#include "Shader.h"
#include "Material.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Model.h"

GameObject::GameObject(const std::string& name, GameObjectType type)
	: name(name), type(type)
{}

// Jerarquía
void GameObject::addChild(std::shared_ptr<GameObject> child)
{
	children.push_back(child);
}

// Geometría
void GameObject::loadMesh(const MeshData& meshData)
{
	freeMesh();
	setupMesh(meshData);
	hasMesh = true;
}

void GameObject::setupMesh(const MeshData& meshData)
{
	indexCount = static_cast<GLsizei>(meshData.indices.size());

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, meshData.vertices.size() * sizeof(GLfloat), meshData.vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.indices.size() * sizeof(GLuint), meshData.indices.data(), GL_STATIC_DRAW);

    constexpr GLsizei stride = 8 * sizeof(GLfloat);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Desenlazar en el orden correcto: VAO primero, EBO despues.
    // Si se desenlaza el EBO antes del VAO, el VAO pierde la referencia al EBO.
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GameObject::freeMesh()
{
    if (VAO) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
	if (VBO) { glDeleteBuffers(1, &VBO); VBO = 0; }
	if (EBO) { glDeleteBuffers(1, &EBO); EBO = 0; }
    hasMesh = false;
	indexCount = 0;
}

// Componentes
void GameObject::setModel(Model* model)
{
    this->model = model;
    this->type = GameObjectType::MODEL;
}

void GameObject::setMaterial(Material* material)
{
    this->material = material;
}

void GameObject::setPointLight(PointLight* pointLight)
{
    this->pointLight = pointLight;
    this->type = GameObjectType::POINT_LIGHT;
}

void GameObject::setSpotLight(SpotLight* spotLight)
{
    this->spotLight = spotLight;
    this->type = GameObjectType::SPOT_LIGHT;
}

void GameObject::setTextureID(GLuint id)
{
    this->textureID = id;
	hasTexture = (id != 0);
}

// Renderizado
void GameObject::draw(Shader& shader, const glm::mat4& parentMatrix)
{
    if (!visible) return;

	glm::mat4 worldMatrix = transform.getModelMatrix(parentMatrix);
    drawSelf(shader, worldMatrix);

    for (const auto& child : children)
        child->draw(shader, worldMatrix);
}

void GameObject::drawSelf(Shader& shader, const glm::mat4& worldMatrix)
{
	syncLightPosition(worldMatrix);

	if (type == GameObjectType::EMPTY) return;

	glUniformMatrix4fv(shader.getModelLocation(), 1, GL_FALSE, glm::value_ptr(worldMatrix));

    if (material)
		material->use(shader.getSpecularIntensityLocation(), shader.getShininessLocation());

    if (hasTexture)
    {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
    }

    if (type == GameObjectType::MODEL && model)
    {
        model->render();
    }
    else if (hasMesh)
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
    }

    if (hasTexture)
		glBindTexture(GL_TEXTURE_2D, 0);
}

void GameObject::syncLightPosition(const glm::mat4& worldMatrix)
{
	glm::vec3 worldPosition = glm::vec3(worldMatrix[3]);

    if (pointLight)
		pointLight->setPosition(worldPosition); 

	if (spotLight)
		spotLight->setPosition(worldPosition);
}

GameObject::~GameObject()
{
	freeMesh();
}