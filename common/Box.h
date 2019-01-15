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


class BoundingBox
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

	vec3 T = vec3(0);	// translation vector
	vec3 S = vec3(0);	// scale vector
	mat4 M = mat4(0);   // transformation matrix

	// used to save the input data
	
	vector<vec3> Verts;
	vector<vec3> Norms;

	// used to send the data to the GPU
	GLuint BoxVAO;
	GLuint VertsVBO;
	GLuint NormsVBO;

	GLuint UseTextLoc;
	GLuint modMatLoc;
	GLuint MaterialLoc[4];

	// Bounding Box Limits in x, y, z axes
	// minx, maxx, miny, maxy, minz, maxz
	float BoxLimits[6] = { 0 };

	Material BoxMaterial
	{
		vec4(vec3(0.1),1),
		vec4(1),
		vec4(vec3(0.1),1),
		1
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
		string ObjName = "models/cube.obj";
		vector<vec2> UVs;
		loadOBJWithTiny(ObjName.c_str(), Verts, UVs, Norms);
	}

	void Bind()
	{
		glGenVertexArrays(1, &BoxVAO);
		glBindVertexArray(BoxVAO);

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

	// Calculates the Limits of the box in x, y, z axes
	void FindLimits()
	{
		BoxLimits[0] = -S.x;
		BoxLimits[1] = S.x;
		BoxLimits[2] = -S.y;
		BoxLimits[3] = S.y;
		BoxLimits[4] = -S.z;
		BoxLimits[5] = S.z;
	}

	void Draw()
	{

		M = translate(mat4(), T) * scale(mat4(), S);	// transformation matrix
		glBindVertexArray(BoxVAO);
		uploadMaterial(BoxMaterial);
		glUniform1i(UseTextLoc, 0);
		glUniformMatrix4fv(modMatLoc, 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, Verts.size());
	}

	// Clear the data from the GPU
	void Delete()
	{
		glDeleteBuffers(1, &VertsVBO);
		glDeleteBuffers(1, &NormsVBO);
		glDeleteVertexArrays(1, &BoxVAO);
	}
};

