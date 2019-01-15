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

#include <common/ModelLoader.h>
#include <common/texture.h>
#include <common/RigidBody.h>

using namespace std;
using namespace glm;

class Ball 
{
private:
	// struct used for storing the material coefficients
	struct Material
	{
		glm::vec4 Ka;
		glm::vec4 Kd;
		glm::vec4 Ks;
		float Ns;
	};

	void uploadMaterial(Material& mtl)
	{
		glUniform4f(MaterialLoc[0], mtl.Ka.r, mtl.Ka.g, mtl.Ka.b, mtl.Ka.a);
		glUniform4f(MaterialLoc[1], mtl.Kd.r, mtl.Kd.g, mtl.Kd.b, mtl.Kd.a);
		glUniform4f(MaterialLoc[2], mtl.Ks.r, mtl.Ks.g, mtl.Ks.b, mtl.Ks.a);
		glUniform1f(MaterialLoc[3], mtl.Ns);
	}

	vec3 NewBallPosition(vec3 Pos, vec3 Vel, float dt)
	{
		vec3 NewPos;
		NewPos = Pos + Vel * dt;
		return NewPos;
	}

public:

	//void Movment(GLFWwindow* window, int key, int scancode, int action, int mods);

//****************	V A R I A B L E S	****************

	// sphere radious
	float radius = 0.1;
	// translation vector
	vec3 Pos = vec3(0, 0, 0);
	// scale vector
	vec3 S = vec3(radius, radius, radius);
	// transformation matrix
	mat4 M = mat4(0);
	// ball velocity
	vec3 V = vec3(0, 0, 0);							//	VELOCITY
	// ball level affects the damage of the ball 
	float BallLevel = 1.0f;							// LEVEL		
	// Ball direction
	vec3 BallDirection = vec3(0, 0, 1);				//	DIRECTION
	// Boundary Box dimensions, used to calculate
	// the limits of the ball movement
	// minx, maxx, miny, maxy
	float BoundingBoxDims[6] = { 0 };
	// Board movment Boundries
	// minx, maxx, miny, maxy
	float BallMovementLimits[6] = { 0 };

	vector<vec3> Verts;
	vector<vec3> Norms;

	GLuint VAO;
	GLuint VertsVBO;
	GLuint NormsVBO;

	GLuint MaterialLoc[4];
	GLuint UseTextLoc;
	GLuint ModMatLoc;


	Material BallMaterial
	{
		// Silver
		vec4{ 0.19225f, 0.19225f, 0.19225f, 1.0f },
		vec4{ 0.50754f, 0.50754f, 0.50754f, 1.0f },
		vec4{ 0.50827f, 0.50827f, 0.50827f, 1.0f },
		51.2f
	};

//****************	F U N C T I O N S	****************

	void Load()
	{
		char cwd[150], *cwd2;
		cwd2 = _getcwd(cwd, 150);
		//cout << cwd2 << endl;
		string wd = string(cwd2);	// store working directory
		chdir("../");				// go to the previous directory
		cwd2 = _getcwd(cwd, 150);
		string pwd = string(cwd2);
		string nwd = pwd + "/resources/";
		chdir(nwd.c_str());			// Change to the resources directory
		string ObjName = "models/sphere.obj";
		vector<vec2> UVs;
		loadOBJWithTiny(ObjName.c_str(), Verts, UVs, Norms);
	}

	void update(float t = 0, float dt = 0)
	{
		Pos = NewBallPosition(Pos, V, dt);
		M = translate(mat4(), Pos) * scale(mat4(), S);	// transformation matrix
		for (int i = 0; i < 6; i++)
			BallMovementLimits[i] = BoundingBoxDims[i] + pow(-1,i) * radius;
		
	};

	void Bind()
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VertsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, VertsVBO);
		glBufferData(GL_ARRAY_BUFFER, Verts.size() * sizeof(glm::vec3), &Verts[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &NormsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, NormsVBO);
		glBufferData(GL_ARRAY_BUFFER, Norms.size() * sizeof(glm::vec3), &Norms[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
	}

	//	Draw sphere
	void Draw()
	{
		glBindVertexArray(VAO);
		glUniform1i(UseTextLoc, 0);
		uploadMaterial(BallMaterial);
		glUniformMatrix4fv(ModMatLoc, 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, Verts.size());
	}

	// Clear the data from the GPU
	void Delete()
	{
		glDeleteBuffers(1, &VertsVBO);
		glDeleteBuffers(1, &NormsVBO);
		glDeleteVertexArrays(1, &VAO);
	}
};
