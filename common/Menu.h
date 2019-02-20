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
	int SettingsMenuIndex = 0;
	// showing which submenu to show
	int MenuIndex = 0;
	bool Left = false;
	bool Right = false;
	enum MenuNames
	{
		Main = 0,Play,LeaderBoards,Options,Exit,Pause,GameOver
	};
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
		string Option1 = "Score Multiplier - ";
		if (ScoreMultiplier > 9)
		{
			Option1.push_back(ScoreMultiplier / 10 + 48);
			Option1.push_back(ScoreMultiplier % 10 + 48);
		}
		else
			Option1.push_back(ScoreMultiplier + 48);
		// second submenu option
		string Option2 = "NUMBER OF X-CUBES - ";
		if ((PlayMenuIndex == 1) && (Left) && (GG.NumxCubes > 1))
		{
			GG.NumxCubes--;
			Left = false;
		}
		else if ((PlayMenuIndex == 1) && (Right) && (GG.NumxCubes < 11))
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
		else if ((PlayMenuIndex == 3) && (Right) && (GG.NumzCubes < 11))
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
		string Option5 = "BALL SPEED  - ";
		if ((PlayMenuIndex == 4) && (Left) && (BallSpeed > 0.6))
		{
			BallSpeed -= 0.1;
			Left = false;
		}
		else if ((PlayMenuIndex == 4) && (Right) && (BallSpeed < 1.4))
		{
			BallSpeed += 0.1;
			Right = false;
		}
		//std::cout << BallSpeed << endl;
		if (BallSpeed > 1.3)
		{
			Option5.push_back(49);
			Option5.push_back(48);
		}
		else
			Option5.push_back((BallSpeed - 0.4) * 10 + 48);
		if (PlayMenuIndex == 0)
			WRT.Write(Option1.c_str(), vec3(-1.5, 1, 1), 0.8);
		else
			WRT.Write(Option1.c_str(), vec3(-1.5, 1, 0), 0.8);
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

	void DrawSettingsMenu()
	{
		SettingsMenuIndex = UpDown;
		if (SettingsMenuIndex > 6)
			SettingsMenuIndex = 6;
		if (SettingsMenuIndex < 0)
			SettingsMenuIndex = 0;
		UpDown = SettingsMenuIndex;
		// Define options
		if (Left)
		{
			Settings[SettingsMenuIndex] = false;
			Left = false;
		}
		if (Right)
		{
			Settings[SettingsMenuIndex] = true;
			Right = false;
		}
		string option_0 = "Show Fps ";
		option_0.push_back(Settings[ShowFps] + 48);
		string option_1 = "Show Score ";
		option_1.push_back(Settings[ShowScore] + 48);
		string option_2 = "Show Lives ";
		option_2.push_back(Settings[ShowLives] + 48);
		string option_3 = "Enable Mirror ";
		option_3.push_back(Settings[EnableMirror] + 48);
		string option_4 = "Enable Ball Board Projection ";
		option_4.push_back(Settings[EnableBallBoardProjection] + 48);
		string option_5 = "Enable Ball Wall Projection ";
		option_5.push_back(Settings[EnableBallWallProjection] + 48);
		string option_6 = "Enable Board Wall Projection ";
		option_6.push_back(Settings[EnableBoardWallProjection] + 48);
		// Print the options
		if (SettingsMenuIndex == ShowFps)
			WRT.Write(option_0.c_str(), vec3(-1.5, 1.5, 1), 0.8);
		else
			WRT.Write(option_0.c_str(), vec3(-1.5, 1.5, 0), 0.8);
		if (SettingsMenuIndex == ShowScore)
			WRT.Write(option_1.c_str(), vec3(-1.5, 1, 1), 0.8);
		else
			WRT.Write(option_1.c_str(), vec3(-1.5, 1, 0), 0.8);
		if (SettingsMenuIndex == ShowLives)
			WRT.Write(option_2.c_str(), vec3(-1.5, 0.5, 1), 0.8);
		else
			WRT.Write(option_2.c_str(), vec3(-1.5, 0.5, 0), 0.8);
		if (SettingsMenuIndex == EnableMirror)
			WRT.Write(option_3.c_str(), vec3(-1.5, 0.0, 1), 0.8);
		else
			WRT.Write(option_3.c_str(), vec3(-1.5, 0.0, 0), 0.8);
		if (SettingsMenuIndex == EnableBallBoardProjection)
			WRT.Write(option_4.c_str(), vec3(-1.5, -0.5, 1), 0.8);
		else
			WRT.Write(option_4.c_str(), vec3(-1.5, -0.5, 0), 0.8);
		if (SettingsMenuIndex == EnableBallWallProjection)
			WRT.Write(option_5.c_str(), vec3(-1.5, -1.0, 1), 0.8);
		else
			WRT.Write(option_5.c_str(), vec3(-1.5, -1.0, 0), 0.8);
		if (SettingsMenuIndex == EnableBoardWallProjection)
			WRT.Write(option_6.c_str(), vec3(-1.5, -1.5, 1), 0.8);
		else
			WRT.Write(option_6.c_str(), vec3(-1.5, -1.5, 0), 0.8);
	}

	void  DrawExit()
	{
		WRT.Write("Are you sure you", vec3(-0.5, 0.5, 0), 0.75);
		WRT.Write("want to exit ?", vec3(-0.5, 0.25, 0), 0.75);
		WRT.Write("Press Enter to exit", vec3(-0.5, 0.0, 0), 0.5);
		WRT.Write("Press Backspace to return", vec3(-0.5, -0.25, 0), 0.5);
	}
	
	void DrawPauseMenu()
	{
		PauseMenuIndex = UpDown;
		if (PauseMenuIndex > 3)
			PauseMenuIndex = 3;
		if (PauseMenuIndex < 0)
			PauseMenuIndex = 0;
		UpDown = PauseMenuIndex;
		if (PauseMenuIndex == 0)
			WRT.Write("Resume", vec3(-0.2, 0.25, GG.NumzCubes + 1), 0.5);
		else
			WRT.Write("Resume", vec3(-0.1, 0.25, GG.NumzCubes), 0.5);
		if (PauseMenuIndex == 1)
			WRT.Write("SETTINGS", vec3(-0.2, 0, GG.NumzCubes + 1), 0.5);
		else
			WRT.Write("SETTINGS", vec3(-0.1, 0, GG.NumzCubes), 0.5);
		if (PauseMenuIndex == 2)
			WRT.Write("START NEW GAME", vec3(-0.2, -0.25, GG.NumzCubes + 1), 0.5);
		else
			WRT.Write("START NEW GAME", vec3(-0.1, -0.25, GG.NumzCubes), 0.5);
		if (PauseMenuIndex == 3)
			WRT.Write("EXIT", vec3(-0.2, -0.5, GG.NumzCubes + 1), 0.5);
		else
			WRT.Write("EXIT", vec3(-0.1, -0.5, GG.NumzCubes), 0.5);
	}

	void GameOverMenu(int score)
	{
		GG.DeleteGrid();
		StartGame = false;
		string score_c = "Final Score ";
		if (score > 99)
		{
			score_c.push_back(((score / 100 + 48)));
			score_c.push_back((((score % 100) / 10 + 48)));
			score_c.push_back(((score % 10 + 48)));
			WRT.Write(score_c.c_str(), vec3(-1, 0.5, 0), 0.8);
		}
		else if (score > 9)
		{
			score_c.push_back(((score / 10 + 48)));
			score_c.push_back(((score % 10 + 48)));
			WRT.Write(score_c.c_str(), vec3(-1, 0.5, 0), 0.8);
		}
		else
		{
			score_c.push_back(score + 48);
			WRT.Write(score_c.c_str(), vec3(-1, 0.5, 0), 0.8);
		}
		WRT.Write("Press Enter to Start new game", vec3(-2, 0.0, 0), 0.6);
		WRT.Write("Press Escape or BackSpace to Exit", vec3(-2, -0.5, 0), 0.6);
		MenuIndex = GameOver;
	}
public:
	//	*****	V A R I A B L E S	****
	enum SetNames
	{
		ShowFps = 0, ShowScore, ShowLives, EnableMirror, EnableBallBoardProjection, EnableBallWallProjection, EnableBoardWallProjection
	};
	
	//
	bool exit = false;
	bool EnterPressed = false;
	// variables that indicates if the game started
	bool StartGame = false;
	// variable that indicates if the game finished
	bool EndGame = false;
	//  variable that indicates if the game paused
	bool PauseGame = false;
	// vector used to store the basic cubes
	vector<Cube> cbs;
	// Ball speed
	float BallSpeed;
	//
	bool RestartGame = false;
	// 
	float BoardLimits[6];
	// Settings Variables
	bool Settings[7] = { true, true, true, true, true, true, true };
	// 
	int ScoreMultiplier = 1;
	/*bool ShowFps = true;					//	Display the fps on the upper left corner of the board
	bool ShowScore = true;					//	Display the score on the upper right corner of the board
	bool EnableMirror = true;				//	Convert the sides of the bounding box to mirrors
	bool EnableBallBoardProjection = true;	//	Show the ball board collision point
	bool EnableBoardWallProjection = true;	//	Display the z - coordinate of the board on the sides of the bounding box
	bool EnableBallWallProjection = true;	//	Display the poisition of the ball in every wall*/
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
			if ((MenuIndex == Play) && (EnterPressed == false) && (!StartGame))
			{
				GG.CreateGame(cbs);
				StartGame = true;
			}
			if (MenuIndex == Main)
				MenuIndex = MainMenuIndex;
			if ((MenuIndex == Exit) && (EnterPressed == false))
				exit = true;
			if ((MenuIndex == Pause) && (EnterPressed == false) && (PauseMenuIndex == 0))
				PauseGame = false;
			if (((MenuIndex == Pause) && (EnterPressed == false) && (PauseMenuIndex == 2)) || (MenuIndex == GameOver))
			{
				MenuIndex = Main;
				GG.DeleteGrid();
				RestartGame = true;
				StartGame = false;
			}
			if ((MenuIndex == Pause) && (EnterPressed == false) && (PauseMenuIndex == 1))
				MenuIndex = Options;
			if ((MenuIndex == Pause) && (EnterPressed == false) && (PauseMenuIndex == 3))
				MenuIndex = Exit;
			
			EnterPressed = true;
		}
		if ((glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE))
			EnterPressed = false;
		
		// return to previus menu
		if ((glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS))
		{
			if (MenuIndex == GameOver)
				exit = true;
			if (PauseGame)
				MenuIndex = Pause;
			else if (MenuIndex > 0)
				MenuIndex = Main;
			
		}

		// pause the game and draw PauseMenu
		if ((glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) && (StartGame))
		{
			UpDown = 0;
			PauseGame = true;
			MenuIndex = Pause;
		}
	}

	void Draw(int fps, int score, int lives, bool DispMen)
	{
		// Draw fps
		if ((Settings[ShowFps]) && (fps > 0))
		{
			string fps_c;
			if (fps > 100)
			{
				fps_c.push_back(((fps / 100 + 48)));
				fps_c.push_back((((fps % 100) / 10 + 48)));
				fps_c.push_back(((fps % 10 + 48)));
			}
			else
			{
				fps_c.push_back(((fps / 10 + 48)));
				fps_c.push_back(((fps % 10 + 48)));
			}
		//	cout << fps << " " <<  fps / 100 << " " <<  fps % 100 << " " << fps % 10 << endl;
			if (StartGame)
				WRT.Write(fps_c.c_str(), vec3(BoardLimits[0] + 0.05, BoardLimits[3] - 0.05, BoardLimits[4] + 0.1), 0.3);
			else
				WRT.Write(fps_c.c_str(), vec3(-1.5, 1.5, 0), 0.5);
		}
		// Draw score
		if ((Settings[ShowScore]) && (StartGame) && (fps > 0))
		{
			string score_c;
			if (score > 99)
			{
				score_c.push_back(((score / 100 + 48)));
				score_c.push_back((((score % 100) / 10 + 48)));
				score_c.push_back(((score % 10 + 48)));
				WRT.Write(score_c.c_str(), vec3(BoardLimits[1] - 0.1, BoardLimits[3] - 0.15, BoardLimits[4] + 0.1), 0.3);
			}
			else if (score > 9)
			{
				score_c.push_back(((score / 10 + 48)));
				score_c.push_back(((score % 10 + 48)));
				WRT.Write(score_c.c_str(), vec3(BoardLimits[1] - 0.05, BoardLimits[3] - 0.1, BoardLimits[4] + 0.1), 0.3);
			}
			else
			{
				score_c.push_back(score + 48);
				WRT.Write(score_c.c_str(), vec3(BoardLimits[1] - 0.05, BoardLimits[3] - 0.05, BoardLimits[4] + 0.1), 0.3);
			}
			
		}
		// Draw Lives
		if ((Settings[ShowScore]) && (StartGame) && (fps > 0))
		{
			string lives_c;
			lives_c.push_back(lives + 48);
			WRT.Write(lives_c.c_str(), vec3(BoardLimits[0] + 0.05, BoardLimits[2] + 0.05, BoardLimits[4] + 0.1), 0.3);
		}
		// Game Ended
		if (fps < 0)
		{
			MenuIndex = GameOver;
			GameOverMenu(score);
		}
		if (DispMen)
		{ 
			if (MenuIndex == Main)
				DrawMainMenu();
			else if (MenuIndex == Play)
				DrawPlayMenu();
			else if (MenuIndex == Options)
				DrawSettingsMenu();
			else if (MenuIndex == Exit)
				DrawExit();
			else if (MenuIndex == Pause)
				DrawPauseMenu();
		}
		
	}
};