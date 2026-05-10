#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Transform
{
public:
	Transform();
	Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	// Setters
	void setPosition(const glm::vec3& position);
	void setPosition(float x, float y, float z);
	void setRotation(const glm::vec3& rotation);
	void setRotation(float x, float y, float z);
	void setScale(const glm::vec3& scale);
	void setScale(float x, float y, float z);
	void setScale(float uniform);

	// Getters
	glm::vec3 getPosition() const { return position; }
	glm::vec3 getRotation() const { return rotation; }
	glm::vec3 getScale() const { return scale; }

	// Modificadores relativos
	void translate(const glm::vec3& delta);
	void translate(float x, float y, float z);
	void rotate(const glm::vec3& delta);
	void rotate(float x, float y, float z);

	// Matriz de transformación
	glm::mat4 getModelMatrix() const;
	glm::mat4 getModelMatrix(const glm::mat4& parentMatrix) const;

	~Transform();

private:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};