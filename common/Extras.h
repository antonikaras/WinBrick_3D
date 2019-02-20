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

class Extras
{
private:

//****************	V A R I A B L E S	****************
	GLuint TextID[6];
	
	GLuint VAO[6], VertsVBO[6], NormsVBO[6], UVsVBO[6];
	vector<vec3> Verts = { vec3(-0.5,  0.5, 0),
							vec3(0.5,  0.5, 0),
							vec3(0.5, -0.5, 0),
							vec3(-0.5, -0.5, 0),
							vec3(-0.5,  0.5, 0),
							vec3(0.5, -0.5, 0)	};
	vector<vec3> Norms = {  vec3(0, 0, -1),
							vec3(0, 0, -1),
							vec3(0, 0, -1),
							vec3(0, 0, -1),
							vec3(0, 0, -1),
							vec3(0, 0, -1)		};
	vector<vec2> UVs = { vec2(0,0),
						 vec2(0,1),
						 vec2(1,1),
						 vec2(1,0),
						 vec2(0,0),
						 vec2(1,1) };

//****************	F U N C T I O N S	****************

public:

//****************	V A R I A B L E S	****************
	
	// convert etras' names to indexes
	enum TextNames
	{
		add_ball = 0, decrease_size, heart, increase_size, sticky_board
	};
	// Use texture location
	GLuint UseTextLoc;
	// Model matrix location
	GLuint ModMatLoc;
	// Scale factor that will be applied to every extra board
	vec3 S = vec3(0.4);

//****************	F U N C T I O N S	****************

// Load the .png file
	void Load()
	{
		char cwd[150], *cwd2;
		cwd2 = _getcwd(cwd, 150);
		//cout << cwd2 << endl;
		string wd = string(cwd2);	// store working directory
		chdir("../");				// go to the previous directory
		cwd2 = _getcwd(cwd, 150);
		string pwd = string(cwd2);
		string nwd = pwd + "/resources/Extras";
		chdir(nwd.c_str());			// Change to the resources directory
									// ----> open the txt with the texture's names <----
		ifstream myReadFile;
		myReadFile.open("Extras.txt");
		char output[100];
		if (myReadFile.is_open())
		{
			int i = 0;
			// ----> start reading the txt file <----
			while (!myReadFile.eof())
			{
				// ----> read the name of the texture <----
				myReadFile >> output;
				string TextureName = string(output) + ".png";
				TextID[i] = loadSOIL(TextureName.c_str());
				glBindTexture(GL_TEXTURE_2D, TextID[i]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glGenerateMipmap(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				i++;
			}
		}
		// ----> close the txt with the material names <----	
		myReadFile.close();
		// ----> return to the intitial working directory <----	
		chdir(wd.c_str());
		
	}

	// Create the VAO and the VBOs
	void Bind()
	{
		for (int i = 0; i < 6; i++)
		{
			glGenVertexArrays(1, &VAO[i]);
			glBindVertexArray(VAO[i]);

			// vertex VBO
			glGenBuffers(1, &VertsVBO[i]);
			glBindBuffer(GL_ARRAY_BUFFER, VertsVBO[i]);
			glBufferData(GL_ARRAY_BUFFER, Verts.size() * sizeof(glm::vec3), &Verts[0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(0);

			// normals VBO
			glGenBuffers(1, &NormsVBO[i]);
			glBindBuffer(GL_ARRAY_BUFFER, NormsVBO[i]);
			glBufferData(GL_ARRAY_BUFFER, Norms.size() * sizeof(glm::vec3), &Norms[0], GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(1);

			// uvs VBO
			glGenBuffers(1, &UVsVBO[i]);
			glBindBuffer(GL_ARRAY_BUFFER, UVsVBO[i]);
			glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(2);
		}
	}

	// Draw the Extras
	void Draw(vec3 &pos, float dt, int Index)
	{
		glBindVertexArray(VAO[Index]);
		glUniform1i(UseTextLoc, 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextID[Index]);
		mat4 M = translate(mat4(), pos) * scale(mat4(), S);
		glUniformMatrix4fv(ModMatLoc, 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		pos.z += dt;
	}

	// Clear the data from the GPU
	void Delete()
	{
		for (int i = 0; i < 6; i++)
		{
			glDeleteBuffers(1, &VertsVBO[i]);
			glDeleteBuffers(1, &UVsVBO[i]);
			glDeleteBuffers(1, &NormsVBO[i]);
			glDeleteVertexArrays(1, &VAO[i]);
			glDeleteTextures(1, &TextID[i]);
		}
	}
};