#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Para rand() y srand()
#include <time.h>   // Para time()
#include <glew.h>
#include <glfw3.h>

// Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

// LENGUAJE DE SHADER (SOMBRAS) GLSL
// Vertex Shader
static const char* vShader = "                      \n\
#version 330                                        \n\
layout (location =0) in vec3 pos;                   \n\
void main()                                         \n\
{                                                   \n\
gl_Position=vec4(pos.x,pos.y,pos.z,1.0f);           \n\
}";

// Fragment Shader (Color de las letras: Rojo sólido)
static const char* fShader = "                      \n\
#version 330                                        \n\
out vec4 color;                                     \n\
void main()                                         \n\
{                                                   \n\
    color = vec4(1.0f,0.0f,0.0f,1.0f);              \n\
}";

void CrearLetras()
{
    // Arreglo de vértices para las letras M, D, H
    // Cada rectángulo está formado por 2 triángulos (6 vértices)
    GLfloat vertices[] = {
        // --- LETRA M (Abajo a la izquierda) --- 24 vértices
        // Palo izquierdo
        -0.9f, -0.9f, 0.0f,   -0.8f, -0.9f, 0.0f,   -0.9f, -0.5f, 0.0f,
        -0.8f, -0.9f, 0.0f,   -0.8f, -0.5f, 0.0f,   -0.9f, -0.5f, 0.0f,
        // Palo derecho
        -0.5f, -0.9f, 0.0f,   -0.4f, -0.9f, 0.0f,   -0.5f, -0.5f, 0.0f,
        -0.4f, -0.9f, 0.0f,   -0.4f, -0.5f, 0.0f,   -0.5f, -0.5f, 0.0f,
        // Diagonal izquierda
        -0.8f, -0.5f, 0.0f,   -0.75f, -0.5f, 0.0f,  -0.7f, -0.75f, 0.0f,
        -0.75f, -0.5f, 0.0f,  -0.65f, -0.75f, 0.0f, -0.7f, -0.75f, 0.0f,
        // Diagonal derecha
        -0.5f, -0.5f, 0.0f,   -0.45f, -0.5f, 0.0f,  -0.65f, -0.75f, 0.0f,
        -0.45f, -0.5f, 0.0f,  -0.6f, -0.75f, 0.0f,  -0.65f, -0.75f, 0.0f,

        // --- LETRA D (Centro) --- 24 vértices
        // Palo izquierdo
        -0.2f, -0.2f, 0.0f,   -0.1f, -0.2f, 0.0f,   -0.2f, 0.2f, 0.0f,
        -0.1f, -0.2f, 0.0f,   -0.1f, 0.2f, 0.0f,    -0.2f, 0.2f, 0.0f,
        // Barra superior
        -0.1f,  0.1f, 0.0f,    0.15f, 0.1f, 0.0f,   -0.1f, 0.2f, 0.0f,
         0.15f, 0.1f, 0.0f,    0.15f, 0.2f, 0.0f,   -0.1f, 0.2f, 0.0f,
         // Barra inferior
         -0.1f, -0.2f, 0.0f,    0.15f,-0.2f, 0.0f,   -0.1f,-0.1f, 0.0f,
          0.15f,-0.2f, 0.0f,    0.15f,-0.1f, 0.0f,   -0.1f,-0.1f, 0.0f,
          // Curva derecha
           0.15f,-0.2f, 0.0f,    0.25f,-0.2f, 0.0f,    0.15f, 0.2f, 0.0f,
           0.25f,-0.2f, 0.0f,    0.25f, 0.2f, 0.0f,    0.15f, 0.2f, 0.0f,

           // --- LETRA H (Arriba a la derecha) --- 18 vértices
           // Palo izquierdo
            0.4f, 0.4f, 0.0f,     0.5f, 0.4f, 0.0f,     0.4f, 0.8f, 0.0f,
            0.5f, 0.4f, 0.0f,     0.5f, 0.8f, 0.0f,     0.4f, 0.8f, 0.0f,
            // Palo derecho
             0.7f, 0.4f, 0.0f,     0.8f, 0.4f, 0.0f,     0.7f, 0.8f, 0.0f,
             0.8f, 0.4f, 0.0f,     0.8f, 0.8f, 0.0f,     0.7f, 0.8f, 0.0f,
             // Barra central
              0.5f, 0.55f, 0.0f,    0.7f, 0.55f, 0.0f,    0.5f, 0.65f, 0.0f,
              0.7f, 0.55f, 0.0f,    0.7f, 0.65f, 0.0f,    0.5f, 0.65f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
    GLuint theShader = glCreateShader(shaderType);
    const GLchar* theCode[1];
    theCode[0] = shaderCode;
    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);
    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);
    GLint result = 0;
    GLchar eLog[1024] = { 0 };
    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("El error al compilar el shader %d es: %s \n", shaderType, eLog);
        return;
    }
    glAttachShader(theProgram, theShader);
}

void CompileShaders() {
    shader = glCreateProgram();
    if (!shader)
    {
        printf("Error creando el shader");
        return;
    }
    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);
    GLint result = 0;
    GLchar eLog[1024] = { 0 };
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("El error al linkear es: %s \n", eLog);
        return;
    }
    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("El error al validar es: %s \n", eLog);
        return;
    }
}

int main()
{
    if (!glfwInit())
    {
        printf("Falló inicializar GLFW");
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "MDH - Letras y Fondo Random", NULL, NULL);

    if (!mainWindow)
    {
        printf("Fallo en crearse la ventana con GLFW");
        glfwTerminate();
        return 1;
    }

    int BufferWidth, BufferHeight;
    glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);
    glfwMakeContextCurrent(mainWindow);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("Falló inicialización de GLEW");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, BufferWidth, BufferHeight);

    CrearLetras();
    CompileShaders();

    // Semilla para asegurar que los colores aleatorios sean distintos en cada ejecución
    srand((unsigned int)time(NULL));

    // Variables de control para el cambio de color cada 2 segundos
    float lastTime = glfwGetTime();
    float r = 0.0f, g = 0.0f, b = 0.0f;

    while (!glfwWindowShouldClose(mainWindow))
    {
        glfwPollEvents();

        // Lógica del temporizador
        float currentTime = glfwGetTime();
        if (currentTime - lastTime >= 2.0f) {
            // Genera un nuevo color aleatorio entre 0.0 y 1.0
            r = (float)rand() / RAND_MAX;
            g = (float)rand() / RAND_MAX;
            b = (float)rand() / RAND_MAX;
            lastTime = currentTime; // Reinicia el contador
        }

        // Limpiar la ventana con el color actual
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        glBindVertexArray(VAO);
        // Dibujamos 66 vértices en total (24 para M + 24 para D + 18 para H)
        glDrawArrays(GL_TRIANGLES, 0, 66);
        glBindVertexArray(0);

        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    return 0;
}