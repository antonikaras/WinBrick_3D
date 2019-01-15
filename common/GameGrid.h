#include <iostream>
#include <vector>
#include <algorithm>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "common/Cube.h"

using namespace std;
using namespace glm;
//*****************************************************************************************************************

class GameGrid
{
private:
//	**********	V A R I A B L E S	**********

	vector<vector<vec2> > RightSide;	// stores the indexes and the x-coordinate of the Right brick wall
	vector<vector<vec2> > TopSide;		// stores the indexes and the y-coordinate of the Top brick wall
	vector<vector<vec2> > LeftSide;		// stores the indexes and the x-coordinate of the Left brick wall
	vector<vector<vec2> > BottomSide;	// stores the indexes and the y-coordinate of the Bottom brick wall
	vector<vector<vec2> > FrontSide;	// stores the indexes and the z-coordinate of the Front brick wall

	float RightSideLimit = 0.0;			// max x value on the Right grid 
	float TopSideLimit = 0.0;			// max y value on the Top grid
	float LeftSideLimit = 0.0;			// min x value on the Left grid
	float BottomSideLimit = 0.0;		// min y value on the Bottom grid
	float FrontSideLimit = 0.0;			// max z value on the Front grid

	int RightSideCollisions = 0;		// Number of collisions between the ball and the right grid side
	int TopSideCollisions = 0;			// Number of collisions between the ball and the top grid side
	int LefttSideCollisions = 0;		// Number of collisions between the ball and the lrftt grid side
	int BottomSideCollisions = 0;		// Number of collisions between the ball and the bottom grid side
	int FronttSideCollisions = 0;		// Number of collisions between the ball and the front grid side

//	**********	F U N C T I O N S	**********

/******************************************************************************/

	// This Function finds the min or max element per side
	float FindMaxCoordinate(vector<vector<vec2> > Side)
	{
		float m = abs(Side[0][0].y);
		vector<vec2> tmp = Side[0];
		for (int i = 0; i < Side.size(); i++)
		{
			for (int j = 0; j < tmp.size(); j++)
			{
				if (m < abs(Side[i][j].y))
					m = abs(Side[i][j].y);
			}
		}

		return m;
	}

	// This function creates the side grid which
	//  is used when a brick is destroyed
	void CreateSides()
	{
		int dim1FirstElem[4] = { (2 * NumxCubes), (2 * NumxCubes + 1) * (2 * NumyCubes + 1) - (2 * NumxCubes + 1) , 0, 0 };
		int dim1Step = (2 * NumxCubes + 1) * (2 * NumyCubes + 1);
		int dim2Step[4] = { (2 * NumyCubes + 1), 1 , (2 * NumyCubes + 1), 1 };

		// Create Right Side
		for (int i = 0; i < (2 * NumyCubes + 1); i++)
		{
			vector<vec2> tmp;
			for (int j = 0; j < NumzCubes; j++)
			{
				int ElemIndex = (dim1FirstElem[0] + i * dim2Step[0]) + j * dim1Step;
				tmp.push_back(vec2(ElemIndex,cbsSpecs[ElemIndex].Pos.x));
			}
			RightSide.push_back(tmp);
		}
		RightSideLimit = FindMaxCoordinate(RightSide);

		// Create Top Side
		for (int i = 0; i < (2 * NumxCubes + 1); i++)
		{
			vector<vec2> tmp;
			for (int j = 0; j < NumzCubes; j++)
			{
				int ElemIndex = (dim1FirstElem[1] + i * dim2Step[1]) + j * dim1Step;
				tmp.push_back(vec2(ElemIndex, cbsSpecs[ElemIndex].Pos.y));
			}
			TopSide.push_back(tmp);
		}
		TopSideLimit = FindMaxCoordinate(TopSide);

		// Create Left Side
		for (int i = 0; i < (2 * NumyCubes + 1); i++)
		{
			vector<vec2> tmp;
			for (int j = 0; j < NumzCubes; j++)
			{
				int ElemIndex = (dim1FirstElem[2] + i * dim2Step[2]) + j * dim1Step;
				tmp.push_back(vec2(ElemIndex, cbsSpecs[ElemIndex].Pos.x));
			}
			LeftSide.push_back(tmp);
		}
		LeftSideLimit = FindMaxCoordinate(LeftSide);

		// Create Bottom Side
		for (int i = 0; i < (2 * NumxCubes + 1); i++)
		{
			vector<vec2> tmp;
			for (int j = 0; j < NumzCubes; j++)
			{
				int ElemIndex = (dim1FirstElem[3] + i * dim2Step[3]) + j * dim1Step;
				tmp.push_back(vec2(ElemIndex, cbsSpecs[ElemIndex].Pos.y));
			}
			BottomSide.push_back(tmp);
		}
		BottomSideLimit = FindMaxCoordinate(BottomSide);

	}

	/***********************************************************************************/

	// This function is used to update the front side grid
	// when a brick in the front has been destroyed
	void UpdateFrontSide(int loc[2])
	{
		// x = loc[0], y = loc[1], z = depth
		int Index = FrontSide[loc[1]][loc[0]].x;
		int depth = Index / (2 * NumxCubes + 1) / (2 * NumyCubes + 1);
		//	cout << Index << "    " << depth << endl;

		if (depth == 0)
		{
			DeletedElement[depth][loc[1]][loc[0]] = true;
			//Index = -1;
		}
		else if (depth > 0)
		{
			Index -= (2 * NumxCubes + 1)*(2 * NumyCubes + 1);
			DeletedElement[depth][loc[1]][loc[0]] = true;
			FrontSide[loc[1]][loc[0]].y = cbsSpecs[Index].Pos.z;
			FrontSide[loc[1]][loc[0]].x = Index;
			if (DeletedElement[depth - 1][loc[1]][loc[0]] == true)	
				UpdateFrontSide(loc);
		}
	}

	// This function is used to update the Right side grid
	// when a brick in the Right has been destroyed
	void UpdateRightSide(int loc[2])
	{
		// x = width, y = loc[0], z = loc[1]
		int Index = RightSide[loc[0]][loc[1]].x;
		int width = Index % (2 * NumxCubes + 1);
		cout << " ---->update right side " <<  Index << " " <<  width << " " << loc[0] << " " << loc[1] << endl;
		if (width == 0)
		{
			DeletedElement[loc[1]][loc[0]][width] = true;
			//RightSide[loc[0]][loc[1]].x = -1;
		}
		else if (width > 0)
		{
			Index--;
			DeletedElement[loc[1]][loc[0]][width] = true;
			RightSide[loc[0]][loc[1]].x = Index;
			RightSide[loc[0]][loc[1]].y = cbsSpecs[Index].Pos.x;
			if (DeletedElement[loc[1]][loc[0]][width - 1] == true)
				UpdateRightSide(loc);
		}
	}

	// This function is used to update the Top side grid
	// when a brick in the Top has been destroyed
	void UpdateTopSide(int loc[2])
	{
		// x = loc[0], y = height, z = loc[1]
		int Index = TopSide[loc[0]][loc[1]].x;
		int height = (Index / (2 * NumxCubes + 1)) % (2 * NumyCubes + 1);
		cout << " ---- > update Top side " << Index << " " << loc[0] << " " << height << " " << loc[1] << endl;
		if (height == 0)
		{
			//TopSide[loc[0]][loc[1]].x = -1;
			DeletedElement[loc[1]][height][loc[0]] = true;
		}
		else if (height > 0)
		{
			Index -= (2 * NumxCubes + 1);
			TopSide[loc[0]][loc[1]].x = Index;
			TopSide[loc[0]][loc[1]].y = cbsSpecs[Index].Pos.y;
			DeletedElement[loc[1]][height][loc[0]] = true;
			if (DeletedElement[loc[1]][height - 1][loc[0]] == true)
				UpdateTopSide(loc);
		}
	}

	// This function is used to update the Left side grid
	// when a brick in the Left has been destroyed
	void UpdateLeftSide(int loc[2])
	{
		// x = width, y = loc[0], z = loc[1]
		int Index = LeftSide[loc[0]][loc[1]].x;
		int width = Index % (2 * NumxCubes + 1);
		cout << " ---->update left side " << Index << " " << width << " " << loc[0] << " " << loc[1] << endl;
		if (width == (2 * NumxCubes))
		{
			DeletedElement[loc[1]][loc[0]][width] = true;
			//Index = -1;
		}
		else if (width >= 0)
		{
			Index++;
			DeletedElement[loc[1]][loc[0]][width] = true;
			LeftSide[loc[0]][loc[1]].x = Index;
			LeftSide[loc[0]][loc[1]].y = cbsSpecs[Index].Pos.x;
			if (DeletedElement[loc[1]][loc[0]][width + 1] == true)
				UpdateLeftSide(loc);
		}
	}

	// This function is used to update the Bottom side grid
	// when a brick in the Bottom has been destroyed
	void UpdateBottomSide(int loc[2])
	{
		// x = loc[0], y = height, z = loc[1]
		int Index = BottomSide[loc[0]][loc[1]].x;
		int height = (Index / (2 * NumxCubes + 1)) % (2 * NumyCubes + 1);
		cout << " ---- > update Bottom side " << Index << " " << loc[0] << " " << height << " " << loc[1] << endl;
		if (height % (2 * NumyCubes) == 0)
		{
			//Index = -1;
			DeletedElement[loc[1]][height][loc[0]] = true;
		}
		else if (height >= 0)
		{
			Index += (2 * NumxCubes + 1);
			DeletedElement[loc[1]][height][loc[0]] = true;
			BottomSide[loc[0]][loc[1]].x = Index;
			BottomSide[loc[0]][loc[1]].y = cbsSpecs[Index].Pos.y;
			if (DeletedElement[loc[1]][height + 1][loc[0]] == true)
				UpdateBottomSide(loc);
		}
	}

	/*****************************************************************************/

	// This function handles the collisions between the ball
	// and the front side of the cube grid
	void FrontSideCollision(float r, vec3 pos, vec3 &speed)
	{

		// find the nearest index depending on the ball x - coordinate
		int x = (pos.x / s.x) + NumxCubes;
		// find the nearest index depending on the ball y - coordinate
		int y = (pos.y / s.y) + NumyCubes;
		cout << " Front side collision ";
		int loc[2] = { x,y };
		int Index = FrontSide[x][y].x;
		// reduce the brick life
		if (Index >= 0)
		{
			int zn = Index / (2 * NumxCubes + 1) / (2 * NumyCubes + 1);
			int yn = (Index / (2 * NumxCubes + 1)) % (2 * NumyCubes + 1);
			int xn = Index % (2 * NumxCubes + 1);
			//cout << " =+==+ " << xn << " " << yn << " " << zn;
			if (DeletedElement[zn][yn][xn] == false)
			{
				cout << " life " << cbsSpecs[Index].Life << endl;
				cbsSpecs[Index].Life--;
				if (cbsSpecs[Index].Life == 0)
					UpdateFrontSide(loc);
			}
			speed = reflect(speed, vec3(0.0, 0.0, 1.0));

		}
		cout << " out" << endl;

	}

	// This function handles the collisions between the ball
	// and the right side of the cube grid
	void RightSideCollision(float r, vec3 pos, vec3 &speed)
	{

		// find the nearest index depending on the ball y - coordinate
		int y = (pos.y / s.y) + NumyCubes;
		// find the nearest index depending on the ball z - coordinate
		int z = (pos.z / s.z);
		cout << " Right side collision ";
		int loc[2] = { y,z };
		int Index = RightSide[y][z].x;
		cout << Index;
		// reduce the brick life 
		if (Index >= 0)
		{
			int zn = Index / (2 * NumxCubes + 1) / (2 * NumyCubes + 1);
			int yn = (Index / (2 * NumxCubes + 1)) % (2 * NumyCubes + 1);
			int xn = Index % (2 * NumxCubes + 1);
			cout << " =+==+ " << xn << " " << yn << " " << zn;
			if (DeletedElement[zn][yn][xn] == false)
			{
				cout << " life " << cbsSpecs[Index].Life << endl;
				cbsSpecs[Index].Life--;
				if (cbsSpecs[Index].Life == 0)
					UpdateRightSide(loc);
			}
			speed = reflect(speed, vec3(1.0, 0, 0));
		}

		cout << " out " << endl;

	}

	// This function handles the collisions between the ball
	// and the Top side of the cube grid
	void TopSideCollision(float r, vec3 pos, vec3 &speed)
	{
		// find the nearest index depending on the ball yx- coordinate
		int x = (pos.x / s.x) + NumxCubes;
		// find the nearest index depending on the ball z - coordinate
		int z = (pos.z / s.z);
		cout << " Top side collision " ;
		int loc[2] = { x,z };
		int Index = TopSide[x][z].x;
		cout << Index;
		// reduce the brick life
		if (Index >= 0)
		{
			int zn = Index / (2 * NumxCubes + 1) / (2 * NumyCubes + 1);
			int yn = (Index / (2 * NumxCubes + 1)) % (2 * NumyCubes + 1);
			int xn = Index % (2 * NumxCubes + 1);
			cout << " =+==+ " << xn << " " << yn << " " << zn;
			if (DeletedElement[zn][yn][xn] == false)
			{
				cout << " life " << cbsSpecs[Index].Life << endl;
				cbsSpecs[Index].Life--;
				if (cbsSpecs[Index].Life == 0)
					UpdateTopSide(loc);
			}
			
		}

			cout << " out" << endl;
	}

	// This function handles the collisions between the ball
	// and the Left side of the cube grid
	void LeftSideCollision(float r, vec3 pos, vec3 &speed)
	{
		// find the nearest index depending on the ball y - coordinate
		int y = (pos.y / s.y) + NumyCubes;
		// find the nearest index depending on the ball z - coordinate
		int z = (pos.z / s.z);
		cout << " Left side collision ";
		int loc[2] = { y,z };
		int Index = LeftSide[y][z].x;
		cout << Index;
		// reduce the brick life
		if (Index >= 0)
		{
			int zn = Index / (2 * NumxCubes + 1) / (2 * NumyCubes + 1);
			int yn = (Index / (2 * NumxCubes + 1)) % (2 * NumyCubes + 1);
			int xn = Index % (2 * NumxCubes + 1);
			cout << " =+==+ " << xn << " " << yn << " " << zn;
			if (DeletedElement[zn][yn][xn] == false)
			{
				cout << " life " << cbsSpecs[Index].Life << endl;
				cbsSpecs[Index].Life--;
				if (cbsSpecs[Index].Life == 0)
					UpdateLeftSide(loc);
			}

		}
		cout << " out" << endl;

	}

	// This function handles the collisions between the ball
	// and the bottom side of the cube grid
	void BottomSideCollision(float r, vec3 pos, vec3 &speed)
	{
		// find the nearest index depending on the ball yx- coordinate
		int x = (pos.x / s.x) + NumxCubes;
		// find the nearest index depending on the ball z - coordinate
		int z = (pos.z / s.z);
		cout << " Bottom side collision ";
		int loc[2] = { x,z };
		int Index = BottomSide[x][z].x;
		cout << Index;
		// reduce the brick life
		if (Index >= 0)
		{
			int zn = Index / (2 * NumxCubes + 1) / (2 * NumyCubes + 1);
			int yn = (Index / (2 * NumxCubes + 1)) % (2 * NumyCubes + 1);
			int xn = Index % (2 * NumxCubes + 1);
			cout << " =+==+ " << xn << " " << yn << " " << zn;
			if (DeletedElement[zn][yn][xn] == false)
			{
				cout << " life " << cbsSpecs[Index].Life << endl;
				cbsSpecs[Index].Life--;
				if (cbsSpecs[Index].Life == 0)
					UpdateBottomSide(loc);
			}

		}

		cout << " out" << endl;
	}

public:

//	**********	V A R I A B L E S	**********

	int NumxCubes;
	int NumyCubes;
	int NumzCubes;
	
	// scale factor that will be applied to every cube
	vec3 s = vec3(0.4);

	// Vector that stores if the Element in this position has been deleted
	vector<vector<vector<bool> > > DeletedElement;

	// struct used to store the Type and level of every cube in the game 
	struct CubeSpecs
	{
		// cube center position
		vec3 Pos;
		// transformation matrix
		mat4 M;
		// cube type
		int Type;
		// cube remaining life
		int Life;
		// cube smoothness
		int Smoothness;
	};

	vector<CubeSpecs> cbsSpecs;

	// Bounding Box Limits in x, y, z axes
	// minx, maxx, miny, maxy, minz, maxz
	float BoxLimits[6] = { 0 };

//	**********	F U N C T I O N S	**********

	//	This function generates the position for every brick in the game. 
	void CreateGame(vector<Cube> cbs)
	{
		mat4 tr1 = translate(mat4(), vec3(0, 0, 0.5));
		mat4 S = scale(mat4(), s);

		int cnt = 0;
		for (int z = 0; z < NumzCubes; z++)
		{
			vector<vector<bool> > tmp0;
			for (int y = -NumyCubes; y <= NumyCubes; y++)
			{
				vector<bool> tmp1;
				vector<vec2> tmp;
				for (int x = -NumxCubes; x <= NumxCubes; x++)
				{
					// choose the type of brick
					int randCube = rand() % (cbs.size());

					CubeSpecs cbsp;
					mat4 M = translate(mat4(), vec3(x * s.x, y * s.y, z * s.z)) * S * tr1;
					cbsp.M = M;
					cbsp.Type = randCube;
					cbsp.Pos = vec3(x * s.x, y * s.y, z * s.z);
					cbsp.Life = cbs[randCube].Life;
					cbsp.Smoothness = cbs[randCube].Smoothness;
					cbsSpecs.push_back(cbsp);
					if (z == NumzCubes - 1)
						tmp.push_back(vec2(cnt, z * s.z));
					cnt++;
					tmp1.push_back(false);
				}
				if (z == NumzCubes - 1)
					FrontSide.push_back(tmp);
				tmp0.push_back(tmp1);
			}
			DeletedElement.push_back(tmp0);
		}
		FrontSideLimit = FindMaxCoordinate(FrontSide);
		CreateSides();
		cout << " cubes created " << endl;
	}

	/***********************************************************************************/

	// This function is used to Test the removal of a brick after it's destroyed
	void RemoveBrick(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		{
			float randx = (2 * (rand() % (NumxCubes + 1)) - NumxCubes) * s.x;
			float randy = (2 * (rand() % (NumyCubes + 1)) - NumyCubes) * s.y;
			int loc[2] = { randx, randy };
			FrontSideCollision(0.4, vec3(randx, randy, 0), vec3(0));
			//UpdateRightSide(loc);
			int f_n = 0;
			for (int k = 0; k < (NumzCubes); k++)
			{
				for (int j = 0; j < (2 * NumyCubes + 1); j++)
				{
					for (int i = 0; i < (2 * NumxCubes + 1); i++)
					{
						if (DeletedElement[k][j][i] == false)
							f_n++;
					}
				}
			}
			cout << f_n << endl;
		//	cout << loc[0] << " " << loc[1] << endl;
		//	UpdateFrontSide(loc);
		}

		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		{
			float randy = (2 * (rand() % (NumyCubes + 1)) - NumyCubes) * s.y;
			float randz = (rand() % (NumzCubes)) * s.z;
			float loc[2] = { randy, randz };
		//	cout << " rand " <<  loc[0] << " " << loc[1] << endl;
			RightSideCollision(0.4, vec3(0, randy, randz), vec3(0));
			//UpdateRightSide(loc);
			int f_n = 0;
			for (int k = 0; k < (NumzCubes); k++)
			{
				for (int j = 0; j < (2 * NumyCubes + 1); j++)
				{
					for (int i = 0; i < (2 * NumxCubes + 1); i++)
					{
						if (DeletedElement[k][j][i] == false)
							f_n++;
					}
				}
			}
			cout << f_n << endl;
		}

		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		{
			float randx = (2 * (rand() % (NumxCubes + 1)) - NumxCubes) * s.x;
			float randz = rand() % (NumzCubes) * s.z;
			float loc[2] = { randx, randz };
			//cout << " rand " <<  loc[0] << " " << loc[1] << endl;
			TopSideCollision(0.4, vec3(randx, 0, randz), vec3(0));
		//	cout << " rand " <<  loc[0] << " " << loc[1] << endl;
		//	UpdateTopSide(loc);
			int f_n = 0;
			for (int k = 0; k < (NumzCubes); k++)
			{
				for (int j = 0; j < (2 * NumyCubes + 1); j++)
				{
					for (int i = 0; i < (2 * NumxCubes + 1); i++)
					{
						if (DeletedElement[k][j][i] == false)
							f_n++;
					}
				}
			}
			cout << f_n << endl;
		}

		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		{
			float randy = (2 * (rand() % (NumyCubes + 1)) - NumyCubes) * s.y;
			float randz = (rand() % (NumzCubes)) * s.z;
			float loc[2] = { randy, randz };
			//	cout << " rand " <<  loc[0] << " " << loc[1] << endl;
			LeftSideCollision(0.4, vec3(0, randy, randz), vec3(0));
			//UpdateRightSide(loc);
			int f_n = 0;
			for (int k = 0; k < (NumzCubes); k++)
			{
				for (int j = 0; j < (2 * NumyCubes + 1); j++)
				{
					for (int i = 0; i < (2 * NumxCubes + 1); i++)
					{
						if (DeletedElement[k][j][i] == false)
							f_n++;
					}
				}
			}
			cout << f_n << endl;
		}

		if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		{
			float randx = (2 * (rand() % (NumxCubes + 1)) - NumxCubes) * s.x;
			float randz = rand() % (NumzCubes)* s.z;
			float loc[2] = { randx, randz };
			//cout << " rand " <<  loc[0] << " " << loc[1] << endl;
			BottomSideCollision(0.4, vec3(randx, 0, randz), vec3(0));
			//	cout << " rand " <<  loc[0] << " " << loc[1] << endl;
			//	UpdateTopSide(loc);
			int f_n = 0;
			for (int k = 0; k < (NumzCubes); k++)
			{
				for (int j = 0; j < (2 * NumyCubes + 1); j++)
				{
					for (int i = 0; i < (2 * NumxCubes + 1); i++)
					{
						if (DeletedElement[k][j][i] == false)
							f_n++;
					}
				}
			}
			cout << f_n << endl;
		}
	}

	/***********************************************************************************/

	// This function checks if a ball collided with the  
	// bounding box and if so it computes the new speed
	// returns true if the ball collides with the rear 
	// wall and then the game is over
	bool BallBoxCollision(float r, vec3 pos, vec3 &speed)
	{
		//cout << pos.z << " " << BoxLimits[5] << " " << speed.x << " " << speed.y << " " << speed.z << endl;
		bool EndGame = false;

		if (pos.x + r >= BoxLimits[1])			// collision with the right wall
			speed = reflect(vec3(-1.0f, 0.0f, 0.0f), speed);
		else if ((pos.x - r <= BoxLimits[0]))	// collision with the left wall
			speed = reflect(vec3(1.0f, 0.0f, 0.0f), speed);
		else if ((pos.y + r >= BoxLimits[3]))	// collision with the top wall
			speed = reflect(vec3(0.0f, -1.0f, 0.0f), speed);
		else if ((pos.y - r <= BoxLimits[2]))	// collision with the bottom wall
			speed = reflect(vec3(0.0f, 1.0f, 0.0f), speed);
		else if ((pos.z - r <= BoxLimits[4]))	// collision with the front wall
			speed = reflect(vec3(0.0f, 0.0f, 1.0f), speed);
		else if ((pos.z > 0.8 * BoxLimits[5]))		// collision with the wall behind
			EndGame = true;

		return EndGame;
	}

	// This function checks if a ball collided with the  
	// board and if so it computes the new speed
	void BallBoardCollision(float r, vec3 pos, vec3 &speed, float boardLimits[6], vec3 boardNorm)
	{
	//	cout << pos.x + r << " " << boardLimits[0] << " - " << boardLimits[1] << " y " << pos.y + r << " " << boardLimits[2] << " - " << boardLimits[3] << " z " << pos.z + r << " " << boardLimits[4] << " - " << boardLimits[5] << endl;

		bool x_axis_check = (pos.x - r <= boardLimits[1]) && (pos.x + r >= boardLimits[0]);
		bool y_axis_check = (pos.y - r <= boardLimits[3]) && (pos.y + r >= boardLimits[2]);
		bool z_axis_check = (boardLimits[4] == boardLimits[5]) ? (pos.z + r >= boardLimits[4] && pos.z - r <= boardLimits[4] ) : (pos.z + r <= boardLimits[5]) && (pos.z - r >= boardLimits[4]);
		
		if ( x_axis_check && y_axis_check && z_axis_check)
			speed = -speed;
	}

	// This function handles the collisions between the cubes and the ball
	void BallCubeGridCollision(float r, vec3 pos, vec3 &speed)
	{
		/*
		float BallSidesDistance[5];
		BallSidesDistance[0] = abs(RightSideLimit + 0.5 * s.x + r - pos.x);
		BallSidesDistance[1] = abs(TopSideLimit + 0.5 * s.y + r - pos.y);
		BallSidesDistance[2] = abs(-LeftSideLimit - 0.5 * s.x - r + pos.x);
		BallSidesDistance[3] = abs(-BottomSideLimit - 0.5 * s.y - r - pos.y);
		BallSidesDistance[4] = abs(FrontSideLimit + 0.5 * s.z + r - pos.z);
		
		// find min Distance index to choose the collision function
		float m = BallSidesDistance[0];
		int mi = 0;
		for (int i = 0; i < 5; i++)
		{
			if (BallSidesDistance[i] < m)
			{
				mi = i;
				m = BallSidesDistance[i];
			}
		}
		cout << "min dist Index " << mi << endl;
		cout << BallSidesDistance[0] << " " << BallSidesDistance[1] << " " << BallSidesDistance[2] << " " << BallSidesDistance[3] << " " << BallSidesDistance[4] << endl;
		// The ball will collide with the right Side
		if (mi == 5)
			RightSideCollision(r, pos, speed);
		*/

		int CollisionSide = -1;

	//	cout << pos.x << " " << RightSideLimit + 0.5 * s.x << " " << -LeftSideLimit - 0.5 * s.x << " " << pos.y << " " << TopSideLimit + 0.5 * s.y << " " << -BottomSideLimit - 0.5 * s.y << " " << pos.z << " " << FrontSideLimit + s.z << endl;
		//cout << FrontSideLimit << endl;
		
		// Check if the ball collided with the F R O N T side
		if ((pos.x < RightSideLimit + 0.5 * s.x) && (pos.x > -LeftSideLimit - 0.5 * s.x) &&
			(pos.y < TopSideLimit + 0.5 * s.y) && (pos.y > -BottomSideLimit - 0.5 * s.y) &&
			(FrontSideLimit + s.z + r - pos.z > 0) &&
			(speed.z < 0))
		{
			// find the nearest index depending on the ball x - coordinate
			int x = (pos.x / s.x) + NumxCubes;
			// find the nearest index depending on the ball y - coordinate
			int y = (pos.y / s.y) + NumyCubes;
			int Index = FrontSide[x][y].x;
			if ((FrontSide[x][y].y + s.z + r - pos.z > 0) && (cbsSpecs[Index].Life > 0) )
			{
				cout << speed.x << " " << speed.y << " " << speed.z << " " << endl;
				FrontSideCollision(r, pos, speed);
				cout << speed.x << " " << speed.y << " " << speed.z << " " << endl;;
			}
			
		}

		// check if the box collided with the R I G H T side
		if ((pos.z < FrontSideLimit + s.z) &&
			(pos.y < TopSideLimit + 0.5 * s.y) && (pos.y > -BottomSideLimit - 0.5 * s.y) &&
			(RightSideLimit + 0.5 * s.x + r - pos.x > 0) && (pos.x > RightSideLimit + 0.5 * s.x))
			RightSideCollision(r, pos, speed);

		// check if the box collided with the T O P side
		if ((pos.z < FrontSideLimit + s.z) &&
			(pos.x < RightSideLimit + 0.5 * s.x) && (pos.x > -LeftSideLimit - 0.5 * s.x) &&
			(TopSideLimit + 0.5 * s.y + r - pos.y > 0) &&
			(speed.y < 0))
		{
			// find the nearest index depending on the ball x - coordinate
			int x = (pos.x / s.x) + NumxCubes;
			// find the nearest index depending on the ball z - coordinate
			int z = (pos.z / s.z);
			int Index = TopSide[x][z].x;
			if ((TopSide[x][z].y + 0.5 * s.y + r - pos.y > 0) && (cbsSpecs[Index].Life > 0))
			{
				cout << speed.x << " " << speed.y << " " << speed.z << " " << endl;
				TopSideCollision(r, pos, speed);
				cout << speed.x << " " << speed.y << " " << speed.z << " " << endl;;
			}

		}
		// check if the box collided with the left side
		if ((pos.z < FrontSideLimit + s.z) &&
			(pos.y < TopSideLimit + 0.5 * s.y) && (pos.y > -BottomSideLimit - 0.5 * s.y) &&
			(LeftSideLimit + 0.5 * s.x + r + pos.x > 0) && (-pos.x > LeftSideLimit + 0.5 * s.x))
			LeftSideCollision(r, pos, speed);

		// check if the box collided with the Bottom side
		if ((pos.z < FrontSideLimit + s.z) &&
			(pos.x < RightSideLimit + 0.5 * s.x) && (pos.x > -LeftSideLimit - 0.5 * s.x) &&
			(BottomSideLimit + s.y + r + 0.5 * pos.y > 0) && (-pos.y > BottomSideLimit + 0.5 * s.y))
			BottomSideCollision(r, pos, speed);

		/*if (CollisionSide >= 0)
			cout << "collision side -> " << CollisionSide << endl;*/

	}
};
