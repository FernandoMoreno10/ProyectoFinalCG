#include <iostream>
#include <cmath>
#include <string>
#include <windows.h>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void animacion();
void animacionpelota();
void animacionbandera();
void animaciontecho();
void animacionpuerta();
void animacionanotacion();


// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 3.0f, 0.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 90.0f;


//Animacion pelota
float movBallX = 0.0;
float movBallY = 0.0;
float rotKit = 90.0;

bool ball_on = false;
bool recorrido1balon = true;
bool recorrido2balon = false;
bool recorrido3balon = false;

//Animacion banderas
float SubirBanderas = 0.0;
bool bandera_enable = false;
bool arriba_bandera = true;
bool abajo_bandera = false;

//Animacion techo
float Vtecho = 0.0;
bool techo_enable = false;
bool abrir_techo = true;
bool cerrar_techo = false;

//Animacion puertas
float MovPuerta = 0;
bool puerta_enable = false;
bool arriba_puerta = false;
bool abajo_puerta = true;



//Animacion anotacion
float scTD = 0;
float tTD = 0;
float scDW = 0;
float tDW = 0;
bool td_enable = false;
bool mov1 = true;
bool mov2 = false;
bool mov3 = false;




// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(60.0f, 1.0f, 1.0f);
bool active;


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
float posX =PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotRodIzq = 0, rotRodDer = 0, rotCodoDer=0, rotCodoIzq=0;

#define MAX_FRAMES 10
int i_max_steps = 190;
int i_curr_steps = 0; 
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ
	float rotRodIzq;
	float rotRodDer;
	float rotCodoDer;
	float rotCodoIzq;
	float rotInc;
	float rotInc2;
	float rotInc3;
	float rotInc4;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

//// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(posX,posY,posZ),
	glm::vec3(140.0,40.0,0),
	glm::vec3(140.0,-40.0,0),
	glm::vec3(0,10,0),
};

glm::vec3 LightP1;




void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);
	
	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;
	
	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].rotRodDer = rotRodDer;
	KeyFrame[FrameIndex].rotCodoDer = rotCodoDer;
	KeyFrame[FrameIndex].rotCodoIzq = rotCodoIzq;
	
	printf("%f %f %f %f %f %f",posX,posY,posZ,rotRodIzq,rotRodDer,rotCodoDer);

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	rotRodDer = KeyFrame[0].rotRodDer;
	rotCodoDer = KeyFrame[0].rotCodoDer;
	rotCodoIzq = KeyFrame[0].rotCodoIzq;
}

void interpolation(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;
	
	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	KeyFrame[playIndex].rotInc2 = (KeyFrame[playIndex + 1].rotRodDer - KeyFrame[playIndex].rotRodDer) / i_max_steps;
	KeyFrame[playIndex].rotInc3 = (KeyFrame[playIndex + 1].rotCodoDer - KeyFrame[playIndex].rotCodoDer) / i_max_steps;
	KeyFrame[playIndex].rotInc4 = (KeyFrame[playIndex + 1].rotCodoIzq - KeyFrame[playIndex].rotCodoIzq) / i_max_steps;
}




int main()
{

	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/
bool played = PlaySound(TEXT("miedo.wav"), NULL, SND_ASYNC);
cout << "Success or Not:" << played << endl;
	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 11", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");
	Model BotaDer((char*)"Models/Personaje/bota.obj");
	Model PiernaDer((char*)"Models/Personaje/piernader.obj");
	Model PiernaIzq((char*)"Models/Personaje/piernaizq.obj");
	Model Torso((char*)"Models/Personaje/torso.obj");
	Model BrazoDer((char*)"Models/Personaje/brazoder.obj");
	Model BrazoIzq((char*)"Models/Personaje/brazoizq.obj");
	Model Cabeza((char*)"Models/Personaje/cabeza.obj");
	Model Football((char*)"Models/Football/football.obj");	
	Model Estadio((char*)"Models/stadium/estadio.obj");
	Model Goal((char*)"Models/Goal/Goal.obj");
	Model Letras((char*)"Models/Letras/Letras.obj");
	Model Flag((char*)"Models/Flag/flag.obj");
	Model Board((char*)"Models/Board/board.obj");
	Model Poste((char*)"Models/Pole/poste.obj");
	Model Roof((char*)"Models/stadium/roof.obj");
	Model Puertas((char*)"Models/stadium/rejas.obj");
	Model Touch((char*)"Models/Board/td.obj");
	Model Down((char*)"Models/Board/td2.obj");
	Model Banca((char*)"Models/bench/bench2.obj");
	// Build and compile our shader program

	//Inicialización de KeyFrames

	for(int i=0; i<MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].rotRodDer = 0;
		KeyFrame[i].rotCodoDer = 0;
		KeyFrame[i].rotCodoIzq = 0;
		KeyFrame[i].rotInc = 0;
		KeyFrame[i].rotInc2 = 0;
		KeyFrame[i].rotInc3 = 0;
		KeyFrame[i].rotInc4 = 0;
	}

	KeyFrame[0].posX = 60.0;
	KeyFrame[0].posY = 1.0;
	KeyFrame[0].posZ = 0.0;
	KeyFrame[0].rotRodIzq = 0.0;
	KeyFrame[0].rotRodDer = 0.0;
	KeyFrame[0].rotCodoDer = 0.0;
	KeyFrame[0].rotCodoIzq = 0.0;
	
	KeyFrame[1].posX = 60.0;
	KeyFrame[1].posY = 1.0;
	KeyFrame[1].posZ = 0.0;
	KeyFrame[1].rotRodIzq = 80.0 ;
	KeyFrame[1].rotRodDer = -45.0;
	KeyFrame[1].rotCodoDer = -45.0;
	KeyFrame[1].rotCodoIzq = 45.0;

	KeyFrame[2].posX = 65.3999;
	KeyFrame[2].posY = 1.0;
	KeyFrame[2].posZ = 0.0999;
	KeyFrame[2].rotRodIzq = -45.0;
	KeyFrame[2].rotRodDer = 80.0;
	KeyFrame[2].rotCodoDer = 80.0;
	KeyFrame[2].rotCodoIzq = -80.f;

	KeyFrame[3].posX = 68.7998;
	KeyFrame[3].posY = 1.0;
	KeyFrame[3].posZ = 0.0999;
	KeyFrame[3].rotRodIzq = -45.0;
	KeyFrame[3].rotRodDer = -10.0;
	KeyFrame[3].rotCodoDer = -39.0;
	KeyFrame[3].rotCodoIzq = 0.0f;


	KeyFrame[4].posX = 68.7998;
	KeyFrame[4].posY = 1.0;
	KeyFrame[4].posZ = 0.0999;
	KeyFrame[4].rotRodIzq = 47.0;
	KeyFrame[4].rotRodDer = -8.0;
	KeyFrame[4].rotCodoDer = 80.0;
	KeyFrame[4].rotCodoIzq = -80.0;

	KeyFrame[5].posX = 69.1998;
	KeyFrame[5].posY = 1.0;
	KeyFrame[5].posZ = 0.0999;
	KeyFrame[5].rotRodIzq = 47.0;
	KeyFrame[5].rotRodDer = 80.0;
	KeyFrame[5].rotCodoDer = 80.0;
	KeyFrame[5].rotCodoIzq = -80.0;

	FrameIndex = 5;

	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};






	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1,&skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.tga");
	faces.push_back("SkyBox/left.tga");
	faces.push_back("SkyBox/top.tga");
	faces.push_back("SkyBox/bottom.tga");
	faces.push_back("SkyBox/back.tga");
	faces.push_back("SkyBox/front.tga");
	
	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();
		animacionpelota();
		animacionbandera();
		animaciontecho();
		animacionpuerta();
		animacionanotacion();
		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);


		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), LightP1.x, LightP1.y, LightP1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), LightP1.x, LightP1.y, LightP1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);



		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 1.0f, 0.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 0.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		
		glm::mat4 tmp = glm::mat4(1.0f); //Temp
		glm::mat4 tmp2 = glm::mat4(1.0f);
		glm::mat4 tmp3 = glm::mat4(1.0f);



		//Carga de modelo 
		//Personaje
		view = camera.GetViewMatrix();
		glm::mat4 model(1);
		tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		model = glm::translate(model,glm::vec3(posX,posY,posZ));
		model = glm::rotate(model, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Torso.Draw(lightingShader);

		//Pierna Izq
		tmp2 = model = glm::translate(model, glm::vec3(.75, 0, 0));
		tmp3 = model = glm::translate(model, glm::vec3(-1.5, 0, 0));
		view = camera.GetViewMatrix();
		model = glm::translate(tmp, glm::vec3(-0.5f, 0.0f, -0.1f));
		model = glm::translate(model, glm::vec3(posX, posY, posZ));
		model = glm::rotate(model, glm::radians(-rotRodIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(tmp2, glm::radians(-rotRodIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PiernaDer.Draw(lightingShader);

		//Pie Izq
		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		BotaDer.Draw(lightingShader);

		//Pierna Der
		view = camera.GetViewMatrix();
		model = glm::translate(tmp, glm::vec3(0.5f, 0.0f, -0.1f));
		model = glm::translate(model, glm::vec3(posX, posY, posZ));
		model = glm::rotate(model, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(tmp3, glm::radians(-rotRodDer), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PiernaIzq.Draw(lightingShader);
		//Pie Der
		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		BotaDer.Draw(lightingShader);

		//Brazo derecho
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(posX, posY, posZ));
		model = glm::rotate(model, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.75f, 2.5f, 0));
		model = glm::rotate(model, glm::radians(-rotCodoDer), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		BrazoDer.Draw(lightingShader);

		//Brazo Izquierdo
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(posX, posY, posZ));
		model = glm::rotate(model, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.75f, 2.5f, 0));
		model = glm::rotate(model, glm::radians(-rotCodoIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		BrazoIzq.Draw(lightingShader);

		//Cabeza
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(posX, posY, posZ));
		model = glm::rotate(model, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cabeza.Draw(lightingShader);


		//Ball
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::translate(model, glm::vec3(movBallX, movBallY, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Football.Draw(lightingShader);

		//Stadium
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Estadio.Draw(lightingShader);


		//Goal
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Goal.Draw(lightingShader);

		//Letras
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Letras.Draw(lightingShader);


		//Flag1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(30.0f, 0.0f, -140.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::translate(model, glm::vec3(-30.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.4f+ SubirBanderas, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Flag.Draw(lightingShader);

	
		//Flag5
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(90.0f, 0.0f, -140.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::translate(model, glm::vec3(-10.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.4f+SubirBanderas, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Flag.Draw(lightingShader);

		//Poste1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::translate(model, glm::vec3(30.0f, 0.0f, 140.0f));
		model = glm::scale(model, glm::vec3(1.65f, 1.25f, 1.0f));
		model = glm::translate(model, glm::vec3(21.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Poste.Draw(lightingShader);

		//Poste5
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::translate(model, glm::vec3(30.0f, 0.0f, 140.0f));
		model = glm::scale(model, glm::vec3(1.65f, 1.25f, 1.0f));
		model = glm::translate(model, glm::vec3(-51.5f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Poste.Draw(lightingShader);

		//Touch
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(141.00f+tTD, 10.0f, -58.0f));
		model = glm::scale(model, glm::vec3(0.5f+scTD, 1.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Touch.Draw(lightingShader);

		//Down
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(141.00f + tDW, 10.0f, -58.0f));
		model = glm::scale(model, glm::vec3(0.5f + scDW, 1.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Down.Draw(lightingShader);


		//Board
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Board.Draw(lightingShader);

		//Roof
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(1.0f+Vtecho, 1.0f, 1.0f+Vtecho));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Roof.Draw(lightingShader);

		//Puertas
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f,0.0f+MovPuerta,0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Puertas.Draw(lightingShader);

		//Bench
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Banca.Draw(lightingShader);

		//Bench
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-20.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Banca.Draw(lightingShader);

		//Bench
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(20.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Banca.Draw(lightingShader);


		glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Draw the light object (using light's vertex attributes)
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);


		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();




	return 0;
}


void animacion()
{
		//Movimiento del personaje

		if (play)
		{
			if (i_curr_steps >= i_max_steps) //end of animation between frames?
			{
				playIndex++;
				if (playIndex>FrameIndex - 2)	//end of total animation?
				{
					printf("termina anim\n");
					playIndex = 0;
					play = false;
					ball_on = true;
				}
				else //Next frame interpolations
				{
					i_curr_steps = 0; //Reset counter
									  //Interpolation
					interpolation();
				}
			}
			else
			{
				//Draw animation
				posX += KeyFrame[playIndex].incX;
				posY += KeyFrame[playIndex].incY;
				posZ += KeyFrame[playIndex].incZ;


				rotRodIzq += KeyFrame[playIndex].rotInc;
				rotRodDer += KeyFrame[playIndex].rotInc2;
				rotCodoDer += KeyFrame[playIndex].rotInc3;
				rotCodoIzq += KeyFrame[playIndex].rotInc4;
				i_curr_steps++;
			}

		}
	}

void animacionpelota()
{

	//Movimiento de la bola
	if (ball_on)
	{
		if (recorrido1balon)
		{
			movBallY += 0.005f;
			movBallX += 0.0075f;
			if (movBallY > 4)
			{
				recorrido1balon = false;
				recorrido2balon = true;
			}
		}
		if (recorrido2balon)
		{
			movBallY -= 0.005f;
			movBallX += 0.005f;
			if (movBallY <= 0)
			{
				recorrido2balon = false;
				recorrido3balon = true;
			}
		}
		if (recorrido3balon)
		{
			movBallX = 0.0f;
			movBallY = 0.0f;
			recorrido3balon = false;
			recorrido1balon = true;
			ball_on = false;
		}

	}
}

void animacionbandera()
{
	if (bandera_enable) {

		if (arriba_bandera) {
			SubirBanderas += 0.0075;

			if (SubirBanderas >= .6) {
				arriba_bandera = false;
				abajo_bandera = true;
				bandera_enable = false;
				
			}
		}
		if (abajo_bandera) {
			SubirBanderas -= 0.0075;

			if (SubirBanderas <= 0.0) {
				arriba_bandera = true;
				abajo_bandera = false;
				bandera_enable = false;

			}
		}




	}
}

void animaciontecho() 
{
	if (techo_enable) {

		if (abrir_techo) {
			Vtecho += 0.002;

			if (Vtecho >= 6.0) {
				abrir_techo = false;
				cerrar_techo = true;
				techo_enable = false;

			}
		}
		if (cerrar_techo) {
			Vtecho -= 0.002;

			if (Vtecho <= 0.0) {
				abrir_techo = true;
				cerrar_techo = false;
				techo_enable = false;

			}
		}


	}
}


void animacionpuerta()
{
	if (puerta_enable) {

		if (abajo_puerta) {
			MovPuerta -= 0.0075;

			if (MovPuerta <= -13.0) {
				abajo_puerta= false;
				arriba_puerta = true;
				puerta_enable = false;

			}
		}
		if (arriba_puerta) {
			MovPuerta += 0.0075;

			if (MovPuerta >= 0.0) {
				abajo_puerta = true;
				arriba_puerta = false;
				puerta_enable = false;
			}
		}




	}
}


void animacionanotacion()
{
	if (td_enable) {

		if (mov1) {
			
			scTD += 0.001;
			tTD -= 0.003;


			if (scTD >=  1.0) {
				mov1 = false;
				mov2 = true;
				
			}
		}

		if (mov2) {
			
			scTD -= 0.001;
			tTD += 0.003;
			scDW += 0.001;
			tDW -= 0.003;

			if (scTD <= 0.3 || scDW >= 1.0 ) {
				mov3 = true;
				mov2 = false;
				
			}
		}
		if (mov3) {

			scTD += 0.001;
			tTD -= 0.003;
			scDW -= 0.001;
			tDW += 0.003;

			if (scTD >= 1.0 || scDW <= 0.3) {
				mov3 = false;
				mov2 = true;
			}
		}



	}
}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_V])
	{
		if (play == false && (FrameIndex > 1))
		{

			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
			

		}
		else
		{
			play = false;
		}

	}


	if (keys[GLFW_KEY_B])
	{
		if (bandera_enable == false)
		{
			bandera_enable = true;
		}

	}

	if (keys[GLFW_KEY_N])
	{
		if (techo_enable == false)
		{
			techo_enable = true;
		}

	}

	if (keys[GLFW_KEY_M])
	{
		if (puerta_enable == false)
		{
			puerta_enable = true;
		}

	}


	if (keys[GLFW_KEY_C])
	{
		if (td_enable == false)
		{
			td_enable = true;
		}
		else
		{
			td_enable = false;
			mov1 = true;
			mov2 = false;
			mov3 = false;
			scTD = 0;
			tTD = 0;
			scDW = 0;
			tDW = 0;
		}
	}


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}


}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{



	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}






}