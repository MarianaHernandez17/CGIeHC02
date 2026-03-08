#include <stdio.h>
#include <string.h>
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"

using std::vector;

// Variables para el control de Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Variables globales
Camera camera;
Window mainWindow;
vector<MeshColor*> meshColorList;
vector<MeshLines*> meshLinesList;
vector<Shader> shaderList;

// Archivos de shaders
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderLine = "shaders/shader_line.vert";
static const char* fShaderLine = "shaders/shader_line.frag";

void CrearTetraedro()
{
	GLfloat vertices_piramide_triangular[] = {
		// Cara 1 - Color Rojo
		 1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 0.0f, 0.0f,

		// Cara 2 - Color Verde
		 1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,	0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f,

		 // Cara 3 - Color Azul
		 -1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,
		 -1.0f,  1.0f, -1.0f,	0.0f, 0.0f, 1.0f,
		  1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f,

		  // Cara 4 - Color Amarillo
		   1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 0.0f,
		  -1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,
		   1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 0.0f
	};
	MeshColor* piramide = new MeshColor();
	piramide->CreateMeshColor(vertices_piramide_triangular, 72);
	meshColorList.push_back(piramide);
}

void CrearBordesTetraedro()
{
	// Grosor de las aristas
	float grosor = 0.1f;

	// Vértices del tetraedro
	glm::vec3 v1(1.0f, 1.0f, 1.0f);
	glm::vec3 v2(-1.0f, -1.0f, 1.0f);
	glm::vec3 v3(-1.0f, 1.0f, -1.0f);
	glm::vec3 v4(1.0f, -1.0f, -1.0f);

	std::vector<GLfloat> aristas;

	// Crear un quad entre dos puntos para representar una arista con grosor visible
	auto crearQuad = [&](glm::vec3 p1, glm::vec3 p2) {
		glm::vec3 direccion = glm::normalize(p2 - p1);

		// Encontrar un vector perpendicular a la arista
		glm::vec3 perpendicular;
		if (abs(direccion.x) < 0.9f) {
			perpendicular = glm::normalize(glm::cross(direccion, glm::vec3(1.0f, 0.0f, 0.0f)));
		}
		else {
			perpendicular = glm::normalize(glm::cross(direccion, glm::vec3(0.0f, 1.0f, 0.0f)));
		}
		glm::vec3 perpendicular2 = glm::normalize(glm::cross(direccion, perpendicular));

		// Declaración de los 4 vértices que formarán la arista con su grosor
		glm::vec3 offset1 = perpendicular * grosor;
		glm::vec3 offset2 = perpendicular2 * grosor;

		glm::vec3 v1 = p1 + offset1;
		glm::vec3 v2 = p1 - offset1;
		glm::vec3 v3 = p2 - offset1;
		glm::vec3 v4 = p2 + offset1;

		// Primer triángulo del quad
		aristas.push_back(v1.x); aristas.push_back(v1.y); aristas.push_back(v1.z);
		aristas.push_back(v2.x); aristas.push_back(v2.y); aristas.push_back(v2.z);
		aristas.push_back(v3.x); aristas.push_back(v3.y); aristas.push_back(v3.z);

		// Segundo triángulo del quad
		aristas.push_back(v1.x); aristas.push_back(v1.y); aristas.push_back(v1.z);
		aristas.push_back(v3.x); aristas.push_back(v3.y); aristas.push_back(v3.z);
		aristas.push_back(v4.x); aristas.push_back(v4.y); aristas.push_back(v4.z);

		// Segundo quad perpendicular (3D falseado)
		v1 = p1 + offset2;
		v2 = p1 - offset2;
		v3 = p2 - offset2;
		v4 = p2 + offset2;

		aristas.push_back(v1.x); aristas.push_back(v1.y); aristas.push_back(v1.z);
		aristas.push_back(v2.x); aristas.push_back(v2.y); aristas.push_back(v2.z);
		aristas.push_back(v3.x); aristas.push_back(v3.y); aristas.push_back(v3.z);

		aristas.push_back(v1.x); aristas.push_back(v1.y); aristas.push_back(v1.z);
		aristas.push_back(v3.x); aristas.push_back(v3.y); aristas.push_back(v3.z);
		aristas.push_back(v4.x); aristas.push_back(v4.y); aristas.push_back(v4.z);
		};

	// Crear quads para las 6 aristas
	crearQuad(v1, v2); // Arista 1
	crearQuad(v1, v3); // Arista 2
	crearQuad(v1, v4); // Arista 3
	crearQuad(v2, v3); // Arista 4
	crearQuad(v2, v4); // Arista 5
	crearQuad(v3, v4); // Arista 6

	MeshLines* bordes = new MeshLines();
	bordes->CreateMeshLines(aristas.data(), aristas.size());
	meshLinesList.push_back(bordes);
}

void CrearOctaedro()
{
	GLfloat vertices_octaedro[] = {
		// Cara 1 - Color Rojo
		 0.0f,  1.0f,  0.0f,	1.0f, 0.0f, 0.0f,
		 1.0f,  0.0f,  0.0f,	1.0f, 0.0f, 0.0f,
		 0.0f,  0.0f,  1.0f,	1.0f, 0.0f, 0.0f,

		 // Cara 2 - Color Rojo
		 0.0f,  1.0f,  0.0f,	1.0f, 0.0f, 0.0f,
		 0.0f,  0.0f,  1.0f,	1.0f, 0.0f, 0.0f,
		-1.0f,  0.0f,  0.0f,	1.0f, 0.0f, 0.0f,

		// Cara 3 - Color Azul
		 0.0f,  1.0f,  0.0f,	0.0f, 0.0f, 1.0f,
		-1.0f,  0.0f,  0.0f,	0.0f, 0.0f, 1.0f,
		 0.0f,  0.0f, -1.0f,	0.0f, 0.0f, 1.0f,

		 // Cara 4 - Color Amarillo
		 0.0f,  1.0f,  0.0f,	1.0f, 1.0f, 0.0f,
		 0.0f,  0.0f, -1.0f,	1.0f, 1.0f, 0.0f,
		 1.0f,  0.0f,  0.0f,	1.0f, 1.0f, 0.0f,

		 // Cara 5 - Color Verde
		  0.0f, -1.0f,  0.0f,	0.0f, 1.0f, 0.0f,
		  0.0f,  0.0f,  1.0f,    0.0f, 1.0f, 0.0f,
		  1.0f,  0.0f,  0.0f,    0.0f, 1.0f, 0.0f,

		  // Cara 6 - Color Rojo
		  0.0f, -1.0f,  0.0f,	1.0f, 0.0f, 0.0f,
		 -1.0f,  0.0f,  0.0f,	1.0f, 0.0f, 0.0f,
		  0.0f,  0.0f,  1.0f,	1.0f, 0.0f, 0.0f,

		  // Cara 7 - Color Azul
		  0.0f, -1.0f,  0.0f,	0.0f, 0.0f, 1.0f,
		  0.0f,  0.0f, -1.0f,	0.0f, 0.0f, 1.0f,
		 -1.0f,  0.0f,  0.0f,	0.0f, 0.0f, 1.0f,

		 // Cara 8 - Color Amarillo
		  0.0f, -1.0f,  0.0f,	1.0f, 1.0f, 0.0f,
		  1.0f,  0.0f,  0.0f,	1.0f, 1.0f, 0.0f,
		  0.0f,  0.0f, -1.0f,	1.0f, 1.0f, 0.0f
	};
	MeshColor* octaedro = new MeshColor();
	octaedro->CreateMeshColor(vertices_octaedro, 144);
	meshColorList.push_back(octaedro);
}

void CrearBordesOctaedro()
{
	// Grosor de las aristas
	float grosor = 0.05f;

	// Vértices del octaedro
	glm::vec3 v_superior(0.0f, 1.0f, 0.0f);
	glm::vec3 v_inferior(0.0f, -1.0f, 0.0f);
	glm::vec3 v_derecha(1.0f, 0.0f, 0.0f);
	glm::vec3 v_izquierda(-1.0f, 0.0f, 0.0f);
	glm::vec3 v_frente(0.0f, 0.0f, 1.0f);
	glm::vec3 v_atras(0.0f, 0.0f, -1.0f);

	std::vector<GLfloat> bordes;

	// Crear un quad entre dos puntos para representar una arista con grosor visible
	auto crearQuad = [&](glm::vec3 p1, glm::vec3 p2) {
		glm::vec3 direccion = glm::normalize(p2 - p1);

		// Encontrar un vector perpendicular a la arista
		glm::vec3 perpendicular;
		if (abs(direccion.x) < 0.9f) {
			perpendicular = glm::normalize(glm::cross(direccion, glm::vec3(1.0f, 0.0f, 0.0f)));
		}
		else {
			perpendicular = glm::normalize(glm::cross(direccion, glm::vec3(0.0f, 1.0f, 0.0f)));
		}
		glm::vec3 perpendicular2 = glm::normalize(glm::cross(direccion, perpendicular));

		// Declaración de los 4 vértices que formarán la arista con su grosor
		glm::vec3 offset1 = perpendicular * grosor;
		glm::vec3 offset2 = perpendicular2 * grosor;

		glm::vec3 v1 = p1 + offset1;
		glm::vec3 v2 = p1 - offset1;
		glm::vec3 v3 = p2 - offset1;
		glm::vec3 v4 = p2 + offset1;

		// Primer triángulo del quad
		bordes.push_back(v1.x); bordes.push_back(v1.y); bordes.push_back(v1.z);
		bordes.push_back(v2.x); bordes.push_back(v2.y); bordes.push_back(v2.z);
		bordes.push_back(v3.x); bordes.push_back(v3.y); bordes.push_back(v3.z);

		// Segundo triángulo del quad
		bordes.push_back(v1.x); bordes.push_back(v1.y); bordes.push_back(v1.z);
		bordes.push_back(v3.x); bordes.push_back(v3.y); bordes.push_back(v3.z);
		bordes.push_back(v4.x); bordes.push_back(v4.y); bordes.push_back(v4.z);

		// Segundo quad perpendicular (3D falseado)
		v1 = p1 + offset2;
		v2 = p1 - offset2;
		v3 = p2 - offset2;
		v4 = p2 + offset2;

		bordes.push_back(v1.x); bordes.push_back(v1.y); bordes.push_back(v1.z);
		bordes.push_back(v2.x); bordes.push_back(v2.y); bordes.push_back(v2.z);
		bordes.push_back(v3.x); bordes.push_back(v3.y); bordes.push_back(v3.z);

		bordes.push_back(v1.x); bordes.push_back(v1.y); bordes.push_back(v1.z);
		bordes.push_back(v3.x); bordes.push_back(v3.y); bordes.push_back(v3.z);
		bordes.push_back(v4.x); bordes.push_back(v4.y); bordes.push_back(v4.z);
		};

	// Crear quads para las 12 aristas del octaedro
	crearQuad(v_superior, v_derecha);
	crearQuad(v_superior, v_frente);
	crearQuad(v_superior, v_izquierda);
	crearQuad(v_superior, v_atras);
	crearQuad(v_inferior, v_derecha);
	crearQuad(v_inferior, v_frente);
	crearQuad(v_inferior, v_izquierda);
	crearQuad(v_inferior, v_atras);
	crearQuad(v_derecha, v_frente);
	crearQuad(v_frente, v_izquierda);
	crearQuad(v_izquierda, v_atras);
	crearQuad(v_atras, v_derecha);

	MeshLines* bordesOctaedro = new MeshLines();
	bordesOctaedro->CreateMeshLines(bordes.data(), bordes.size());
	meshLinesList.push_back(bordesOctaedro);
}


void CreateShaders()
{
	// Shader para renderizar las figuras con colores
	Shader* shader = new Shader();
	shader->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader);

	// Shader para renderizar los bordes negros
	Shader* shaderLine = new Shader();
	shaderLine->CreateFromFiles(vShaderLine, fShaderLine);
	shaderList.push_back(*shaderLine);
}

void RenderizarTetraedro(glm::vec3 position, glm::mat4 projection, Camera& camera)
{
	GLuint uniformModel, uniformProjection, uniformView;
	glm::mat4 model(1.0);

	// Realizar la rotación global según el input
	model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));

	// Trasladar a la posición deseada
	model = glm::translate(model, position);

	// Renderizar el tetraedro con colores
	shaderList[0].useShader();
	uniformModel = shaderList[0].getModelLocation();
	uniformProjection = shaderList[0].getProjectLocation();
	uniformView = shaderList[0].getViewLocation();

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

	meshColorList[0]->RenderMeshColor();

	// Renderizar los bordes negros
	shaderList[1].useShader();
	uniformModel = shaderList[1].getModelLocation();
	uniformProjection = shaderList[1].getProjectLocation();
	uniformView = shaderList[1].getViewLocation();
	GLuint uniformLineColor = shaderList[1].getColorLocation();

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
	glm::vec3 lineColor = glm::vec3(0.0f, 0.0f, 0.0f);
	glUniform3fv(uniformLineColor, 1, glm::value_ptr(lineColor));

	meshLinesList[0]->RenderMeshTriangles();
}

void RenderizarOctaedro(glm::vec3 position, glm::mat4 projection, Camera& camera)
{
	GLuint uniformModel, uniformProjection, uniformView;
	glm::mat4 model(1.0);

	// Realizar la rotación global según el input
	model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));

	// Trasladar a la posición deseada
	model = glm::translate(model, position);

	// Escalar el octaedro para que ocupe el espacio requerido
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

	// Renderizar el octaedro con colores
	shaderList[0].useShader();
	uniformModel = shaderList[0].getModelLocation();
	uniformProjection = shaderList[0].getProjectLocation();
	uniformView = shaderList[0].getViewLocation();

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

	meshColorList[1]->RenderMeshColor();

	// Renderizar los bordes negros
	shaderList[1].useShader();
	uniformModel = shaderList[1].getModelLocation();
	uniformProjection = shaderList[1].getProjectLocation();
	uniformView = shaderList[1].getViewLocation();
	GLuint uniformLineColor = shaderList[1].getColorLocation();

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
	glm::vec3 lineColor = glm::vec3(0.0f, 0.0f, 0.0f);
	glUniform3fv(uniformLineColor, 1, glm::value_ptr(lineColor));

	meshLinesList[1]->RenderMeshTriangles();
}

int main()
{
	// Dimensiones de la ventana
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	// Inicializar los shaders
	CreateShaders();

	// Crear el tetraedro
	CrearTetraedro();
	CrearBordesTetraedro();

	// Crear el octaedro
	CrearOctaedro();
	CrearBordesOctaedro();



	// Configuración de la cámara y proyección en perspectiva
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	while (!mainWindow.getShouldClose())
	{
		// Control de Delta Time a 60 FPS
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Construcción de los tetraedros para el pyraminx
		RenderizarTetraedro(glm::vec3(1.0f, 1.0f, 1.0f), projection, camera);
		RenderizarTetraedro(glm::vec3(-1.0f, -1.0f, 1.0f), projection, camera);
		RenderizarTetraedro(glm::vec3(-1.0f, 1.0f, -1.0f), projection, camera);
		RenderizarTetraedro(glm::vec3(1.0f, -1.0f, -1.0f), projection, camera);
		RenderizarTetraedro(glm::vec3(3.0f, 1.0f, 3.0f), projection, camera);
		RenderizarTetraedro(glm::vec3(1.0f, -1.0f, 3.0f), projection, camera);
		RenderizarTetraedro(glm::vec3(3.0f, -1.0f, 1.0f), projection, camera);
		RenderizarTetraedro(glm::vec3(1.0f, -3.0f, 1.0f), projection, camera);
		RenderizarTetraedro(glm::vec3(-1.0f, -3.0f, 3.0f), projection, camera);
		RenderizarTetraedro(glm::vec3(3.0f, -3.0f, -1.0f), projection, camera);

		// Construcción de los octaedros para el pyraminx
		RenderizarOctaedro(glm::vec3(0.0f, 0.0f, 0.0f), projection, camera);
		RenderizarOctaedro(glm::vec3(2.0f, 0.0f, 2.0f), projection, camera);
		RenderizarOctaedro(glm::vec3(0.0f, -2.0f, 2.0f), projection, camera);
		RenderizarOctaedro(glm::vec3(2.0f, -2.0f, 0.0f), projection, camera);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}