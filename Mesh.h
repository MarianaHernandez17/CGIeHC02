#pragma once
#include <glew.h>
#include <vector>

class Mesh
{
public:
	Mesh();
	void CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numberOfIndices);
	void CreateMeshGeometry(std::vector<GLfloat>& vertices, std::vector<unsigned int>& indices, unsigned int numOfVertices, unsigned int numberOfIndices);
	void RenderMesh();
	void RenderMeshGeometry();
	void ClearMesh();
	~Mesh();
private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};

class MeshColor
{
public:
	MeshColor();
	void CreateMeshColor(GLfloat *vertices, unsigned int numOfVertices);
	void RenderMeshColor();
	void ClearMeshColor();
	~MeshColor();
private:
	GLuint VAO, VBO;
	GLsizei  vertexCount;
};

class MeshLines
{
public:
	MeshLines();
	void CreateMeshLines(GLfloat* vertices, unsigned int numOfVertices);
	void RenderMeshLines();
	void RenderMeshTriangles();
	void ClearMeshLines();
	GLuint getVAO() { return VAO; }
	~MeshLines();
private:
	GLuint VAO, VBO;
	GLsizei vertexCount;
};
