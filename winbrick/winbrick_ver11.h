// Include C++ headers
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <direct.h>
#include <fstream>
#include <math.h>

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
#include <common/ModelLoader.h>
#include <common/texture.h>
#include <common/Board.h>
#include <common/Box.h>
//#include <common/Fonts.h>
#include <common/Sphere.h>
#include <common/Menu.h>
//#include <common/GameGrid_ver2.h>
#include <common/Extras.h>

using namespace std;
using namespace glm;

/*******************************************************************/

#define W_WIDTH 1024
#define W_HEIGHT 768
#define TITLE "WINBRICK"

// Global variables
GLFWwindow* window;
GLuint shaderProgram;
GLuint projectionMatrixLocation, viewMatrixLocation, modelMatrixLocation;
// light properties
GLuint LaLocation, LdLocation, LsLocation, SideLightPositionLocation, FrontLightPositionLocation;
// material properties
GLuint KdLocation, KsLocation, KaLocation, NsLocation;
GLuint diffuceColorSampler;
GLuint diffuseTexture, UseTextureLocation;
// cube side mirror properties
GLuint MirVAO, MirVertsVBO, MirUVsVBO, MirNormsVBO;
float MirVerts[] = { -0.5,  0.5, 0,
						  0.5,  0.5, 0,
						  0.5, -0.5, 0,
						 -0.5, -0.5, 0,
						 -0.5,  0.5, 0,
						  0.5, -0.5, 0 };
vector<vec2> MirUVs = { vec2(0,0),
						vec2(0,1),
						vec2(1,1),
						vec2(1,0),
						vec2(0,0),
						vec2(1,1) };
vector<vec3> MirNorms = { vec3(0,0,-1),
							vec3(0,0,-1), 
							vec3(0,0,-1), 
							vec3(0,0,-1), 
							vec3(0,0,-1), 
							vec3(0,0,-1) };
//		Settings used to create the game
int level = 1;			// Game level can be changed inside the game
int NumxCubes = 1;		//	number of cubes in x - axis 3
int NumyCubes = 1;		//	number of cubes in y - axis 3
int NumzCubes = 2;		//	number of cubes in z - axis 2
int score = 0;			//	game score
int Lives = 3;			//	game lives
int scoreMultiplier = 1;//	score multiplier
bool StopRender = false;
bool GameStarted = false;
bool EndGame = false;
bool StickyBoard = false;
/*******************************************************************/
// OPTIONS' MENU VARIABLES
bool EnableMirror = true;
bool EnableBoardMirrorProjection = true;
GLuint BoardMirrorVAO, BoardMirrorVertsVBO;
vector<vec3> BoardMirrorVerts;
bool EnableBallBoardProjection = true;
GLuint BallBoardVAO, BallBoardVertsVBO;
bool EnableBallMirrorProjection = true;
GLuint BallMirrorVAO, BallMirrorVertsVBO;
/*******************************************************************/

// struct used for storing the light parameters
struct Light {
	glm::vec4 La;
	glm::vec4 Ld;
	glm::vec4 Ls;
	glm::mat4 SideLightPosition;
	glm::vec4 FrontLightPosition;
};

// struct used for storing the material coefficients
struct Material 
{
	glm::vec4 Ka;
	glm::vec4 Kd;
	glm::vec4 Ks;
	float Ns;
};

// struct used for storing the camera properties
struct Camera
{
	vec3 position;
	float FoV = 45.0f;
	vec3 direction;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	vec3 right = vec3(1.0, 0.0, 0.0);
	vec3 up = vec3(0, 1, 0);
	void update()
	{
		//vec3 up = cross(right, direction);
		projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
		viewMatrix = lookAt(position, position + direction, up);
	}
};
Camera camera;
Ball ball;
/*******************************************************************/

const Material CubeMaterial
{
	vec4(0.19225, 0.19225, 0.19225, 1),
	vec4(0.50754, 0.50754, 0.50754, 1),
	vec4(0.508273, 0.508273, 0.508273, 1),
	51.2
};

Light light1, light2;
BoundingBox box;
Board board;
vector<Ball> balls;
// vector used to store the basic cubes
vector<Cube> cbs;
vector<vector<vector<bool> > > DeletedElements;

//GameGrid GG;
//WriteText WRT;
Menu MN;
Extras extras;
// Textures used to create the mirrors
GLuint TextID[4] = { -1 };
bool BoxCreated = false;
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
void InitializeGameEnvironment();
void ProcessExtras(int ExtraPos);
/*******************************************************************/