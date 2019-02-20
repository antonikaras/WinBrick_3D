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

#include <common/ModelLoader.h>
#include <common/texture.h>


using namespace std;
using namespace glm;

class Cube
{
public:

//****************	V A R I A B L E S	****************

	// used to save the input data
	vector<vec3> Verts;
	vector<vec3> Norms;
	vector<vec2> UVs;
	GLuint Texture;

	// How easy it is to destroy the cube
	int Life;
	// How smooth the cube is
	int Smoothness;

	// used to send the data to the GPU
	GLuint VAO;
	GLuint VertsVBO;
	GLuint UVsVBO;
	GLuint NormsVBO;

//****************	F U N C T I O N S	****************

	// Load the . obj file and the .jpg file
	void Load(string TextureName, int TextureQuality)
	{
		// the cube is the same only the texture changes so read it only once
		string ObjName = "models/cube.obj";
		loadOBJWithTiny(ObjName.c_str(), Verts, UVs, Norms);
		string TexturePath;
		if (TextureQuality == 1)
			TexturePath = "1024_768/";
		TextureName = TexturePath + TextureName + ".jpg";
		Texture = loadSOIL(TextureName.c_str());
		//loadSOIL(TextureName.c_str());
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Create the VAO and the VBOs
	void Bind()
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// vertex VBO
		glGenBuffers(1, &VertsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, VertsVBO);
		glBufferData(GL_ARRAY_BUFFER, Verts.size() * sizeof(glm::vec3), &Verts[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		// normals VBO
		glGenBuffers(1, &NormsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, NormsVBO);
		glBufferData(GL_ARRAY_BUFFER, Norms.size() * sizeof(glm::vec3), &Norms[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

		// uvs VBO
		glGenBuffers(1, &UVsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, UVsVBO);
		glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);
	}

	// Clear the data from the GPU
	void Delete()
	{
		glDeleteBuffers(1, &VertsVBO);
		glDeleteBuffers(1, &UVsVBO);
		glDeleteBuffers(1, &NormsVBO);
		glDeleteVertexArrays(1, &VAO);
		glDeleteTextures(1, &Texture);
	}
};
