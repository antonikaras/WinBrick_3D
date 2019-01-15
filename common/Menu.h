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
#include <common/Fonts.h>
#include <common/GameGrid_ver2.h>

using namespace std;
using namespace glm;

WriteText WRT;
GameGrid GG;

class Menu
{
private:
	//	*****	V A R I A B L E S	****

	int UpDown = 0;
	float LeftRight = 0;
	int MainMenuIndex = 1;
	int PlayMenuIndex = 0;
	int PauseMenuIndex = 0;
	// showing which submenu to show
	int MenuIndex = 0;
	// Display the fps on the upper left corner
	bool ShowFps = true;
	bool Left = false;
	bool Right = false;
	//	****	F U N C T I O N S	****
	
	void DrawMainMenu()
	{
		MainMenuIndex = UpDown;
		if (MainMenuIndex > 4)
			MainMenuIndex = 4;
		if (MainMenuIndex < 1)
			MainMenuIndex = 1;
		UpDown = MainMenuIndex;
		if (MainMenuIndex == 1)
			WRT.Write("PLAY", vec3(-0.5, 1, 1), 1.0);
		else
			WRT.Write("PLAY", vec3(-0.5, 1, 0), 1.0);
		if (MainMenuIndex == 2)
			WRT.Write("LEADERBORD", vec3(-0.5, 0.5, 1.0), 1.0);
		else
			WRT.Write("LEADERBORD", vec3(-0.5, 0.5, 0.0), 1.0);
		if (MainMenuIndex ==3)
			WRT.Write("SETTINGS", vec3(-0.5, 0, 1.0), 1.0);
		else
			WRT.Write("SETTINGS", vec3(-0.5, 0, 0), 1.0);
		if (MainMenuIndex == 4)
			WRT.Write("EXIT", vec3(-0.5, -0.5, 1.0), 1.0);
		else
			WRT.Write("EXIT", vec3(-0.5, -0.5, 0), 1.0);
	}

	void DrawPlayMenu()
	{
		PlayMenuIndex = UpDown;
		if (PlayMenuIndex > 4)
			PlayMenuIndex = 4;
		if (PlayMenuIndex < 0)
			PlayMenuIndex = 0;
		UpDown = PlayMenuIndex;
		// first submenu option
		string Option1 = "LEVEL";
		// second submenu option
		string Option2 = "NUMBER OF X-CUBES - ";
		if ((PlayMenuIndex == 1) && (Left) && (GG.NumxCubes > 1))
		{
			GG.NumxCubes--;
			Left = false;
		}
		else if ((PlayMenuIndex == 1) && (Right) && (GG.NumxCubes < 49))
		{
			GG.NumxCubes++;
			Right = false;
		}
		if (GG.NumxCubes < 5)
			Option2.push_back(2 * GG.NumxCubes + 1 + 48);
		else
		{
			int tmp = 2 * GG.NumxCubes + 1;
			Option2.push_back(tmp / 10 + 48);
			Option2.push_back(tmp % 10 + 48);
		}
		// third submenu option
		string Option3 = "NUMBER OF Y-CUBES - ";
		if ((PlayMenuIndex == 2) && (Left) && (GG.NumyCubes > 1))
		{
			GG.NumyCubes--;
			Left = false;
		}
		else if ((PlayMenuIndex == 2) && (Right) && (GG.NumyCubes < 49))
		{
			GG.NumyCubes++;
			Right = false;
		}
		if (GG.NumyCubes < 5)
			Option3.push_back(2 * GG.NumyCubes + 1 + 48);
		else
		{
			int tmp = 2 * GG.NumyCubes + 1;
			Option3.push_back(tmp / 10 + 48);
			Option3.push_back(tmp % 10 + 48);
		}
		// forth submenu option
		if ((PlayMenuIndex == 3) && (Left) && (GG.NumzCubes > 1))
		{
			GG.NumzCubes--;
			Left = false;
		}
		else if ((PlayMenuIndex == 3) && (Right) && (GG.NumzCubes < 99))
		{
			GG.NumzCubes++;
			Right = false;
		}
		string Option4 = "NUMBER OF Z-CUBES - ";
		if (GG.NumzCubes < 10)
			Option4.push_back(GG.NumzCubes + 48);
		else
		{
			Option4.push_back(GG.NumzCubes / 10 + 48);
			Option4.push_back(GG.NumzCubes % 10 + 48);
		}
		// fifth submenu option
		string Option5 = "BALL SPEED";
		if (PlayMenuIndex == 0)
			WRT.Write(Option1.c_str(), vec3(-0.5, 1, 1), 0.8);
		else
			WRT.Write(Option1.c_str(), vec3(-0.5, 1, 0), 0.8);
		if (PlayMenuIndex == 1)
			WRT.Write(Option2.c_str(), vec3(-1.5, 0.5, 1), 0.8);
		else
			WRT.Write(Option2.c_str(), vec3(-1.5, 0.5, 0), 0.8);
		if (PlayMenuIndex == 2)
			WRT.Write(Option3.c_str(), vec3(-1.5, 0, 1), 0.8);
		else
			WRT.Write(Option3.c_str(), vec3(-1.5, 0, 0), 0.8); 
		if (PlayMenuIndex == 3)
			WRT.Write(Option4.c_str(), vec3(-1.5, -0.5, 1), 0.8);
		else
			WRT.Write(Option4.c_str(), vec3(-1.5, -0.5, 0), 0.8);
		if (PlayMenuIndex == 4)
			WRT.Write(Option5.c_str(), vec3(-0.5, -1, 1), 0.8);
		else
			WRT.Write(Option5.c_str(), vec3(-0.5, -1, 0), 0.8);

	}

	void  DrawExit()
	{
		WRT.Write("Are you sure you", vec3(-0.5, 0.5, GG.NumzCubes), 0.5);
		WRT.Write("want to exit ?", vec3(-0.5, 0.25, GG.NumzCubes), 0.5);
		WRT.Write("Press Enter to exit", vec3(-0.5, 0.0, GG.NumzCubes), 0.25);
		WRT.Write("Press Backspace to return", vec3(-0.5, -0.25, GG.NumzCubes), 0.25);
	}
	
	void DrawPauseMenu()
	{
		PauseMenuIndex = UpDown;
		if (PauseMenuIndex > 2)
			PauseMenuIndex = 2;
		if (PauseMenuIndex < 0)
			PauseMenuIndex = 0;
		UpDown = PauseMenuIndex;
		if (PauseMenuIndex == 0)
			WRT.Write("Resume", vec3(0.0, 0.25, GG.NumzCubes + 1), 0.1);
		else
			WRT.Write("Resume", vec3(0.0, 0.25, GG.NumzCubes), 0.1);
		if (PauseMenuIndex == 1)
			WRT.Write("SETTINGS", vec3(0.0, 0, GG.NumzCubes + 1), 0.1);
		else
			WRT.Write("SETTINGS", vec3(0.0, 0, GG.NumzCubes), 0.1);
		if (PauseMenuIndex == 2)
			WRT.Write("EXIT", vec3(0.0, -0.25, GG.NumzCubes + 1), 0.1);
		else
			WRT.Write("EXIT", vec3(0.0, -0.25, GG.NumzCubes), 0.1);
	}
public:
	//	*****	V A R I A B L E S	****
	
	//
	bool Exit = false;
	bool EnterPressed = false;
	// variables that indicates if the game started
	bool StartGame = false;
	// variable that indicates if the game finished
	bool EndGame = false;
	//  variable that indicates if the game paused
	bool PauseGame = false;
	// vector used to store the basic cubes
	vector<Cube> cbs;
	//	****	F U N C T I O N S	****

	void MenuMovment(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		// Move up
		if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS))
			UpDown --;

		// Move down
		if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS))
			UpDown++;

		// Move Right
		if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS))
			Right = true;

		// Move left
		if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS))
			Left = true;

		// change Menu
		if ((glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS))
		{ 
			UpDown = 0;
			if ((MenuIndex == 1) && ((EnterPressed == false)))
			{
				GG.CreateGame(cbs);
				StartGame = true;
			}
			if ((MenuIndex == 4) && (EnterPressed == false))
				Exit = true;
			if ((MenuIndex == 5) && (EnterPressed == false) && (PauseMenuIndex == 0))
				PauseGame = false;
			if ((MenuIndex == 5) && (EnterPressed == false) && (PauseMenuIndex == 2))
				MenuIndex = 4;
			if (MenuIndex == 0)
				MenuIndex = MainMenuIndex;
			EnterPressed = true;
		}
		if ((glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE))
			EnterPressed = false;
		
		// return to previus menu
		if ((glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS))
		{
			if (PauseGame)
				MenuIndex = 5;
			else if (MenuIndex > 0)
				MenuIndex = 0;
		}

		// pause the game and draw PauseMenu
		if ((glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) && (StartGame))
		{
			UpDown = 0;
			PauseGame = true;
			MenuIndex = 5;
		}
	}

	void Draw(int fps)
	{

		if (ShowFps)
		{
			string fps_c;
			fps_c.push_back(((fps / 10 + 48)));
			fps_c.push_back(((fps % 10 + 48)));
			WRT.Write(fps_c.c_str(), vec3(-1.5, 1.5, 0), 0.5);
		}
		if (MenuIndex == 0)

			DrawMainMenu();
		else if (MenuIndex == 1)
			DrawPlayMenu();
		else if (MenuIndex == 4)
			DrawExit();
		else if (MenuIndex == 5)
			DrawPauseMenu();

	}
};