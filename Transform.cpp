#include "Transform.h"

// Constructores
Transform::Transform()
	: position(0.0f), rotation(0.0f), scale(1.0f)
{}

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
	: position(position), rotation(rotation), scale(scale)
{}

// Setters Absolutos
// Posición
void Transform::setPosition(const glm::vec3& position)
{
	this->position = position;
}

void Transform::setPosition(float x, float y, float z)
{
	this->position = glm::vec3(x, y, z);
}

// Rotación
void Transform::setRotation(const glm::vec3& rotation)
{
	this->rotation = rotation;
}

void Transform::setRotation(float x, float y, float z)
{
	this->rotation = glm::vec3(x, y, z);
}

// Escala
void Transform::setScale(const glm::vec3& scale)
{
	this->scale = scale;
}

void Transform::setScale(float x, float y, float z)
{
	this->scale = glm::vec3(x, y, z);
}

void Transform::setScale(float uniform)
{
	this->scale = glm::vec3(uniform);
}

// Modificadores Relativos
// Traslación
void Transform::translate(const glm::vec3& delta)
{
	this->position += delta;
}

void Transform::translate(float x, float y, float z)
{
	this->position += glm::vec3(x, y, z);
}

// Rotación
void Transform::rotate(const glm::vec3& delta)
{
	this->rotation += delta;
}

void Transform::rotate(float x, float y, float z)
{
	this->rotation += glm::vec3(x, y, z);
}

// Matriz de transformación
// Calcula la matriz con el orden de transformación: Escala -> Rotación (X, Y, Z) -> Traslación
glm::mat4 Transform::getModelMatrix() const
{
	glm::mat4 model(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, scale);
	return model;
}

// Se calcula la matriz considerando la matriz del padre, de forma recursiva
glm::mat4 Transform::getModelMatrix(const glm::mat4& parentMatrix) const
{
	return parentMatrix * getModelMatrix();
}

// Destructor
Transform::~Transform()
{}