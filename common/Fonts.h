// Include C++ headers
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <direct.h>
#include <fstream>
#include <sstream>

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

class WriteText
{
private:

public:
	//	*****	V A R I A B L E S	****

	// Model Matrix Location
	GLuint modmatLoc;

	// Use texture Location
	GLuint UseTextLoc;

	// diffuse color sampler 
	GLuint DifColSam;

	// Enable Phong Lighting Location
	GLuint EnPhLightLoc;

	// Model Matrix
	mat4 M = mat4(1);

	// used to save the input data
	vector<vec3> Verts = {vec3(-1.0, -1.0, 0.0),vec3(-1.0, 1.0, 0.0), vec3(1.0, -1.0, 0.0),
						  vec3(1.0, -1.0, 0.0),vec3(1.0, 1.0, 0.0), vec3(-1.0, 1.0, 0.0) };
	vector<vec3> Norms = {vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0),  
						  vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0)};
	GLuint Texture;
	vector<vec2> UVs;

	// used to send the data to the GPU
	GLuint VAO;
	GLuint VertsVBO;
	GLuint UVsVBO;
	GLuint NormsVBO;

	// used for storing every character
	struct Char
	{
		int ascii;
		// UV coordinates for each character
		vector<vec2> UVs;	
		// scale
		vec3 S;
		// x - axis length
		float xlen;
		// y - axis elevation
		float offset;
	};

	// vector used for storing all the characters
	vector<Char> Chars;

	//	****	F U N C T I O N S	****

	// Load the font texture 
	void Load()
	{
		// ----> change the working directory to read the cubes <----

		char cwd[150], *cwd2;
		cwd2 = _getcwd(cwd, 150);
		//cout << cwd2 << endl;
		string wd = string(cwd2);	// store working directory
		chdir("../");				// go to the previous directory
		cwd2 = _getcwd(cwd, 150);
		string pwd = string(cwd2);
		string nwd = pwd + "/resources/Fonts";
		chdir(nwd.c_str());			// Change to the fonts directory

		// Read and load the font texture
		Texture = loadSOIL("fonts.bmp");
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Read the parameters for evey character
		int ascii;
		float x1, x2, y1, y2, off;
		ifstream fonts;
		fonts.open("fonts.txt");
		char let[10];
		if (fonts.is_open())
		{
			// ----> start reading the txt file <----
			int i = 0;
			while (!fonts.eof())
			{
				// disregard the first 5 words
				while (i < 6)
				{
					fonts >> let;
					i++;
				//	cout << let << " ";
				}
				//cout << endl;
				// read ascii code
				fonts >> let;
				ascii = atoi(let);
				// read x1
				fonts >> let;
				x1 = float(atoi(let)) / 1024;
				// read y1
				fonts >> let;
				y1 = float(atoi(let)) / 768;
				// read x2
				fonts >> let;
				x2 = float(atoi(let)) / 1024;
				// read y2 
				fonts >> let;
				y2 = float(atoi(let)) / 768;
				// read offset
				fonts >> let;
				off = float(atoi(let)) / 768;
				//cout << ascii << " " << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
				Char Ch;
				Ch.ascii = ascii;
				Ch.UVs.push_back(vec2(x1, y1));
				Ch.UVs.push_back(vec2(x1, y2));
				Ch.UVs.push_back(vec2(x2, y1));
				Ch.UVs.push_back(vec2(x2, y1));
				Ch.UVs.push_back(vec2(x2, y2));
				Ch.UVs.push_back(vec2(x1, y2));
				Ch.S = vec3(x2 - x1, y1 - y2, 1.0);
				Ch.xlen = x2 - x1;
				Ch.offset = off;
				Chars.push_back(Ch);
				UVs = Ch.UVs;
			}
		}
		// close the .txt file
		fonts.close();
		// return to the initial working directory
		chdir(wd.c_str());
	}

	// Create the VAO and the VBOs
	void Bind()
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

	//	UVs.resize(6);
		//UVs = Chars[4].UVs;
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

	// Draw char
	void Draw(int index, mat4 M)
	{
	//	glUniform1i(EnPhLightLoc, 1);
		UVs = Chars[index].UVs;
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, UVsVBO);
		glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);
		glUniform1i(UseTextLoc, 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(DifColSam, 0);
		glUniformMatrix4fv(modmatLoc, 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, Verts.size());
	}

	// Writes the text on the window
	void Write(const char *text, vec3 FirstCharPos, float sz)
	{
		/*	index	ascii	character
			0		33		!
			1		63		?
			2-27	65-90	A-Z
			28		91		[
			29		92		\
			30		93		]
			31-56	97-122	a-z
			57		123		{
			58		124		|
			59		125		}
			60		40		(
			61		41		)
			62		42		*
			63		43		+
			64		44		,
			65		45		-
			66		46		.
			67		47		/
			68-77	48-57   0-9
			78				"space"
		*/
		
		// Translation vector
		vec3 T = FirstCharPos;
	//	cout << strlen(text) << endl;
		for (int i = 0; i < strlen(text); i++)
		{
			
			// convert character to ascii code
			int let = text[i];
			// find the character index in the Chars vector
			int index;
			if (let == 33)
				index = 0;
			else if (let == 63)
				index = 1;
			else if ((let > 64) && (let < 94))
				index = let - 63;
			else if ((let >= 97) && (let <= 125))
				index = let - 66;
			else if ((let >= 40) && (let <= 57))
				index = let + 20;
			else
				index = 78;	// character is not available
	//		cout << text[i] << " ";
			if (index >= 0)
			{
				vec3 newS = vec3(Chars[index].S.x * sz, Chars[index].S.y * sz, Chars[index].S.z);
				vec3 Toff = vec3(0);
				if (Chars[index].offset != 0)
					Toff = -vec3(0.0, Chars[index].offset, 0.0);
				mat4 M = translate(mat4(), T + Toff) * scale(mat4(), newS) ;
				//cout << M[3][1] << " ";
				Draw(index, M);
			/*	if (Chars[index].offset != 0)
					T = T + vec3(0.0, 0.004f * newS.y / Chars[index].offset, 0.0);*/
				T += vec3(0.15f * newS.x / Chars[index].xlen , 0.0, 0.0);
			}
			
		}
	//	cout << endl;
	}

	// Delete data from the GPU
	void Delete()
	{
		glDeleteBuffers(1, &VertsVBO);
		glDeleteBuffers(1, &UVsVBO);
		glDeleteBuffers(1, &NormsVBO);
		glDeleteVertexArrays(1, &VAO);
		glDeleteTextures(1, &Texture);
	}
};