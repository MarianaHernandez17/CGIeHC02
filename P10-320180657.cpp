/*
* Proyecto Final - CGeIHC
* Buendía López Sebastián - 320014932
* Hernández Pérez Mariana Daniela - 320180657
* Ortega Novoa Octavio - 317147768
*/

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "AudioManager.h"
#include "Camera.h"
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "InputManager.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include "Skybox.h"
#include "SpotLight.h"
#include "Texture.h"
#include "Window.h"
#include "GameObject.h"
#include "Train.h"
#include "Castle.h"

// Rutas de shaders
static const char* VERT_SHADER = "shaders/shader.vert";
static const char* FRAG_SHADER = "shaders/shader.frag";
static const char* SKYBOX_VERT = "shaders/skybox.vert";
static const char* SKYBOX_FRAG = "shaders/skybox.frag";

// Constantes para conversión de grados a radianes
const float DEG2RAD = 3.14159265f / 180.0f;

// Variables globales para la nave (keyframes)
float posXavion = 0.0f, posYavion = 1.5f, posZavion = 0.0f;
float movAvion_x = 0.0f;
float movAvion_y = 0.0f;
float giroAvion = 0.0f;

float wingFlapAngle = 0.0f;   
float wingFlapTime = 0.0f;  
const float WING_FLAP_SPEED = 3.0f;   
const float WING_FLAP_AMPLITUDE = 15.0f;  

float heliceAngle = 0.0f;
const float HELICE_SPEED = 360.0f; 

#define MAX_FRAMES 100
int i_max_steps = 90;
int i_curr_steps = 6;

typedef struct _frame {
    float movAvion_x, movAvion_y;
    float movAvion_xInc, movAvion_yInc;
    float giroAvion, giroAvionInc;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 11;    
int playIndex = 0;

int reproduciranimacion = 0, habilitaranimacion = 0;
int guardoFrame = 0, reinicioFrame = 0;
int ciclo = 0, ciclo2 = 0;

bool readyXNeg = true, readyYPos = true, readyYNeg = true, readyRot = true;
bool play = false;

// Geometría del piso
static GLfloat FLOOR_VERTS[] = {
    //  x      y      z      u      v      nx    ny    nz
       -1.0f,  0.0f, -1.0f,  0.0f, 10.0f,  0.0f, 1.0f, 0.0f,
        1.0f,  0.0f, -1.0f, 10.0f, 10.0f,  0.0f, 1.0f, 0.0f,
        1.0f,  0.0f,  1.0f, 10.0f,  0.0f,  0.0f, 1.0f, 0.0f,
       -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,
};
static GLuint FLOOR_IDX[] = { 0, 1, 2,  0, 2, 3 };

// Funciones para keyframes de la nave
void saveFrame()
{
    printf("guardando frame %d\n", FrameIndex);
    KeyFrame[FrameIndex].movAvion_x = movAvion_x;
    KeyFrame[FrameIndex].movAvion_y = movAvion_y;
    KeyFrame[FrameIndex].giroAvion = giroAvion;
    FrameIndex++;
}

void resetElements()
{
    movAvion_x = KeyFrame[0].movAvion_x;
    movAvion_y = KeyFrame[0].movAvion_y;
    giroAvion = KeyFrame[0].giroAvion;
}

void interpolation()
{
    KeyFrame[playIndex].movAvion_xInc =
        (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
    KeyFrame[playIndex].movAvion_yInc =
        (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
    KeyFrame[playIndex].giroAvionInc =
        (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;
}

void animate()
{
    if (play)
    {
        if (i_curr_steps >= i_max_steps)
        {
            playIndex++;
            if (playIndex > FrameIndex - 2)
            {
                printf("Animación terminada. Presiona 0 y luego ESPACIO para repetir.\n");
                playIndex = 0;
                play = false;
            }
            else
            {
                i_curr_steps = 0;
                interpolation();
            }
        }
        else
        {
            movAvion_x += KeyFrame[playIndex].movAvion_xInc;
            movAvion_y += KeyFrame[playIndex].movAvion_yInc;
            giroAvion += KeyFrame[playIndex].giroAvionInc;
            i_curr_steps++;
        }
    }
}

void handleShipInput(bool* keys)
{
    // Reproducción
    if (keys[GLFW_KEY_SPACE] && reproduciranimacion < 1)
    {
        if (!play && FrameIndex > 1)
        {
            resetElements();
            interpolation();
            play = true;
            playIndex = 0;
            i_curr_steps = 0;
            reproduciranimacion++;
            habilitaranimacion = 0;
            printf("Reproduciendo... presiona 0 para re-armar.\n");
        }
        else { play = false; }
    }

    if (keys[GLFW_KEY_0] && habilitaranimacion < 1)
    {
        reproduciranimacion = 0;
        habilitaranimacion = 1;
        printf("Listo. Presiona ESPACIO para reproducir.\n");
    }

    // Guardar frame manual
    if (keys[GLFW_KEY_L] && guardoFrame < 1)
    {
        saveFrame();
        guardoFrame = 1;
        reinicioFrame = 0;
        printf("Frame guardado. Presiona P para guardar otro.\n");
    }
    if (keys[GLFW_KEY_P] && reinicioFrame < 1)
    {
        guardoFrame = 0;
        reinicioFrame = 1;
        printf("Listo para guardar otro frame (presiona L).\n");
    }

    // Movimiento manual de la nave
    if (keys[GLFW_KEY_Q] && readyXNeg)
    {
        movAvion_x -= 1.0f; readyXNeg = false; printf("X: %.2f\n", movAvion_x);
    }
    if (keys[GLFW_KEY_E] && !readyXNeg)
    {
        readyXNeg = true;
    }

    if (keys[GLFW_KEY_R] && readyYPos)
    {
        movAvion_y += 1.0f; readyYPos = false; printf("Y: %.2f\n", movAvion_y);
    }
    if (keys[GLFW_KEY_F] && !readyYPos)
    {
        readyYPos = true;
    }

    if (keys[GLFW_KEY_T] && readyYNeg)
    {
        movAvion_y -= 1.0f; readyYNeg = false; printf("Y: %.2f\n", movAvion_y);
    }
    if (keys[GLFW_KEY_G] && !readyYNeg)
    {
        readyYNeg = true;
    }

    if (keys[GLFW_KEY_Y] && readyRot)
    {
        giroAvion += 180.0f;
        if (giroAvion >= 360.0f) giroAvion -= 360.0f;
        readyRot = false;
        printf("Rotación Y: %.2f\n", giroAvion);
    }
    if (keys[GLFW_KEY_H] && !readyRot)
    {
        readyRot = true;
    }

    if (keys[GLFW_KEY_1] && ciclo < 1)
    {
        movAvion_x += 1.0f;
        ciclo++;  ciclo2 = 0;
        printf("movAvion_x: %.2f  (presiona 2 para re-armar)\n", movAvion_x);
    }
    if (keys[GLFW_KEY_2] && ciclo2 < 1)
    {
        ciclo = 0;
        ciclo2++;
        printf("Listo, presiona 1 para mover.\n");
    }
}

// CreateAvatar
// Construir la jerarquía del avatar (Ruby)
std::shared_ptr<GameObject> CreateAvatar(
    Material& matOpaco,
    Model& rubyModel,
    Model& rubyLeftArm,
    Model& rubyRightArm,
    Model& rubyLeftLeg,
    Model& rubyRightLeg,
    Model& rubyCape,
    std::shared_ptr<GameObject>& leftArm,
    std::shared_ptr<GameObject>& rightArm,
    std::shared_ptr<GameObject>& leftLeg,
    std::shared_ptr<GameObject>& rightLeg,
    std::shared_ptr<GameObject>& cape)
{
    // Cuerpo raíz
    std::shared_ptr<GameObject> rubyObj = std::make_shared<GameObject>("Ruby", GameObjectType::MODEL);
    rubyObj->setModel(&rubyModel);
    rubyObj->setMaterial(&matOpaco);
    rubyObj->transform.setPosition(20.0f, 1.2f, 1.64f);
    rubyObj->transform.setScale(5.0f);

    // Extremidades — hijos de rubyObj
    leftArm = std::make_shared<GameObject>("LeftArm", GameObjectType::MODEL);
    leftArm->setModel(&rubyLeftArm);
    leftArm->transform.setPosition(0.025f, 0.0611f, 0.0f);
    leftArm->transform.setRotation(0.0f, 0.0f, -65.0f);
    rubyObj->addChild(leftArm);

    rightArm = std::make_shared<GameObject>("RightArm", GameObjectType::MODEL);
    rightArm->setModel(&rubyRightArm);
    rightArm->transform.setPosition(-0.025f, 0.0611f, 0.0f);
    rightArm->transform.setRotation(0.0f, 0.0f, 65.0f);
    rubyObj->addChild(rightArm);

    leftLeg = std::make_shared<GameObject>("LeftLeg", GameObjectType::MODEL);
    leftLeg->setModel(&rubyLeftLeg);
    rubyObj->addChild(leftLeg);

    rightLeg = std::make_shared<GameObject>("RightLeg", GameObjectType::MODEL);
    rightLeg->setModel(&rubyRightLeg);
    rubyObj->addChild(rightLeg);

    cape = std::make_shared<GameObject>("Cape", GameObjectType::MODEL);
    cape->setModel(&rubyCape);
    cape->transform.setPosition(0.0f, 0.065f, -0.01f);
    rubyObj->addChild(cape);

    return rubyObj;
}

// ─────────────────────────────────────────────────────────────────────────────
int main()
{
    // Creación de la ventana
    Window mainWindow("Proyecto Final - CGeIHC");
    if (mainWindow.Initialize() != 0) return -1;

    // Instancia de audio
    AudioManager& audioManager = AudioManager::getInstance();
    if (!audioManager.Initialize()) return -1;

    // Reproducir música
    audioManager.loadMP3("bgMusic", "Sounds/bgMusic.mp3");
    audioManager.play("bgMusic", true, 0.5f);

    // Configuración inicial de la cámara
    Camera camera(glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.1f);
    camera.setCameraMode(CameraMode::THIRD_PERSON);

    // Shader
    Shader shader;
    shader.createFromFiles(VERT_SHADER, FRAG_SHADER);

	// Creación del skybox con texturas de día y noche (ambas con Space)
	Skybox skybox;
	skybox.create(
		// Texturas de día (Space Nebula Blue)
		// Orden: RIGHT, LEFT, UP, DOWN, FRONT, BACK
		{
			"Textures/Skybox/jettelly_space_nebulas_blue_LEFT.png",
			"Textures/Skybox/jettelly_space_nebulas_blue_RIGHT.png",
			"Textures/Skybox/jettelly_space_nebulas_blue_UP.png",
			"Textures/Skybox/jettelly_space_nebulas_blue_DOWN.png",
			"Textures/Skybox/jettelly_space_nebulas_blue_FRONT.png",
			"Textures/Skybox/jettelly_space_nebulas_blue_BACK.png"
		},
		// Texturas de noche (Space Nebula Blue)
		// Orden: RIGHT, LEFT, UP, DOWN, FRONT, BACK
		{
			"Textures/Skybox/jettelly_space_nebulas_black_LEFT.png",
			"Textures/Skybox/jettelly_space_nebulas_black_RIGHT.png",
			"Textures/Skybox/jettelly_space_nebulas_black_UP.png",
			"Textures/Skybox/jettelly_space_nebulas_black_DOWN.png",
			"Textures/Skybox/jettelly_space_nebulas_black_FRONT.png",
			"Textures/Skybox/jettelly_space_nebulas_black_BACK.png"
		},
		SKYBOX_VERT, SKYBOX_FRAG
	);

	// Configurar duración del ciclo día-noche (30 segundos para el ciclo completo)
	skybox.setDayNightCycleDuration(30.0f);

    // Texturas y materiales
    Texture floorTexture("Textures/naka_yuka_01_52D00-DXT1.png");
    floorTexture.loadWithAlpha();
    Material matOpaco(0.2f, 4.0f);
    Material matBrillante(1.0f, 32.0f);

    // Modelos

    // Ruby — partes del avatar
    Model rubyModel;    if (!rubyModel.load("Models/RubyCuerpo.obj"))      return -1;
    Model rubyLeftArm;  if (!rubyLeftArm.load("Models/RubyBrazoIzq.obj"))  return -1;
    Model rubyRightArm; if (!rubyRightArm.load("Models/RubyBrazoDer.obj")) return -1;
    Model rubyLeftLeg;  if (!rubyLeftLeg.load("Models/RubyPiernaIzq.obj")) return -1;
    Model rubyRightLeg; if (!rubyRightLeg.load("Models/RubyPiernaDer.obj")) return -1;
    Model rubyCape;     if (!rubyCape.load("Models/RubyCapa.obj"))         return -1;

    // Castillo de Hyrule — partes del escenario
    Model mainRoom;  if (!mainRoom.load("Models/HyruleCastle_MainRoom.obj")) return -1;
    Model bigTower;  if (!bigTower.load("Models/HyruleCastle_BigTower.obj")) return -1;
    Model wall;      if (!wall.load("Models/HyruleCastle_Wall.obj"))         return -1;
    Model midTower;  if (!midTower.load("Models/HyruleCastle_MidTower.obj")) return -1;
    Model floor;     if (!floor.load("Models/HyruleCastle_Floor.obj"))       return -1;

    // Nave — partes del modelo (keyframes)
    Model Ship_M;
    Model Wing_M;       
    Model Wing2_M;      
    Model Helice_M;     
    Model Helice2_M;

    if (!Ship_M.load("Models/nave.obj")) return -1;
    if (!Wing_M.load("Models/ala.obj")) return -1;
    if (!Helice_M.load("Models/helice1.obj")) return -1;
    if (!Helice2_M.load("Models/helice2.obj")) return -1;

    // Texturas de la nave (keyframes)
    Texture brickTexture, dirtTexture, plainTexture, floorTexture2;

    brickTexture = Texture("Textures/brick.png");        
    brickTexture.loadWithAlpha();
    dirtTexture = Texture("Textures/dirt.png");         
    dirtTexture.loadWithAlpha();
    plainTexture = Texture("Textures/plain.png");        
    plainTexture.loadWithAlpha();
    floorTexture2 = Texture("Textures/piso.tga");         
    floorTexture2.loadWithAlpha();

    // Materiales de la nave (keyframes)
    Material shinyMat(1.0f, 256);
    Material dullMat(0.3f, 4);

    // Inicializar keyframes
    const float H = 3.0f;  
    const float S = 3.0f;   

    KeyFrame[0] = { 0 * S,  0,     0 };    
    KeyFrame[1] = { 1 * S,  H,     0 };   
    KeyFrame[2] = { 2 * S,  0,     0 };    
    KeyFrame[3] = { 3 * S, -H,     0 };    
    KeyFrame[4] = { 4 * S,  H,     0 };    
    KeyFrame[5] = { 5 * S,  0,    45 };    
    KeyFrame[6] = { 5 * S + 1,-H,   90 };    
    KeyFrame[7] = { 5 * S,  0,   180 };   
    KeyFrame[8] = { 4 * S,  H,   270 };    
    KeyFrame[9] = { 3 * S,  0,   360 };    
    KeyFrame[10] = { 0 * S,  0,   360 };    

    interpolation();

    printf("\n=== Controles de Keyframes ===\n");
    printf("ESPACIO  — reproducir animación\n");
    printf("0        — re-armar reproducción\n");
    printf("L        — guardar frame manual\n");
    printf("P        — habilitar guardar otro frame\n");
    printf("Q/E      — mover nave -X / re-armar\n");
    printf("R/F      — mover nave +Y / re-armar\n");
    printf("T/G      — mover nave -Y / re-armar\n");
    printf("Y/H      — rotar nave 180° / re-armar\n");
    printf("1/2      — mover nave +X / re-armar\n");
    printf("==============================\n\n");

    // GameObjects de la escena

    // Piso de la escena
    std::shared_ptr<GameObject> floorObj = std::make_shared<GameObject>("Floor", GameObjectType::MESH);
    MeshData floorData;
    floorData.vertices = std::vector<GLfloat>(std::begin(FLOOR_VERTS), std::end(FLOOR_VERTS));
    floorData.indices = std::vector<GLuint>(std::begin(FLOOR_IDX), std::end(FLOOR_IDX));
    floorObj->loadMesh(floorData);
    floorObj->setTextureID(floorTexture.getID());
    floorObj->setMaterial(&matOpaco);
    floorObj->transform.setScale(50.0f, 1.0f, 50.0f);

    Castle castle;
    if (!castle.Initialize(matOpaco)) return -1;

    Train train;
    if (!train.Initialize(matOpaco)) return -1;

    float trainSpeed = 5.0f;
    float wheelRotationSpeed = 200.0f;

    // Avatar: Ruby

    // Variables para las extremidades
    std::shared_ptr<GameObject> leftArm;
    std::shared_ptr<GameObject> rightArm;
    std::shared_ptr<GameObject> leftLeg;
    std::shared_ptr<GameObject> rightLeg;
    std::shared_ptr<GameObject> cape;

    // Creación del avatar
    std::shared_ptr<GameObject> rubyObj = CreateAvatar(matOpaco, rubyModel, rubyLeftArm, rubyRightArm, rubyLeftLeg, rubyRightLeg, rubyCape, leftArm, rightArm, leftLeg, rightLeg, cape);

    // Estado de la animación de caminata
    float walkTime = 0.0f;
    float walkSpeed = 10.0f;
    float walkAmplitude = 35.0f;
    float angleThigh = 0.0f;

    // Luces

    // Luz direccional
    DirectionalLight directionalLight(
        1.0f, 0.95f, 0.8f,
        0.3f, 0.8f,
        0.0f, -1.0f, -0.5f
    );

    // Punto de Interés 1: Torre Grande Central
    // Enfoca una de las torres principales del castillo desde una vista dinámica
    glm::vec3 bigTower1Pos(16.84f, 8.92f, -9.0f);
    camera.addInterestPoint(
        bigTower1Pos + glm::vec3(15.0f, 8.0f, 15.0f),
        bigTower1Pos + glm::vec3(0.0f, 5.0f, 0.0f) 
    );

    // Punto de Interés 2: Entrada del Castillo (Vista General)
    // Muestra la entrada y la estructura general del castillo
    glm::vec3 castleEntrancePos(20.0f, 5.0f, -8.0f);
    camera.addInterestPoint(
        castleEntrancePos + glm::vec3(-20.0f, 12.0f, 25.0f),
        castleEntrancePos
    );

    // Punto de Interés 3: Torres de Defensa Laterales
    // Enfoca las torres medias de las esquinas del castillo
    glm::vec3 lateralTowerPos(36.09f, 5.44f, -3.64f);
    camera.addInterestPoint(
        lateralTowerPos + glm::vec3(18.0f, 10.0f, 18.0f),
        lateralTowerPos + glm::vec3(0.0f, 8.0f, 0.0f)
    );

    // Proyección
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)mainWindow.getBufferWidth() / (float)mainWindow.getBufferHeight(), 0.1f, 500.0f);

    // Matriz de modelo
    glm::mat4 model(1.0f);
    GLfloat lastTime = (GLfloat)glfwGetTime();

    // Bucle principal
    while (!mainWindow.shouldClose())
    {
        // Delta Time
        GLfloat now = (GLfloat)glfwGetTime();
        GLfloat deltaTime = now - lastTime;
        lastTime = now;

        // Actualizar ciclo día-noche del skybox
        skybox.updateDayNightCycle(deltaTime);

        // Input
        glfwPollEvents();
        InputManager& input = InputManager::getInstance();
        input.beginFrame();

        // Cambio de modo de cámara (C, V, B)
        if (input.isKeyDown(GLFW_KEY_C))
        {
            camera.setCameraMode(CameraMode::THIRD_PERSON);
        }

        if (input.isKeyDown(GLFW_KEY_V))
        {
            camera.setCameraMode(CameraMode::AERIAL);
        }

        if (input.isKeyDown(GLFW_KEY_B))
        {
            camera.setCameraMode(CameraMode::INTEREST_POINT);
        }

        // Manejo de input de la nave (keyframes)
        // Convertir InputManager a arreglo de booleanos para compatibilidad con handleShipInput
        bool shipKeys[1024] = {false};
        // Mapear las teclas usadas en handleShipInput
        if (input.isKeyDown(GLFW_KEY_SPACE)) shipKeys[GLFW_KEY_SPACE] = true;
        if (input.isKeyDown(GLFW_KEY_0)) shipKeys[GLFW_KEY_0] = true;
        if (input.isKeyDown(GLFW_KEY_L)) shipKeys[GLFW_KEY_L] = true;
        if (input.isKeyDown(GLFW_KEY_P)) shipKeys[GLFW_KEY_P] = true;
        if (input.isKeyDown(GLFW_KEY_Q)) shipKeys[GLFW_KEY_Q] = true;
        if (input.isKeyDown(GLFW_KEY_E)) shipKeys[GLFW_KEY_E] = true;
        if (input.isKeyDown(GLFW_KEY_R)) shipKeys[GLFW_KEY_R] = true;
        if (input.isKeyDown(GLFW_KEY_F)) shipKeys[GLFW_KEY_F] = true;
        if (input.isKeyDown(GLFW_KEY_T)) shipKeys[GLFW_KEY_T] = true;
        if (input.isKeyDown(GLFW_KEY_G)) shipKeys[GLFW_KEY_G] = true;
        if (input.isKeyDown(GLFW_KEY_Y)) shipKeys[GLFW_KEY_Y] = true;
        if (input.isKeyDown(GLFW_KEY_H)) shipKeys[GLFW_KEY_H] = true;
        if (input.isKeyDown(GLFW_KEY_1)) shipKeys[GLFW_KEY_1] = true;
        if (input.isKeyDown(GLFW_KEY_2)) shipKeys[GLFW_KEY_2] = true;
        handleShipInput(shipKeys);

        // Avanzar al siguiente punto de interés con SPACE
        if (input.isKeyDown(GLFW_KEY_SPACE) && camera.getCameraMode() == CameraMode::INTEREST_POINT)
            camera.nextInterestPoint();

        // Actualizar cámara según el modo actual
        glm::vec3 rubyPos = rubyObj->transform.getPosition();

        // Destino de tercera persona (usado como target en transiciones hacia/desde ese modo)
        glm::vec3 camDir3P = camera.getDirection();
        glm::vec3 thirdCamPos = rubyPos - glm::normalize(glm::vec3(camDir3P.x, 0.0f, camDir3P.z)) * 2.5f;
        thirdCamPos.y = rubyPos.y + 1.0f;
        glm::vec3 thirdLookAt = rubyPos + glm::vec3(0.0f, 1.0f, 0.0f);

        // Destino aéreo: posición sobre el centro del mapa a aerialHeight
        glm::vec3 aerialTarget(0.0f, 15.0f, 0.0f);
        glm::vec3 aerialLookAt(0.0f, 0.0f, 0.0f);

        switch (camera.getCameraMode())
        {
        case CameraMode::THIRD_PERSON:
            // Durante transición hacia tercera persona, proveer el destino
            if (camera.isInModeTransition())
            {
                camera.setTransitionTarget(thirdCamPos, thirdLookAt);
                camera.updateInterestPointCamera(deltaTime);
                break;
            }
            // Control normal
            if (input.isKeyDown(GLFW_KEY_Q))
                camera.rotateOrbit(90.0f * deltaTime);
            if (input.isKeyDown(GLFW_KEY_E))
                camera.rotateOrbit(-90.0f * deltaTime);
            camera.mouseControl(input.getMouseDeltaX() * 0.5f, input.getMouseDeltaY() * 0.5f);
            camera.updateThirdPersonCamera(rubyPos, deltaTime);
            break;

        case CameraMode::AERIAL:
            // Durante transición hacia aéreo, proveer el destino
            if (camera.isInModeTransition())
            {
                camera.setTransitionTarget(aerialTarget, aerialLookAt);
                camera.updateInterestPointCamera(deltaTime);
                break;
            }
            // Control normal
            camera.updateAerialCamera(input, deltaTime);
            break;

        case CameraMode::INTEREST_POINT:
        {
            // Proveer destino de salida hacia tercera persona
            camera.setTransitionTarget(thirdCamPos, thirdLookAt);
            camera.updateInterestPointCamera(deltaTime);
            break;
        }
        }

        // Movimiento del tren
        train.Update(trainSpeed, deltaTime, wheelRotationSpeed);

        // Animación de la nave (wingFlap, helice, keyframes)
        wingFlapTime += deltaTime;
        wingFlapAngle = WING_FLAP_AMPLITUDE * sinf(WING_FLAP_SPEED * wingFlapTime);

        heliceAngle += HELICE_SPEED * deltaTime;
        if (heliceAngle >= 360.0f) heliceAngle -= 360.0f;

        animate();

        // Movimiento de Ruby — solo en modo THIRD_PERSON y sin transición activa
        float rubyMoveSpeed = 8.0f;
        glm::vec3 cameraDirection = camera.getDirection();
        glm::vec3 moveDirection = glm::normalize(glm::vec3(cameraDirection.x, 0.0f, cameraDirection.z));
        glm::vec3 rightDirection = glm::normalize(glm::cross(moveDirection, glm::vec3(0.0f, 1.0f, 0.0f)));

        bool isWalking = false;

        if (camera.getCameraMode() == CameraMode::THIRD_PERSON && !camera.isInModeTransition())
        {
            if (input.isKeyDown(GLFW_KEY_W))
            {
                glm::vec3 movement = moveDirection * rubyMoveSpeed * deltaTime;
                rubyObj->transform.translate(movement.x, 0.0f, movement.z);
                isWalking = true;
            }
            if (input.isKeyDown(GLFW_KEY_S))
            {
                glm::vec3 movement = moveDirection * rubyMoveSpeed * deltaTime;
                rubyObj->transform.translate(-movement.x, 0.0f, -movement.z);
                isWalking = true;
            }
            if (input.isKeyDown(GLFW_KEY_A))
            {
                glm::vec3 movement = rightDirection * rubyMoveSpeed * deltaTime;
                rubyObj->transform.translate(-movement.x, 0.0f, -movement.z);
                isWalking = true;
            }
            if (input.isKeyDown(GLFW_KEY_D))
            {
                glm::vec3 movement = rightDirection * rubyMoveSpeed * deltaTime;
                rubyObj->transform.translate(movement.x, 0.0f, movement.z);
                isWalking = true;
            }
        }

        // Animación de caminata de Ruby
        if (isWalking)
        {
            walkTime += deltaTime * walkSpeed;
            angleThigh = walkAmplitude * sin(walkTime);
        }
        else
        {
            walkTime = 0.0f;
            angleThigh += (0.0f - angleThigh) * 8.0f * deltaTime;
        }

        leftArm->transform.setRotation(angleThigh, 0.0f, -65.0f);
        rightArm->transform.setRotation(-angleThigh, 0.0f, 65.0f);
        leftLeg->transform.setRotation(-angleThigh, 0.0f, 0.0f);
        rightLeg->transform.setRotation(angleThigh, 0.0f, 0.0f);
        cape->transform.setRotation((-angleThigh * 0.5f) + 45.0f, 0.0f, 0.0f);

        // Orientar a Ruby hacia donde mira la cámara — solo en modo THIRD_PERSON sin transición
        if (camera.getCameraMode() == CameraMode::THIRD_PERSON && !camera.isInModeTransition())
        {
            glm::vec3 rubyDirection = glm::normalize(glm::vec3(cameraDirection.x, 0.0f, cameraDirection.z));
            float angleY = atan2(rubyDirection.x, rubyDirection.z);
            rubyObj->transform.setRotation(0.0f, glm::degrees(angleY), 0.0f);
        }

        // Luces

        // Luz direccional
        float timeProgress = skybox.getTimeProgress();
        float dayIntensity, nightIntensity;

        if (timeProgress < 0.5f)
        {
            // Primera mitad del ciclo: día -> noche
            float t = timeProgress * 2.0f;
            dayIntensity = 1.0f - t;
            nightIntensity = t;
        }
        else
        {
            // Segunda mitad del ciclo: noche -> día
            float t = (timeProgress - 0.5f) * 2.0f;
            dayIntensity = t;
            nightIntensity = 1.0f - t;
        }

        glm::vec3 dayColor(1.0f, 0.95f, 0.8f);   // amarillo cálido
        glm::vec3 nightColor(0.4f, 0.4f, 0.6f);  // azul oscuro
        glm::vec3 currentColor = glm::mix(nightColor, dayColor, dayIntensity);

        directionalLight.setColor(currentColor);
        directionalLight.setAmbientIntensity(0.3f * (0.5f + dayIntensity * 0.5f));
        directionalLight.setDiffuseIntensity(0.8f * dayIntensity);

        // Renderizado
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Skybox
        skybox.draw(camera.calculateViewMatrix(), projection);

        // Configurar shader principal
        shader.use();

        glm::mat4 view = camera.calculateViewMatrix();
        glUniformMatrix4fv(shader.getProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(shader.getViewLocation(), 1, GL_FALSE, glm::value_ptr(view));
        glm::vec3 eye = camera.getPosition();
        glUniform3f(shader.getEyePositionLocation(), eye.x, eye.y, eye.z);
        glUniform3f(shader.getColorLocation(), 1.0f, 1.0f, 1.0f);
        glUniform2f(shader.getTextureOffsetLocation(), 0.0f, 0.0f);

        shader.setDirectionalLight(&directionalLight);

		// Renderizar objetos
		floorObj->draw(shader);

		
        // Objetos de la escena
        floorObj->draw(shader);
        
        // Render del castillo
        castle.GetCastleObject()->draw(shader);

        // Render del tren
        train.GetTrainObject()->draw(shader);

        // Nave y animación por keyframes
        glm::vec3 posNave(posXavion + movAvion_x, posYavion + movAvion_y, posZavion);
        glm::mat4 matNave = glm::mat4(1.0f);
        matNave = glm::translate(matNave, posNave);
        matNave = glm::rotate(matNave, giroAvion * DEG2RAD, glm::vec3(0, 1, 0));

        // Nave principal
        glUniformMatrix4fv(shader.getModelLocation(), 1, GL_FALSE, glm::value_ptr(matNave));
        shinyMat.use(shader.getSpecularIntensityLocation(), shader.getShininessLocation());
        Ship_M.render();

        // Ala izquierda
        glm::mat4 matAlaIzq = matNave;
        matAlaIzq = glm::translate(matAlaIzq, glm::vec3(0.0f, 0.0f, -0.25f));
        matAlaIzq = glm::rotate(matAlaIzq, wingFlapAngle * DEG2RAD, glm::vec3(0, 1, 0));
        glUniformMatrix4fv(shader.getModelLocation(), 1, GL_FALSE, glm::value_ptr(matAlaIzq));
        shinyMat.use(shader.getSpecularIntensityLocation(), shader.getShininessLocation());
        Wing_M.render();

        // Ala derecha
        glm::mat4 matAlaDer = matNave;
        matAlaDer = glm::translate(matAlaDer, glm::vec3(0.0f, 0.0f, 0.0f));
        matAlaDer = glm::rotate(matAlaDer, 180.0f * DEG2RAD, glm::vec3(0, 1, 0));
        matAlaDer = glm::rotate(matAlaDer, -wingFlapAngle * DEG2RAD, glm::vec3(0, 1, 0));
        matAlaDer = glm::scale(matAlaDer, glm::vec3(-1, 1, 1));
        glUniformMatrix4fv(shader.getModelLocation(), 1, GL_FALSE, glm::value_ptr(matAlaDer));
        shinyMat.use(shader.getSpecularIntensityLocation(), shader.getShininessLocation());
        Wing_M.render();

        // Hélice 1
        glm::vec3 offsetHelice(-0.4f, -0.3f, -0.35f);
        glm::mat4 matHelice = matNave;
        matHelice = glm::translate(matHelice, offsetHelice);
        matHelice = glm::rotate(matHelice, heliceAngle * DEG2RAD, glm::vec3(1, 0, 0));
        glUniformMatrix4fv(shader.getModelLocation(), 1, GL_FALSE, glm::value_ptr(matHelice));
        shinyMat.use(shader.getSpecularIntensityLocation(), shader.getShininessLocation());
        Helice_M.render();

        // Hélice 2
        glm::vec3 offsetHelice2(-0.4f, -0.3f, 0.35f);
        matHelice = matNave;
        matHelice = glm::translate(matHelice, offsetHelice2);
        matHelice = glm::rotate(matHelice, heliceAngle * DEG2RAD, glm::vec3(1, 0, 0));
        glUniformMatrix4fv(shader.getModelLocation(), 1, GL_FALSE, glm::value_ptr(matHelice));
        shinyMat.use(shader.getSpecularIntensityLocation(), shader.getShininessLocation());
        Helice2_M.render();

        // Ruby
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        rubyObj->draw(shader);
        glDisable(GL_BLEND);

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    audioManager.shutdown();
    return 0;
}