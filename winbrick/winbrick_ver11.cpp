#include <winbrick\winbrick_ver11.h>

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
	glDeleteBuffers(1, &MirVertsVBO);
	glDeleteBuffers(1, &MirUVsVBO);
	glDeleteBuffers(1, &MirNormsVBO);
	glDeleteVertexArrays(1, &MirVAO);
	glDeleteVertexArrays(1, &BoardMirrorVAO);
	glDeleteBuffers(1, &BoardMirrorVertsVBO);
	glDeleteVertexArrays(1, &BallBoardVAO);
	glDeleteBuffers(1, &BallBoardVertsVBO);
	glDeleteVertexArrays(1, &BallMirrorVAO);
	glDeleteBuffers(1, &BallMirrorVertsVBO);
	glDeleteProgram(shaderProgram);
    glfwTerminate();
}

void mainLoop()
{
	float t = glfwGetTime();
	camera.position = (vec3(0, 0, 5));
	camera.direction = vec3(0, 0, -1);
		// update the number of cubes per axis
	GG.NumxCubes = NumxCubes ;
	GG.NumyCubes = NumyCubes;
	GG.NumzCubes = NumzCubes;
	MN.BallSpeed = ball.speed = 1;
	//float dt = 0;
	int frames = 0;
	float dt_sum = 0;
	int fps = 0;

	float LightPower = 8.0f ;
	light1.SideLightPosition = mat4( 0, 0, 4, LightPower,
									0, 0, 4, LightPower,
									0, 0, 4, LightPower,
									0, 0, 4, LightPower);
	light1.FrontLightPosition = vec4(0, 0, 4, LightPower);
	light1.La = light1.Ld = light1.Ls = vec4(1);

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(shaderProgram);

		// camera
		camera.update();
		// calculate dt
		float currentTime = glfwGetTime();
		float dt = currentTime - t;
		//float dt = 0;
		
		EnableMirror = MN.Settings[MN.EnableMirror];
		EnableBallBoardProjection = MN.Settings[MN.EnableBallBoardProjection];
		EnableBallMirrorProjection = MN.Settings[MN.EnableBallWallProjection];
		EnableBoardMirrorProjection = MN.Settings[MN.EnableBoardWallProjection];
		scoreMultiplier = 1;
		if (!EnableMirror)
			scoreMultiplier += 2;
		if (!EnableBallBoardProjection)
			scoreMultiplier += 2;
		if (!EnableBallMirrorProjection)
			scoreMultiplier += 2;
		if (!EnableBoardMirrorProjection)
			scoreMultiplier += 2;
		if (balls.size() > 0)
		{
			balls[0].speed = MN.BallSpeed;
			scoreMultiplier += 5 * (balls[0].speed - 1);
			score = GG.Collisions;
			if (scoreMultiplier <= 0)
				scoreMultiplier = 1;
			score = score * scoreMultiplier;
		}
		else
			score = score;
		
		//cout << EndGame << endl;
		if (MN.RestartGame)
		{
			GameStarted = false;
			BoxCreated = false;
			MN.RestartGame = false;
			MN.StartGame = false;
			EndGame = false;
			StickyBoard = false;
			Lives = 3;
			balls.clear();
			GG.NoBrickCollissions = 0;
			score = 0;
			//cout << "in 1" << endl;
		}
		else if (!GameStarted)
		{
			mat4 projectionMatrix = camera.projectionMatrix;
			mat4 viewMatrix = camera.viewMatrix;
			// transfer uniforms to GPU
			glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
			glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
			uploadLight(light1);
			GameStarted = MN.StartGame;
			MN.Draw(fps, score, Lives,true);
			BoxCreated = false;
			MN.ScoreMultiplier = scoreMultiplier;
			//cout << "in 2" << endl;
		}
		else if (!EndGame)
		{
			//cout << "in 3" << endl;
			if (!BoxCreated)
			{
				BoxCreated = true;
				InitializeGameEnvironment();
				MN.RestartGame = false;
			}
			//cout << "in 6" << endl;
			for (int i = 0; i < 6; i++)
				MN.BoardLimits[i] = board.BoardLimits[i];
			if ((balls[0].V.x != 0) || (balls[0].V.y != 0) || (balls[0].V.z != 0))
			{
				for (int i = 0; i < balls.size(); i++)
				{
					if (GG.BallBoxCollision(balls[i].radius, balls[i].Pos, balls[i].V))
						balls.erase(balls.begin() + i);
				}
			}
			
			if (balls.size() == 0)
				EndGame = true;
			if (!EndGame)
			{
				//cout << "in 5" << endl;
				for (int i = 0; i < balls.size(); i++)
					GG.BallBoardCollision(balls[i].radius, balls[i].Pos, balls[i].V, board.BoardLimits, board.Norms[0], StickyBoard, 1);
				if (StopRender)
					RenderGame(t, 0, fps);
				else if (MN.PauseGame)
				{
					RenderGame(t, 0, fps);
					uploadLight(light1);
					MN.Draw(fps, score, Lives, true);
				}
				else
				{

					for (int i = 0; i < balls.size(); i++)
						GG.BallCubeGridCollision(balls[i].radius, balls[i].Pos, balls[i].V);
					RenderGame(t, dt, fps);
					uploadLight(light1);
					MN.Draw(fps, score, Lives, false);

					// set ball initial speed using the mouse
					if (balls[0].V == vec3(0))
					{
						int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
						double xPos, yPos;
						glfwGetCursorPos(window, &xPos, &yPos);
						vec3 V = vec3(0);
						V = (vec3(2 * xPos / double(W_WIDTH) - 1, -2 * yPos / double(W_HEIGHT) + 1, board.Pos.z + 0.01f));
						if (state == GLFW_PRESS)
							balls[0].V = normalize(vec3(V.x,V.y,-1));
						glBindVertexArray(BallMirrorVAO);
						glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &V, GL_STATIC_DRAW);
						glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glEnableVertexAttribArray(0);
						glUniform1i(UseTextureLocation, 0);
						mat4 M = mat4(1);
						glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);
						glDrawArrays(GL_POINTS, 0, 1);
					//	cout << V.x << " " << V.y << " " << V.z << endl;
					}
				}
			}
		}
		else if ((EndGame) && (Lives > 1))
		{
			EndGame = false;
			InitializeGameEnvironment();
			Lives--;
			GG.NoBrickCollissions = 0;
		}
		else if ((EndGame) && (Lives == 1))
		{
			//cout << "in 4" << endl;
			uploadLight(light1);
			MN.Draw(-1, score, Lives, false);
		}
		//cout << Lives << endl;
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
		(!MN.exit));
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
	if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS))
		light2.FrontLightPosition.w++;
	if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)) 
		light2.FrontLightPosition.w--;
	if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS))
		EndGame = true;
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
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glPointSize(10);

    // Log
    logGLParameters();

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
	ball.Load();

	// read the fonts
	WRT.Load();

	// read the Extras
	extras.Load();

	// a pointer to the texture sampler
	diffuceColorSampler = glGetUniformLocation(shaderProgram, "diffuceColorSampler");
	WRT.DifColSam = diffuceColorSampler;
	ball.UseTextLoc = box.UseTextLoc = board.UseTextLoc = UseTextureLocation = glGetUniformLocation(shaderProgram, "useTexture");
	WRT.UseTextLoc = UseTextureLocation;
	extras.UseTextLoc = UseTextureLocation;

	// get pointers for the uniform variables (transformation matrices)
	projectionMatrixLocation = glGetUniformLocation(shaderProgram, "P");
	viewMatrixLocation = glGetUniformLocation(shaderProgram, "V");
	modelMatrixLocation = glGetUniformLocation(shaderProgram, "M");
	board.ModMatLoc = modelMatrixLocation;
	WRT.modmatLoc = modelMatrixLocation;
	ball.ModMatLoc = box.modMatLoc = modelMatrixLocation;
	extras.ModMatLoc = modelMatrixLocation;

	// get pointers for the uniform variables (light coefficients)
	LaLocation = glGetUniformLocation(shaderProgram, "light.La");
	LdLocation = glGetUniformLocation(shaderProgram, "light.Ld");
	LsLocation = glGetUniformLocation(shaderProgram, "light.Ls");
	SideLightPositionLocation = glGetUniformLocation(shaderProgram, "light.SideLightPosition");
	FrontLightPositionLocation = glGetUniformLocation(shaderProgram, "light.FrontLightPosition");
	

	// get pointers for the uniform variables (material coefficients)
	ball.MaterialLoc[0] = board.MaterialLoc[0] = box.MaterialLoc[0] = KaLocation = glGetUniformLocation(shaderProgram, "mtl.Ka");
	ball.MaterialLoc[1] = board.MaterialLoc[1] = box.MaterialLoc[1] = KdLocation = glGetUniformLocation(shaderProgram, "mtl.Kd");
	ball.MaterialLoc[2] = board.MaterialLoc[2] = box.MaterialLoc[2] = KsLocation = glGetUniformLocation(shaderProgram, "mtl.Ks");
	ball.MaterialLoc[3] = board.MaterialLoc[3] = box.MaterialLoc[3] = NsLocation = glGetUniformLocation(shaderProgram, "mtl.Ns");
	 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// create VAOs 

	for (int i = 0; i < cbs.size(); i++)
	{
		cbs[i].Bind();
	}

	// create mirror VBOs
	glGenVertexArrays(1, &MirVAO);
	glBindVertexArray(MirVAO);

	// vertex VBO
	glGenBuffers(1, &MirVertsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, MirVertsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MirVerts), &MirVerts[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	// normals VBO
	glGenBuffers(1, &MirNormsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, MirNormsVBO);
	glBufferData(GL_ARRAY_BUFFER, MirNorms.size() * sizeof(glm::vec3), &MirNorms[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	// uvs VBO
	glGenBuffers(1, &MirUVsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, MirUVsVBO);
	glBufferData(GL_ARRAY_BUFFER, MirUVs.size() * sizeof(glm::vec2), &MirUVs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	board.Bind();
	ball.Bind();
	box.Bind();
	WRT.Bind();
	extras.Bind();
	
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
	glUniformMatrix4fv(SideLightPositionLocation, 1, GL_FALSE, &light.SideLightPosition[0][0]);
	glUniform4f(FrontLightPositionLocation, light.FrontLightPosition.x, light.FrontLightPosition.y, light.FrontLightPosition.x, light.FrontLightPosition.w);
}

// This function draws the cubes in the game window
void  RenderGame(float t, float dt, int fps)
{

	// update the number of cubes per axis
	NumxCubes = GG.NumxCubes;
	NumyCubes = GG.NumyCubes;
	NumzCubes = GG.NumzCubes;
	DeletedElements = GG.DeletedElement;

	//***************************************************//
	/* Generate the reflections on every side of the box */
	//***************************************************//

	float height_x, height_y, height_z;
	// update the ball position
	for (int i = 0; i < balls.size(); i++)
		balls[i].update(t, dt);
	
	if (EnableMirror)
	{
		if (int(t) % 1 == 0)
		{
			if (TextID[0] != -1)
			{
				for (int i = 0; i < 4; i++)
					glDeleteTextures(1, &TextID[i]);
			}

			height_x = (((NumyCubes + 2) >(NumzCubes)) ? (0.5 * NumyCubes / tan(radians(0.5 * camera.FoV))) : (0.5 * NumzCubes / tan(radians(0.5 * camera.FoV)))) + 0.5 * (NumxCubes + 2);
			height_y = (((NumxCubes + 2) > (NumzCubes)) ? (0.5 * NumxCubes / tan(radians(0.5 * camera.FoV))) : (0.5 * NumzCubes / tan(radians(0.5 * camera.FoV)))) + 0.5 * (NumyCubes + 2);
			height_z = 0.5 * NumzCubes;

			vec3 CamUp[4] = { vec3(0,1,0), vec3(-1,0,0), vec3(0,1,0), vec3(-1,0,0) };
			vec3 CamDir[4] = { vec3(-1,0,0),vec3(0,-1,0),vec3(1,0,0),vec3(0,1,0) };
			vec3 CamPos[4] = { vec3(height_x, 0, height_z), vec3(0, height_y, height_z),  vec3(-height_x, 0, height_z), vec3(0, -height_y, height_z) };
			
			for (int k = 0; k < 4; k++)
			{
				camera.position = CamPos[k];
				camera.direction = CamDir[k];
				camera.up = CamUp[k];
				camera.update();
				// draw the cubes
				mat4 projectionMatrix = camera.projectionMatrix;
				mat4 viewMatrix = camera.viewMatrix;
				// transfer uniforms to GPU
				glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
				glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

				uploadLight(light2);
				
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
						int x = j % (2 * NumxCubes + 1);
						//cout << j << " " << x << " " << y << " " << z << endl;
						//cout << GG.cbsSpecs[j].Type << endl;
						//cout << DeletedElements[z][y][x] << endl;
						if ((GG.cbsSpecs[j].Type == i) && !(DeletedElements[z][y][x]))
						{
							glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &GG.cbsSpecs[j].M[0][0]);
							glDrawArrays(GL_TRIANGLES, 0, cbs[i].Verts.size());
						}
					}
				}


				// create the "reflection's texture"

				unsigned char* buffer = (unsigned char*)malloc(W_WIDTH * W_HEIGHT * 3);
				glReadPixels(0, 0, W_WIDTH, W_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buffer);
				glGenTextures(1, &TextID[k]);
				// "Bind" the newly created texture : all future texture functions will modify this texture
				glBindTexture(GL_TEXTURE_2D, TextID[k]);

				// Give the image to OpenGL
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W_WIDTH, W_HEIGHT, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);
				delete buffer;

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				// ... which requires mipmaps. Generate them automatically.
				glGenerateMipmap(GL_TEXTURE_2D);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
		}
	}

	//***************************************************//
	// draw the front side
	//***************************************************//
	
	height_z = ((NumxCubes < NumyCubes) ? (0.5 * (NumxCubes + 2) / tan(radians(0.5 * camera.FoV))) : (0.5 * (NumyCubes + 2) / tan(radians(0.5 * camera.FoV)))) + NumzCubes;
	camera.position = vec3(0, 0, height_z);
	camera.direction = vec3(0, 0, -1);
	camera.up = vec3(0, 1, 0);
	camera.update();
	mat4 projectionMatrix = camera.projectionMatrix;
	mat4 viewMatrix = camera.viewMatrix;
	// transfer uniforms to GPU
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	//glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	uploadLight(light2);
	uploadMaterial(CubeMaterial);
	
	//cout << DeletedElements[0][0].size() << " " << DeletedElements[0].size() << " " << DeletedElements.size() << endl;
	//cout << GG.cbsSpecs.size() << endl;
	
	// draw the bounding box mirror sides
	mat4 M[4];
	M[0] = translate(mat4(), vec3(0.5 * (NumxCubes + 2), 0, 2)) * rotate(mat4(), 3.14f, vec3(0, 0, 1)) * rotate(mat4(), 0.5f * 3.14f, vec3(0, -1, 0)) * scale(mat4(), vec3(height_z, NumyCubes + 2, 1)) * rotate(mat4(), 1.5f * 3.14f, vec3(0, 0, 1));
	M[1] = translate(mat4(), vec3(0, 0.5 * (NumyCubes + 2), 2)) * rotate(mat4(), 3.14f, vec3(0, 0, 1)) * rotate(mat4(), 0.5f * 3.14f, vec3(1, 0, 0)) * scale(mat4(), vec3(NumxCubes + 2, height_z, 1));
	M[2] = translate(mat4(), vec3(-0.5 * (NumxCubes + 2), 0, 2)) * rotate(mat4(), 3.14f, vec3(0, 0, 1)) * rotate(mat4(), -1.5f * 3.14f, vec3(0, 1, 0)) * scale(mat4(), vec3(height_z, NumyCubes + 2, 1)) * rotate(mat4(), 1.5f * 3.14f, vec3(0, 0, 1));
	M[3] = translate(mat4(), vec3(0, -0.5 * (NumyCubes + 2), 2)) * rotate(mat4(), 3.14f, vec3(0, 0, 1)) * rotate(mat4(), -1.5f * 3.14f, vec3(-1, 0, 0)) * scale(mat4(), vec3(NumxCubes + 2, height_z, 1)) ;
	for (int i = 0; i < 4; i++)
	{
		glBindVertexArray(MirVAO);
		if (EnableMirror)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextID[i]);
			glUniform1i(diffuceColorSampler, 0);
		}
		else
		{
			glUniform1i(UseTextureLocation, 0);
			uploadMaterial(CubeMaterial);
		}
		//mat4 Mi = M[i];
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[i][0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
		
	}

	// Draw main cube Grid
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
		//	cout << j << " " << x << " " << y << " " << z << endl;
			if ((GG.cbsSpecs[j].Type == i) && !(DeletedElements[z][y][x]))
			{
				glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &GG.cbsSpecs[j].M[0][0]);
				glDrawArrays(GL_TRIANGLES, 0, cbs[i].Verts.size());
			}
		}
	}

	// Draw Extras
	for (int i = 0; i < GG.cbsSpecs.size(); i++)
	{
		int z = i / (2 * NumxCubes + 1) / (2 * NumyCubes + 1);
		int y = (i / (2 * NumxCubes + 1)) % (2 * NumyCubes + 1);
		int x = i % (2 * NumxCubes + 1);
		if (DeletedElements[z][y][x])
		{
			if (GG.cbsSpecs[i].ExtraID != -1)
			{
				extras.Draw(GG.cbsSpecs[i].Pos, dt, GG.cbsSpecs[i].ExtraID);
				ProcessExtras(i);
			}
		}
	}

	if (EnableBallBoardProjection)
	{
		vector<vec3> tmp;
		for (int i = 0; i < balls.size(); i++)
		{
			vec2 pos = vec2(balls[i].Pos.x, balls[i].Pos.y);
			float r = balls[i].radius;
			if ((pos.x + r > board.BoardLimits[0]) && (pos.x - r < board.BoardLimits[1]) &&
				(pos.y + r > board.BoardLimits[2]) && (pos.y - r < board.BoardLimits[3]))
				tmp.push_back(vec3(pos, board.Pos.z + 0.01f));
		}
		if (tmp.size() > 0)
		{
			glBindVertexArray(BallBoardVAO);
			glBufferData(GL_ARRAY_BUFFER, tmp.size() * sizeof(glm::vec3), &tmp[0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnableVertexAttribArray(0);
			glUniform1i(UseTextureLocation, 0);
			mat4 M = mat4(1);
			glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);
			glDrawArrays(GL_POINTS, 0, tmp.size());
		}

	}

	if (EnableBallMirrorProjection)
	{
		vector<vec3> tmp;
		for (int i = 0 ; i < balls.size(); i++)
		{
			vec3 rm = vec3(GG.BoxLimits[1] - 0.01f, balls[i].Pos.y, balls[i].Pos.z);
			vec3 tm = vec3(balls[i].Pos.x, GG.BoxLimits[3] - 0.01f, balls[i].Pos.z);
			vec3 lm = vec3(GG.BoxLimits[0] + 0.01f, balls[i].Pos.y, balls[i].Pos.z);
			vec3 bm = vec3(balls[i].Pos.x, GG.BoxLimits[2] + 0.01f, balls[i].Pos.z);
			tmp.push_back(rm);
			tmp.push_back(tm);
			tmp.push_back(lm);
			tmp.push_back(bm);
		}

		glBindVertexArray(BallMirrorVAO);
		glBufferData(GL_ARRAY_BUFFER, tmp.size() * sizeof(glm::vec3), &tmp[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnableVertexAttribArray(0);
		glUniform1i(UseTextureLocation, 0);
		mat4 M = mat4(1);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_POINTS, 0, tmp.size());
	}

	if (EnableBoardMirrorProjection)
	{
		glBindVertexArray(BoardMirrorVAO);
		glBindBuffer(GL_ARRAY_BUFFER, BoardMirrorVertsVBO);
		glBufferData(GL_ARRAY_BUFFER, BoardMirrorVerts.size() * sizeof(glm::vec3), &BoardMirrorVerts[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnableVertexAttribArray(0);
		glUniform1i(UseTextureLocation, 0);
		mat4 M = mat4(1);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_LINE_STRIP, 0, 5);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	for (int i = 0; i < balls.size(); i++)
		balls[i].Draw();

	// draw the boundimg Cube
	//box.Draw();

	// draw the board
	board.Draw();
	//glUniform1i(UseTextureLocation, 1);

	//WRT.Draw();
	//cout << fps << endl;

}

//*************************************************************************************************

void InitializeGameEnvironment()
{
//	cout << "in 7" << endl;
	// update the number of cubes per axis
	NumxCubes = GG.NumxCubes;
	NumyCubes = GG.NumyCubes;
	NumzCubes = GG.NumzCubes;

	// Set the Bounding Box Dimensions
	box.T = vec3(0, 0, 0.5 * (NumzCubes + 1.9f));
	box.S = vec3((NumxCubes + 2), (NumyCubes + 2), (NumzCubes + 2));
	box.FindLimits();
	//cout << "in 10" << endl;
	// Define the bounding box dimensions to the Board
	board.BoundingBoxDims[0] = -0.5 * box.S.x;
	board.BoundingBoxDims[1] = 0.5 * box.S.x;
	board.BoundingBoxDims[2] = -0.5 * box.S.y;
	board.BoundingBoxDims[3] = 0.5 * box.S.y;
	board.BoardLevel = 1.0f;

	// Define the bounding box dimensions to the GameGrid, used for ball-box collisions
	GG.BoxLimits[0] = -0.5 * box.S.x;
	GG.BoxLimits[1] = 0.5 * box.S.x;
	GG.BoxLimits[2] = -0.5 * box.S.y;
	GG.BoxLimits[3] = 0.5 * box.S.y;
	GG.BoxLimits[4] = 0;
	GG.BoxLimits[5] = box.S.z;
//	cout << "in 11" << endl;
	// change board position
	board.Pos = vec3(0.0f, 0.0f, 0.8 * NumzCubes);
	// change ball initial position
	balls.push_back(ball);
	balls[0].Pos = vec3(0.0f, 0.0f, 0.6 * NumzCubes - 2.0f * balls[0].radius);
	balls[0].V = vec3(0.0, 0.0, 0.0);

	// generate the buffers for the board mirror projection
	vec3 tmp_vec = vec3(GG.BoxLimits[1] - 0.01f, GG.BoxLimits[3] - 0.01f, board.Pos.z);
	vector<vec3> tmp = { vec3(-tmp_vec.x, tmp_vec.y, tmp_vec.z), vec3(tmp_vec.x , tmp_vec.y, tmp_vec.z),
						 vec3(tmp_vec.x, -tmp_vec.y, tmp_vec.z), vec3(-tmp_vec.x , -tmp_vec.y, tmp_vec.z),
						 vec3(-tmp_vec.x, tmp_vec.y, tmp_vec.z) };
	BoardMirrorVerts = tmp;
//	cout << "in 8" << endl;
	glGenVertexArrays(1, &BoardMirrorVAO);
	glBindVertexArray(BoardMirrorVAO);
	glGenBuffers(1, &BoardMirrorVertsVBO);
//	cout << "in 9" << endl;
	// generate the buffers for the ball board projection
	glGenVertexArrays(1, &BallBoardVAO);
	glBindVertexArray(BallBoardVAO);
	glGenBuffers(1, &BallBoardVertsVBO);

	// generate the buffers for the ball mirror projection
	glGenVertexArrays(1, &BallMirrorVAO);
	glBindVertexArray(BallMirrorVAO);
	glGenBuffers(1, &BallMirrorVertsVBO);

	// Store the light parameters
	light2.La = light2.Ld = light2.Ls = vec4(1);
	float height_x = (((NumyCubes + 2) > (NumzCubes)) ? (0.5 * NumyCubes / tan(radians(0.5 * camera.FoV))) : (0.5 * NumzCubes / tan(radians(0.5 * camera.FoV)))) + 0.5 * (NumxCubes + 2);
	float height_y = (((NumxCubes + 2) > (NumzCubes)) ? (0.5 * NumxCubes / tan(radians(0.5 * camera.FoV))) : (0.5 * NumzCubes / tan(radians(0.5 * camera.FoV)))) + 0.5 * (NumyCubes + 2);
	float height_z = 0.5 * NumzCubes;
	float LightPower = 5.0f * std::max(height_x, height_y);
	mat4 LightPos = { height_x, 0, height_z, LightPower,
		0, height_y, height_z, LightPower,
		-height_x, 0, height_z, LightPower,
		0, -height_y, height_z, LightPower };

	light2.SideLightPosition = LightPos;
	height_z = ((NumxCubes > NumyCubes) ? ((NumxCubes + 2) / tan(radians(0.5 * camera.FoV))) : ((NumyCubes + 2) / tan(radians(0.5 * camera.FoV)))) + NumzCubes;
	LightPower = std::max(LightPower * 0.2f, height_z) * std::max(LightPower * 0.2f, height_z);
	light2.FrontLightPosition = vec4(0, 0, 2 * height_z, LightPower);
	
}

//*************************************************************************************************

void ProcessExtras(int ExtraPos)
{
	vec3 Pos = GG.cbsSpecs[ExtraPos].Pos;
	
	// Check if the extra is inside the board's limits
	if (Pos.z > std::max(board.BoardLimits[4], board.BoardLimits[5]))
		GG.cbsSpecs[ExtraPos].ExtraID = -1;
	else if ((Pos.x > board.BoardLimits[0]) && (Pos.x < board.BoardLimits[1]) &&
		(Pos.y > board.BoardLimits[2]) && (Pos.y < board.BoardLimits[3]) &&
		(Pos.z + 0.1 >= std::min(board.BoardLimits[4], board.BoardLimits[5])) &&
		(Pos.z - 0.1 <= std::max(board.BoardLimits[4], board.BoardLimits[5])))
	{
		cout << GG.cbsSpecs[ExtraPos].ExtraID << endl;
		if (GG.cbsSpecs[ExtraPos].ExtraID == extras.heart)
			Lives++;
		else if (GG.cbsSpecs[ExtraPos].ExtraID == extras.increase_size)
			board.BoardLevel += 0.1;
		else if (GG.cbsSpecs[ExtraPos].ExtraID == extras.decrease_size)
			board.BoardLevel -= 0.1;
		else if (GG.cbsSpecs[ExtraPos].ExtraID == extras.add_ball)
		{
			balls.push_back(ball);
			balls[balls.size() - 1].V = normalize(vec3(balls[0].V.y, balls[0].V.x, -1));
			balls[balls.size() - 1].Pos = vec3(balls[0].Pos.y, balls[0].Pos.x, balls[0].Pos.z);
		}
		else if (GG.cbsSpecs[ExtraPos].ExtraID == extras.sticky_board)
		{
			StickyBoard = true;
		}
		GG.cbsSpecs[ExtraPos].ExtraID = -1;
	}
}