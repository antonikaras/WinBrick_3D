#include <winbrick\winbrick_ver8.h>

void createContext()
{
    // Create and compile our GLSL program from the shaders
    shaderProgram = loadShaders(
        "StandardShading.vertexshader",
        "StandardShading.fragmentshader");

	// load cubes
	ReadCubes();

	// Get uniform pointers & define VAOs
	GetUniformPointers();

	// Create the box to surround the game
	//box = new Box((NumxCubes + 2), (NumyCubes + 2), NumzCubes);
}

void free()
{
	for (int i = 0; i < cbs.size(); i++)
		cbs[i].Delete();
	for (int i = 0; i < balls.size(); i++)
		balls[i].Delete();
	box.Delete();
	board.Delete();
	WRT.Delete();

	glDeleteProgram(shaderProgram);
    glfwTerminate();
}

void mainLoop()
{
	float t = glfwGetTime();
	camera->position = (vec3(0, 0, NumzCubes + 1.8));

	// Create Game
	GG.NumxCubes = NumxCubes;
	GG.NumyCubes = NumyCubes;
	GG.NumzCubes = NumzCubes;
	//GG.CreateGame(cbs);

	// Set the Bounding Box Dimensions
	box.T = vec3(0, 0, 0.5 * (NumzCubes + 1.9f));
	box.S = vec3((NumxCubes + 2), (NumyCubes + 2), (NumzCubes + 2));
	box.FindLimits();

	// Define the bounding box dimensions to the Board
	board.BoundingBoxDims[0] = -0.5 * box.S.x;
	board.BoundingBoxDims[1] = 0.5 * box.S.x;
	board.BoundingBoxDims[2] = -0.5 * box.S.y;
	board.BoundingBoxDims[3] = 0.5 * box.S.y;

	// Define the bounding box dimensions to the GameGrid, used for ball-box collisions
	GG.BoxLimits[0] = -0.5 * box.S.x;
	GG.BoxLimits[1] = 0.5 * box.S.x;
	GG.BoxLimits[2] = -0.5 * box.S.y;
	GG.BoxLimits[3] = 0.5 * box.S.y;
	GG.BoxLimits[4] = 0;
	GG.BoxLimits[5] = box.S.z;

	// change board position
	board.Pos = vec3(0.0f, 0.0f, 0.8 * NumzCubes );
	// change ball initial position
	balls[0].Pos = vec3(0.0f, 0.0f, 0.6 * NumzCubes - 2.0f * balls[0].radius);
	balls[0].V = vec3(0.3, 0.35, -0.5);
	//float dt = 0;
	int frames = 0;
	float dt_sum = 0;
	int fps = 0;
	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(shaderProgram);

		// camera
		camera->update();
		// calculate dt
		float currentTime = glfwGetTime();
		float dt = currentTime - t;
		//float dt = 0;
		if (!GameStarted)
		{
			mat4 projectionMatrix = camera->projectionMatrix;
			mat4 viewMatrix = camera->viewMatrix;
			// transfer uniforms to GPU
			glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
			glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
			uploadLight(light);
			GameStarted = MN.StartGame;
			MN.Draw(fps);
		}
		else
		{
			bool EndGame = GG.BallBoxCollision(balls[0].radius, balls[0].Pos, balls[0].V);
			if (!EndGame) 
			{
				GG.BallBoardCollision(balls[0].radius, balls[0].Pos, balls[0].V, board.BoardLimits, board.Norms[0]);
				if (StopRender)
					RenderGame(t, 0, fps);
				else if (MN.PauseGame)
				{
					MN.Draw(fps);
					RenderGame(t, 0, fps);
				}
				else
				{
					GG.BallCubeGridCollision(balls[0].radius, balls[0].Pos, balls[0].V);
					RenderGame(t, dt, fps);
				}
			}
		}

		t += dt;
		// calculate fps
		frames++;
		dt_sum += dt;
		if (dt_sum > 1)
		{
			fps = frames;
			dt_sum = 0;
			frames = 0;
		}
        glfwSwapBuffers(window);

        glfwPollEvents();
    } while ((glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0) &&
		(!MN.Exit));
}

void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (!MN.PauseGame)
		board.Movment(window, key, scancode, action, mods);
	//GG.RemoveBrick(window, key, scancode, action, mods);
	MN.MenuMovment(window, key, scancode, action, mods);
	if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS))
		StopRender = true;
	if ((glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS))
		StopRender = false;

}

void initialize()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        throw runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        throw runtime_error(string(string("Failed to open GLFW window.") +
            " If you have an Intel GPU, they are not 3.3 compatible." +
            "Try the 2.1 version.\n"));
    }
    glfwMakeContextCurrent(window);

    // Start GLEW extension handler
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw runtime_error("Failed to initialize GLEW\n");
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

    // Gray background color
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

	// Keyboard function
	glfwSetKeyCallback(window, pollKeyboard);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// enable textures
	glEnable(GL_TEXTURE_2D);

    // Log
    logGLParameters();

    // Create camera
    camera = new Camera(window);
}

int main(void)
{
    try
    {
        initialize();
        createContext();
        mainLoop();
        free();
    }
    catch (exception& ex)
    {
        cout << ex.what() << endl;
        getchar();
        free();
        return -1;
    }

    return 0;
}

//*************************************************************************************************

void GetUniformPointers()
{

	// create Bounding Box
	box.Load();

	// create ball
	Ball tmp;
	tmp.Load();
	balls.push_back(tmp);

	// read the fonts
	WRT.Load();

	// a pointer to the texture sampler
	diffuceColorSampler = glGetUniformLocation(shaderProgram, "diffuceColorSampler");
	WRT.DifColSam = diffuceColorSampler;
	balls[0].UseTextLoc = box.UseTextLoc = board.UseTextLoc = UseTextureLocation = glGetUniformLocation(shaderProgram, "useTexture");
	WRT.UseTextLoc = UseTextureLocation;

	// get pointers for the uniform variables (transformation matrices)
	projectionMatrixLocation = glGetUniformLocation(shaderProgram, "P");
	viewMatrixLocation = glGetUniformLocation(shaderProgram, "V");
	modelMatrixLocation = glGetUniformLocation(shaderProgram, "M");
	board.ModMatLoc = modelMatrixLocation;
	WRT.modmatLoc = modelMatrixLocation;
	balls[0].ModMatLoc = box.modMatLoc = modelMatrixLocation;

	// get pointers for the uniform variables (light coefficients)
	LaLocation = glGetUniformLocation(shaderProgram, "light.La");
	LdLocation = glGetUniformLocation(shaderProgram, "light.Ld");
	LsLocation = glGetUniformLocation(shaderProgram, "light.Ls");
	lightPositionLocation = glGetUniformLocation(shaderProgram, "light.lightPosition_worldspace");
	lightPowerLocation = glGetUniformLocation(shaderProgram, "light.power");
	WRT.EnPhLightLoc =  EnPhLightLoc = glGetUniformLocation(shaderProgram, "EnablePhong");
	

	// get pointers for the uniform variables (material coefficients)
	balls[0].MaterialLoc[0] = board.MaterialLoc[0] = box.MaterialLoc[0] = KaLocation = glGetUniformLocation(shaderProgram, "mtl.Ka");
	balls[0].MaterialLoc[1] = board.MaterialLoc[1] = box.MaterialLoc[1] = KdLocation = glGetUniformLocation(shaderProgram, "mtl.Kd");
	balls[0].MaterialLoc[2] = board.MaterialLoc[2] = box.MaterialLoc[2] = KsLocation = glGetUniformLocation(shaderProgram, "mtl.Ks");
	balls[0].MaterialLoc[3] = board.MaterialLoc[3] = box.MaterialLoc[3] = NsLocation = glGetUniformLocation(shaderProgram, "mtl.Ns");
	 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// create VAOs 

	for (int i = 0; i < cbs.size(); i++)
	{
		cbs[i].Bind();
	}

	board.Bind();
	balls[0].Bind();
	box.Bind();
	WRT.Bind();
}

//*************************************************************************************************


// This function import the cubes and 
// the textures used for the game.
void ReadCubes()
{

	// ----> change the working directory to read the cubes <----
	
	char cwd[150], *cwd2;
	cwd2 = _getcwd(cwd, 150);	
	//cout << cwd2 << endl;
	string wd = string(cwd2);	// store working directory
	chdir("../");				// go to the previous directory
	cwd2 = _getcwd(cwd, 150);
	string pwd = string(cwd2);
	string nwd = pwd + "/resources/";
	chdir(nwd.c_str());			// Change to the resources directory
	// ----> open the txt with the texture's names <----
	ifstream myReadFile;
	myReadFile.open("Materials.txt");
	char output[100];
	if (myReadFile.is_open())
	{
		int i = 0;
		// ----> start reading the txt file <----
		while (!myReadFile.eof())
		{
			string TextName;
			// ----> read the name of the texture <----
			myReadFile >> output;
			string data = string(output);

			// disregard first 3 data
			if (i > 2)
			{
				// Texture Name, Life, Smoothness
				Cube cb;
				cb.Load(string(output), 1);
				myReadFile >> output;
				cb.Life = atoi(output);
				myReadFile >> output;
				cb.Smoothness = atoi(output);
				cbs.push_back(cb);	// store the cube in the struct
			//	cout << cb.Life << " " << cb.Smoothness << endl;
			}
			i++;
		}
	}
	// ----> close the txt with the material names <----	
	myReadFile.close();
	// ----> return to the intitial working directory <----	
	chdir(wd.c_str());
	MN.cbs = cbs;
}

//*************************************************************************************************

void uploadMaterial(const Material& mtl) {
	glUniform4f(KaLocation, mtl.Ka.r, mtl.Ka.g, mtl.Ka.b, mtl.Ka.a);
	glUniform4f(KdLocation, mtl.Kd.r, mtl.Kd.g, mtl.Kd.b, mtl.Kd.a);
	glUniform4f(KsLocation, mtl.Ks.r, mtl.Ks.g, mtl.Ks.b, mtl.Ks.a);
	glUniform1f(NsLocation, mtl.Ns);
}

void uploadLight(const Light& light) {
	glUniform4f(LaLocation, light.La.r, light.La.g, light.La.b, light.La.a);
	glUniform4f(LdLocation, light.Ld.r, light.Ld.g, light.Ld.b, light.Ld.a);
	glUniform4f(LsLocation, light.Ls.r, light.Ls.g, light.Ls.b, light.Ls.a);
	glUniform3f(lightPositionLocation, light.lightPosition_worldspace.x,
		light.lightPosition_worldspace.y, light.lightPosition_worldspace.z);
	glUniform1f(lightPowerLocation, light.power);
}

void  RenderGame(float t, float dt, int fps)
{

	/*
		This function draws the
		cubes in the game window
	*/

	mat4 projectionMatrix = camera->projectionMatrix;
	mat4 viewMatrix = camera->viewMatrix;
	// transfer uniforms to GPU
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	//glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

	uploadLight(light);
	uploadMaterial(CubeMaterial);
	
	DeletedElements = GG.DeletedElement;
	//cout << DeletedElements[0][0].size() << " " << DeletedElements[0].size() << " " << DeletedElements.size() << endl;
	//cout << GG.cbsSpecs.size() << endl;
	// update the number of cubes per axis
	NumxCubes = GG.NumxCubes;
	NumyCubes = GG.NumyCubes;
	NumzCubes = GG.NumzCubes;

	for (int i = 0; i < cbs.size(); i++)
	{
		glBindVertexArray(cbs[i].VAO);
		glUniform1i(UseTextureLocation, 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cbs[i].Texture);
		glUniform1i(diffuceColorSampler, 0);
		for (int j = 0; j < GG.cbsSpecs.size(); j++)
		{
			int z = j / (2 * NumxCubes + 1) / (2 * NumyCubes + 1);
			int y = (j / (2 * NumxCubes + 1)) % (2 * NumyCubes + 1);
			int x = j % (2 * NumxCubes + 1) ;
	//		cout << j << " " << x << " " << y << " " << z << endl;
			if ((GG.cbsSpecs[j].Type == i) && !(DeletedElements[z][y][x]))
			{
				glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &GG.cbsSpecs[j].M[0][0]);
				glDrawArrays(GL_TRIANGLES, 0, cbs[i].Verts.size());
			}
		}
	}
	
	for (int i = 0; i < balls.size(); i++)
	{
		balls[i].update(t, dt);
		balls[i].Draw();
	}

	// draw the boundimg Cube
	box.Draw();

	// draw the board
	board.Draw();
	//glUniform1i(UseTextureLocation, 1);

	//WRT.Draw();
	//cout << fps << endl;

}

//*************************************************************************************************