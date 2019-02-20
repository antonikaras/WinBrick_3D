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

	//	**********	F U N C T I O N S	**********

	/******************************************************************************/

	// This Function finds the min or max element per side
	int FindMinIndex(float Dist[9])
	{
		float m = 1000.0f;
		int mi = -1;
		
		for (int i = 0; i < 9; i++)
		{
			//cout << "Min i " << Dist[i] << endl;
			if (m > Dist[i])
			{
				m = Dist[i];
				mi = i;
			}
		}

		return mi;
	}

	// This function computes the new speed after the collision
	vec3 NewSpeed(vec3 sp, vec3 BallPos, vec3 CubePos)
	{
		vec3 newsp = -sp;
		vec3 dir = normalize((BallPos - CubePos));
		float angles[9] = { 400.0f,400.0f, 400.0f, 400.0f, 400.0f, 400.0f, 400.0f, 400.0f, 400.0f };
		// compute the dot product between dir and the normal of every side
		
		angles[0] = degrees(acos(dot(dir, vec3(0 , 0 , 1)))) ;	// front side
		angles[1] = degrees(acos(dot(dir, vec3(1 , 0 , 0)))) ;	// right side
		angles[2] = degrees(acos(dot(dir, vec3(0 , 0 , -1))));	// back side
		angles[3] = degrees(acos(dot(dir, vec3(-1, 0 , 0)))) ;	// left side
		angles[4] = degrees(acos(dot(dir, vec3(0 , 1 , 0)))) ;	// top side
		angles[5] = degrees(acos(dot(dir, vec3(0 , -1, 0)))) ;	// bottom side

		int mi = FindMinIndex(angles);
		//cout << " collision side " << mi << endl;
		if (mi == 0) 
			newsp = reflect(sp, vec3(0,  0, 1));	// front side collision
		else if (mi == 1) 
			newsp = reflect(sp, vec3(1,  0, 0));	// right side collision
		else if (mi == 2) 
			newsp = reflect(sp, vec3(0,  0, -1));	// back side collision
		else if (mi == 3) 
			newsp = reflect(sp, vec3(-1, 0, 0));	// left side collision
		else if (mi == 4) 
			newsp = reflect(sp, vec3(0,  1, 0));	// top side collision
		else if (mi == 5) 
			newsp = reflect(sp, vec3(0, -1, 0));	// bottom side collision
		
		return newsp;
	}


public:

	//	**********	V A R I A B L E S	**********

	int NumxCubes = 2;
	int NumyCubes = 2;
	int NumzCubes = 4;

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
		float Life;
		// cube smoothness
		int Smoothness;
		// cube extra
		int ExtraID = -1;
	};

	vector<CubeSpecs> cbsSpecs;

	// Bounding Box Limits in x, y, z axes
	// minx, maxx, miny, maxy, minz, maxz
	float BoxLimits[6] = { 0 };

	// Number of Collisions between the ball and the cubes
	// used to compute the score
	int Collisions = 0;
	//
	int NoBrickCollissions = 0;
	//	**********	F U N C T I O N S	**********

	//	This function generates the position for every brick in the game. 
	void CreateGame(vector<Cube> cbs)
	{
		mat4 tr1 = translate(mat4(), vec3(0, 0, 0.5));
		mat4 S = scale(mat4(), s);
	//	cout << 2 * NumxCubes + 1 << " " << 2 * NumyCubes + 1 << " " << NumzCubes << endl;
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
					cbsp.Pos = vec3(x * s.x, y * s.y, z * s.z) + 0.5f * vec3(0, 0, s.z);
					cbsp.Life = cbs[randCube].Life;
					cbsp.Smoothness = cbs[randCube].Smoothness;
					cbsSpecs.push_back(cbsp);
					if (z == NumzCubes - 1)
						tmp.push_back(vec2(cnt, z * s.z));
					cnt++;
					tmp1.push_back(false);
				}
				tmp0.push_back(tmp1);
			}
			DeletedElement.push_back(tmp0);
		}

		// Random choose extra location
		int MaxExtras = 0.2 * cbsSpecs.size();
		vector<int> ExtrasOldPos;
		while (ExtrasOldPos.size() < MaxExtras)
		{
			int ExtraPos = rand() % cbsSpecs.size();
			bool ExtraPosExists = false;
			for (int i = 0; i < ExtrasOldPos.size(); i++)
			{
				if (ExtraPos == ExtrasOldPos[i])
					ExtraPosExists = true;
			}
			if (!ExtraPosExists)
			{
				ExtrasOldPos.push_back(ExtraPos);
				int ExtraID = rand() % 5;
				cbsSpecs[ExtraPos].ExtraID = ExtraID;
			}

		}
	//	cout << " cubes created " << endl;
	//	cout << DeletedElement[0][0].size() << " " << DeletedElement[0].size() << " " << DeletedElement.size() << endl;
	//	cout << cbsSpecs.size() << endl;
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
		
		if ((0.5 * pos.x < BoxLimits[0]) || (0.5 * pos.x > BoxLimits[1]) ||
			(0.5 * pos.y < BoxLimits[2]) || (0.5 * pos.y > BoxLimits[3]) ||
			(0.5 * pos.z + r < BoxLimits[4]) || (0.5 * pos.z > BoxLimits[5]))
		{
			EndGame = true;
			cout << BoxLimits[0] << " " << BoxLimits[2] << " " << BoxLimits[5] << endl;
			cout << pos.x << " " << pos.y << " " << pos.y << endl;
		}
		else if (pos.x + r >= BoxLimits[1])			// collision with the right wall
		{
			speed = reflect(speed, vec3(-1.0f, 0.0f, 0.0f));
			NoBrickCollissions++;
		}
		else if ((pos.x - r <= BoxLimits[0]))	// collision with the left wall
		{
			speed = reflect(speed, vec3(1.0f, 0.0f, 0.0f));
			NoBrickCollissions++;
		}
		else if ((pos.y + r >= BoxLimits[3]))	// collision with the top wall
		{
			speed = reflect(speed, vec3(0.0f, -1.0f, 0.0f));
			NoBrickCollissions++;
		}
		else if ((pos.y - r <= BoxLimits[2]))	// collision with the bottom wall
		{
			speed = reflect(speed, vec3(0.0f, 1.0f, 0.0f));
			NoBrickCollissions++;
		}
		else if ((pos.z - 1.5 * r <= BoxLimits[4]))	// collision with the front wall
		{
			speed = reflect(speed, vec3(0.0f, 0.0f, 1.0f));
			NoBrickCollissions++;
		}
		else if ((pos.z > 0.7 * BoxLimits[5]))	// collision with the wall behind
			EndGame = true;
		/*if (NoBrickCollissions > 20)
		{
			EndGame = true;
		}*/
		speed = normalize(speed);
		return EndGame;
	}

	// This function checks if a ball collided with the  
	// board and if so it computes the new speed
	void BallBoardCollision(float r, vec3 pos, vec3 &speed, float boardLimits[6], vec3 boardNorm, bool StickyBoard, int reflectType)
	{
		//	cout << pos.x + r << " " << boardLimits[0] << " - " << boardLimits[1] << " y " << pos.y + r << " " << boardLimits[2] << " - " << boardLimits[3] << " z " << pos.z + r << " " << boardLimits[4] << " - " << boardLimits[5] << endl;

		bool x_axis_check = (pos.x - r <= boardLimits[1]) && (pos.x + r >= boardLimits[0]);
		bool y_axis_check = (pos.y - r <= boardLimits[3]) && (pos.y + r >= boardLimits[2]);
		bool z_axis_check = (boardLimits[4] == boardLimits[5]) ? (pos.z + r >= boardLimits[4] && pos.z - r <= boardLimits[4]) : (pos.z + r <= boardLimits[5]) && (pos.z - r >= boardLimits[4]);

		if (x_axis_check && y_axis_check && z_axis_check)
		{
			if (!StickyBoard)
			{
				if (reflectType == 1)
					speed = -speed;
				else if (reflectType == 2)
					speed = reflect(speed, boardNorm);
				else if (reflectType == 3)
				{
					// convert to a sphere like coordinate system
					float x_pos = (pos.x + abs(boardLimits[0])) / (abs(boardLimits[0]) + abs(boardLimits[1]));
					float y_pos = (pos.y + abs(boardLimits[2])) / (abs(boardLimits[2]) + abs(boardLimits[3]));
					float th = y_pos * 3.14f;
					float phi = x_pos * 2 * 3.14f;
					speed = normalize(vec3(sin(th) * cos(phi), sin(th) * sin(phi), cos(th)));
				}
				NoBrickCollissions++;
			}
			else
				speed = vec3(0);
		}
	}

	// This function handles the collisions between the cubes and the ball
	void BallCubeGridCollision(float r, vec3 &pos, vec3 &speed)
	{
		if ((pos.x - r < (NumxCubes + 0.5) * s.x) && (pos.x + r > -(NumxCubes + 0.5) * s.x) &&
			(pos.y - r < (NumyCubes + 0.5) * s.y) && (pos.y + r > -(NumyCubes + 0.5) * s.y) &&
			(pos.z - r < (NumzCubes)* s.z))
		{
			// find the nearest index depending on the ball x - coordinate
			int x = (abs(pos.x) / s.x) + NumxCubes;
			// find the nearest index depending on the ball y - coordinate
			int y = (abs(pos.y) / s.y) + NumyCubes;
			// find the nearest index depending on the ball z - coordinate
			int z = ((pos.z - r) / s.z);

			// use pythagorean theory to calculate the radius of the surrounding sphere
			float Cube_r = 0.5 * (sqrt(2 * 0.25 * s.x * s.x + 0.25 * s.y *s.y) + 0.5 * s.x); 

			float Dist[9] = { 10000.0f, 10000.0f, 10000.0f, 10000.0f, 10000.0f, 10000.0f, 10000.0f, 10000.0f, 10000.0f };
			int DistInd[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0 };
			// Center Cube
			if (DeletedElement[z][y][x] == false)
			{
				int Index = z * (2 * NumxCubes + 1) * (2 * NumyCubes + 1) + y * (2 * NumxCubes + 1) + x;
				//cout << "Center Cube Index " << Index << " Life " << cbsSpecs[Index].Life << " distance " << distance(cbsSpecs[Index].Pos, pos) << endl;
				// check if the ball is too close to the Center cube
				if (distance(cbsSpecs[Index].Pos, pos) < Cube_r + r)
				{
					Dist[0] = distance(cbsSpecs[Index].Pos, pos);
					DistInd[0] = Index;
				}
			}
			// right cube
			if (x < (2 * NumxCubes))	
				{
					if (DeletedElement[z][y][x + 1] == false)
					{
						int Index = z * (2 * NumxCubes + 1) * (2 * NumyCubes + 1) + y * (2 * NumxCubes + 1) + x + 1;
						//cout << "Right Cube Index " << Index << " Life " << cbsSpecs[Index].Life << " distance " << distance(cbsSpecs[Index].Pos, pos) << endl;
						// check if the ball is too close to the right cube
						if (distance(cbsSpecs[Index].Pos, pos) < Cube_r + r)
						{
							Dist[1] = distance(cbsSpecs[Index].Pos, pos);
							DistInd[1] = Index;
						}
					}
				}
			// right Top cube
			if ((x < (2 * NumxCubes)) && (y < (2 * NumyCubes)))	
			{
				if (DeletedElement[z][y + 1][x + 1] == false)
				{
					int Index = z * (2 * NumxCubes + 1) * (2 * NumyCubes + 1) + (y + 1) * (2 * NumxCubes + 1) + x + 1;
					//cout << "Right Top Cube Index " << Index << " Life " << cbsSpecs[Index].Life << " distance " << distance(cbsSpecs[Index].Pos, pos) << endl;
					// check if the ball is too close to the right Top cube
					if (distance(cbsSpecs[Index].Pos, pos) < Cube_r + r)
					{
						Dist[2] = distance(cbsSpecs[Index].Pos, pos);
						DistInd[2] = Index;
					}
				}
			}
			// Top cube
			if (y < (2 * NumyCubes))	
			{
				if (DeletedElement[z][y + 1][x] == false)
				{
					int Index = z * (2 * NumxCubes + 1) * (2 * NumyCubes + 1) + (y + 1) * (2 * NumxCubes + 1) + x;
					//cout << "Top Cube Index " << Index << " Life " << cbsSpecs[Index].Life << " distance " << distance(cbsSpecs[Index].Pos, pos) << endl;
					// check if the ball is too close to the Top cube
					if (distance(cbsSpecs[Index].Pos, pos) < Cube_r + r)
					{
						Dist[3] = distance(cbsSpecs[Index].Pos, pos);
						DistInd[3] = Index;
					}
				}
			}
			// left Top cube
			if ((x > 0)	&& (y < (2 * NumyCubes)))	
			{
				if (DeletedElement[z][y + 1][x - 1] == false)
				{
					int Index = z * (2 * NumxCubes + 1) * (2 * NumyCubes + 1) +(y + 1) * (2 * NumxCubes + 1) + x - 1;
					//cout << "Left Top Cube Index " << Index << " Life " << cbsSpecs[Index].Life << " distance " << distance(cbsSpecs[Index].Pos, pos) << endl;
					// check if the ball is too close to the right cube
					if (distance(cbsSpecs[Index].Pos, pos) < Cube_r + r)
					{
						Dist[4] = distance(cbsSpecs[Index].Pos, pos);
						DistInd[4] = Index;
					}
				}
			}
			// left cube
			if (x > 0)	
			{
				if (DeletedElement[z][y][x - 1] == false)
				{
					int Index = z * (2 * NumxCubes + 1) * (2 * NumyCubes + 1) + (y) * (2 * NumxCubes + 1) + x - 1;
					//cout << "Left Top Cube Index " << Index << " Life " << cbsSpecs[Index].Life << " distance " << distance(cbsSpecs[Index].Pos, pos) << endl;
					// check if the ball is too close to the left Top cube
					if (distance(cbsSpecs[Index].Pos, pos) < Cube_r + r)
					{
						Dist[5] = distance(cbsSpecs[Index].Pos, pos);
						DistInd[5] = Index;
					}
				}
			}
			// left Bottom cube
			if ((x > 0) && (y > 0))	
			{
				if (DeletedElement[z][y - 1][x - 1] == false)
				{
					int Index = z * (2 * NumxCubes + 1) * (2 * NumyCubes + 1) + (y - 1) * (2 * NumxCubes + 1) + x - 1;
					//cout << "Left Bottom Cube Index " << Index << " Life " << cbsSpecs[Index].Life << " distance " << distance(cbsSpecs[Index].Pos, pos) << endl;
					// check if the ball is too close to the left Bottom cube
					if (distance(cbsSpecs[Index].Pos, pos) < Cube_r + r)
					{
						Dist[6] = distance(cbsSpecs[Index].Pos, pos);
						DistInd[6] = Index;
					}
				}
			}
			// Bottom cube
			if (y > 0)	
			{
				if (DeletedElement[z][y - 1][x] == false)
				{
					int Index = z * (2 * NumxCubes + 1) * (2 * NumyCubes + 1) + (y - 1) * (2 * NumxCubes + 1) + x;
					//cout << "Bottom Cube Index " << Index << " Life " << cbsSpecs[Index].Life << " distance " << distance(cbsSpecs[Index].Pos, pos) << endl;
					// check if the ball is too close to the Bottom cube
					if (distance(cbsSpecs[Index].Pos, pos) < Cube_r + r)
					{
						Dist[7] = distance(cbsSpecs[Index].Pos, pos);
						DistInd[7] = Index;
					}
				}
			}
			// right Bottom cube
			if ((x < (2 * NumxCubes)) && (y > 0))	
			{
				if (DeletedElement[z][y - 1][x + 1] == false)
				{
					int Index = z * (2 * NumxCubes + 1) * (2 * NumyCubes + 1) + (y - 1) * (2 * NumxCubes + 1) + x + 1;
					//cout << "Right Top Cube Index " << Index << " Life " << cbsSpecs[Index].Life << " distance " << distance(cbsSpecs[Index].Pos, pos) << endl;
					// check if the ball is too close to the left Bottom cube
					if (distance(cbsSpecs[Index].Pos, pos) < Cube_r + r)
					{
						Dist[8] = distance(cbsSpecs[Index].Pos, pos);
						DistInd[8] = Index;
					}
				}
			}
			// Detect the collided Cube
			int MinIndex = FindMinIndex(Dist);
			if (MinIndex > -1)
			{
				//cout << " collision cube " << DistInd[MinIndex] << " Life " << cbsSpecs[DistInd[MinIndex]].Life << endl;
				//cout << " ball position before " << pos.x << " " << pos.y << " " << pos.z << " distance " << Dist[MinIndex] << endl;
				//cout << " ball speed  before " << speed.x << " " << speed.y << " " << speed.z << endl;
				MinIndex = DistInd[MinIndex];
				speed = NewSpeed(speed, pos, cbsSpecs[MinIndex].Pos);
				pos += 0.1f * speed;	// change the position so that in the next check won't detect the same collision
				//cout << " ball position after " << pos.x << " " << pos.y << " " << pos.z << " distance " << distance(pos, cbsSpecs[MinIndex].Pos) << endl;
				//cout << " ball speed after " << speed.x << " " << speed.y << " " << speed.z << endl;
				//cout << "--------------------------------" << endl;
				cbsSpecs[MinIndex].Life--;
				if (cbsSpecs[MinIndex].Life == 0)
				{
					int zn = MinIndex / (2 * NumxCubes + 1) / (2 * NumyCubes + 1);
					int yn = (MinIndex / (2 * NumxCubes + 1)) % (2 * NumyCubes + 1);
					int xn = MinIndex % (2 * NumxCubes + 1);
					DeletedElement[zn][yn][xn] = true;
				}
				// update the score
				Collisions++;
				NoBrickCollissions = 0;
			}
		}	
	}

	// This function deletes the old Grid
	void DeleteGrid()
	{
		cbsSpecs.clear();
		DeletedElement.clear();
	}
};
