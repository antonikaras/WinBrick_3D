// Include C++ headers
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <direct.h>
#include <fstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// include Soil
#include <SOIL.h>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/ModelLoader.h>
#include <common/texture.h>
#include <common/Board.h>
#include <common/Box.h>
//#include <common/Fonts.h>
#include <common/Sphere.h>
#include <common/Menu.h>
//#include <common/GameGrid_ver2.h>

using namespace std;
using namespace glm;

/*******************************************************************/

#define W_WIDTH 1024
#define W_HEIGHT 768
#define TITLE "WINBRICK"

// Global variables
GLFWwindow* window;
Camera* camera;
GLuint shaderProgram;
GLuint projectionMatrixLocation, viewMatrixLocation, modelMatrixLocation;
// light properties
GLuint LaLocation, LdLocation, LsLocation, lightPositionLocation, lightPowerLocation;
// material properties
GLuint KdLocation, KsLocation, KaLocation, NsLocation;
GLuint diffuceColorSampler;
GLuint diffuseTexture, UseTextureLocation;
// Enable Phong Lighting Location
GLuint EnPhLightLoc;

//		Settings used to create the game
int level = 1;	// Game level can be changed inside the game
int NumxCubes = 1;		//	number of cubes in x - axis 3
int NumyCubes = 1;		//	number of cubes in y - axis 3
int NumzCubes = 2;		//	number of cubes in z - axis 10

bool StopRender = false;
bool GameStarted = false;
/*******************************************************************/

// struct used for storing the light parameters
struct Light {
	glm::vec4 La;
	glm::vec4 Ld;
	glm::vec4 Ls;
	glm::vec3 lightPosition_worldspace;
	float power;
};

// struct used for storing the material coefficients
struct Material 
{
	glm::vec4 Ka;
	glm::vec4 Kd;
	glm::vec4 Ks;
	float Ns;
};

/*******************************************************************/

const Material CubeMaterial
{
	vec4(1),
	vec4(1),
	vec4(1),
	1
};

Light light{
	vec4{ 1, 1, 1, 1 },
	vec4{ 1, 1, 1, 1 },
	vec4{ 1, 1, 1, 1 },
	vec3{ 0, 0, 10 },
	20.0f
};

BoundingBox box;
Board board;
vector<Ball> balls;
// vector used to store the basic cubes
vector<Cube> cbs;
vector<vector<vector<bool> > > DeletedElements;

//GameGrid GG;
//WriteText WRT;
Menu MN;
/*******************************************************************/
// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();
void ReadCubes();
//void CreateGame();
void RenderGame(float t, float dt, int fps);
void GetUniformPointers();
void uploadMaterial(const Material& mtl);
void uploadLight(const Light& light);
void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
/*******************************************************************/