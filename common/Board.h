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

using namespace std;
using namespace glm;

class Board
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

public:
	
//****************	V A R I A B L E S	****************

	//void Movment(GLFWwindow* window, int key, int scancode, int action, int mods);
	// translation vector
	vec3 Pos = vec3(0, 0, 6);
	// scale vector
	vec3 S = vec3(1.2, 0.8, 0);
	// transformation matrix
	mat4 M = mat4(0);
	// used for the board movment
	float speed = 15.0;
	// Board level affects the size of the Board 
	float BoardLevel = 1.0f;
	// Boundary Box dimensions, used to calculate
	// the limits of the board movement
	// minx, maxx, miny, maxy, minz, maxz
	float BoundingBoxDims[6] = { 0 };
	// Board movment Boundries
	// minx, maxx, miny, maxy
	float BoardMovementLimits[4] = { 0 };
	// Board limits
	// minx, maxx, miny, maxy, minz, maxz
	float BoardLimits[6] = { 0 };

	GLuint VAO;
	GLuint VertsVBO;
	GLuint NormsVBO;

	GLuint MaterialLoc[4];
	GLuint UseTextLoc;
	GLuint ModMatLoc;

	vector<vec3> Verts = { vec3(-0.5,  0.5, 0.0),
		vec3(0.5,  0.5, 0.0),
		vec3(0.5, -0.5, 0.0),
		vec3(-0.5, -0.5, 0.0),
		vec3(-0.5,  0.5, 0.0),
		vec3(0.5, -0.5, 0.0)
	};

	vector<vec3> Norms = { vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.0, 1.0)
	};

	Material BoardMaterial
	{
		// Emerald
		vec4{ 0.0215f, 0.1745f, 0.0215f, 0.55f },
		vec4{ 0.07568f, 0.61424f, 0.07568f, 0.55f },
		vec4{ 0.633f, 0.727811f, 0.633f, 0.55f },
		76.8f
	};

//****************	F U N C T I O N S	****************

	void update()
	{
		vec3 tmp_s = S;
		if (BoardLevel < 1.5)
			S = (S) * BoardLevel;
		
		M = translate(mat4(), Pos) * scale(mat4(), S);	// transformation matrix
		
		BoardMovementLimits[0] = BoundingBoxDims[0] + 0.5 * S.x;
		BoardMovementLimits[1] = BoundingBoxDims[1] - 0.5 * S.x;
		BoardMovementLimits[2] = BoundingBoxDims[2] + 0.5 * S.y;
		BoardMovementLimits[3] = BoundingBoxDims[3] - 0.5 * S.y;
		
		BoardLimits[0] = (M * vec4(Verts[0], 1)).x;
		BoardLimits[1] = (M * vec4(Verts[1], 1)).x;
		BoardLimits[2] = (M * vec4(Verts[2], 1)).y;
		BoardLimits[3] = (M * vec4(Verts[1], 1)).y;
		BoardLimits[4] = (M * vec4(Verts[0], 1)).z;
		BoardLimits[5] = (M * vec4(Verts[3], 1)).z;

		S = tmp_s;
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

	//	Draw board
	void Draw()
	{
		update();
		glBindVertexArray(VAO);
		glUniform1i(UseTextLoc, 0);
		uploadMaterial(BoardMaterial);
		glUniformMatrix4fv(ModMatLoc, 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void Movment(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		float dt = 0.01f;

		bool limup = BoardMovementLimits[3] - Pos.y > 0;
		bool limdown = BoardMovementLimits[2] - Pos.y < 0;
		bool limleft = BoardMovementLimits[0] -  Pos.x < 0;
		bool limright = BoardMovementLimits[1] - Pos.x > 0;

		// Move board up
		if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && (limup))
			Pos += vec3(0.0f, 1.0f, 0.0f) * dt * speed;

		// Move board down
		if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (limdown))
			Pos -= vec3(0.0f, 1.0f, 0.0f) * dt * speed;

		// Move board Right
		if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) && (limright))
			Pos += vec3(1.0f, 0.0f, 0.0f) * dt * speed;

		// Move Board left
		if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) && (limleft))
			Pos -= vec3(1.0f, 0.0f, 0.0f) * dt * speed;

		// Increase Board Speed
		if ((glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) && (speed < 30))
			speed += 0.2;

		// Decrease Board Speed
		if ((glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) && (speed > 15))
			speed -= 0.2;
		update();
	}

	void Delete()
	{
		glDeleteBuffers(1, &VertsVBO);
		//	glDeleteBuffers(1, &UVsVBO);
		glDeleteBuffers(1, &NormsVBO);
		glDeleteVertexArrays(1, &VAO);
	}
};
