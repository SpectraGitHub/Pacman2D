//Standard libraries
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

//Glad & GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Shaders
#include "ghost.h"
#include "pacman.h"
#include "..\..\shaders\shader.h"  

//GLM
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"


using namespace std;

void lerp(GLuint shader, Move* entity, float sizex, float sizey, float offsetx, float offsety);
void printMap();
void nextMove(int x, int y);
void readLevel(string path);
void Transform(const GLuint shader, float x, float y);
void inputHandler();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void resizeCallback(GLFWwindow* window, int width, int height);
GLuint CreateSquare(double x, double y, float size);

//Game logic
vector<vector<int>> level; //Level 2D array
bool gameOver = false;
bool started = false;
float timeTracker = 0;
float deltaTime = 0;

//Screen
const float WIDTH = 280;
const float HEIGHT = 360;
const float ASPECT = WIDTH/HEIGHT;   // desired aspect ratio

//Player and ghosts
Pacman *pac;
Ghost* ghost;

int main() {
	readLevel("../../../levels/level0");
	 
	//Initialises GLFW
	if (!glfwInit()) {
		cerr << "GLFW Failed initializing. \n";
		cin.get();
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Hints for Window (version & profile)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creates the Window
	auto* window = glfwCreateWindow(WIDTH, HEIGHT, "Pacman", NULL, NULL);

	// Error check for Window
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Sets OpenGL context
	glfwMakeContextCurrent(window);

	//Initialises GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Wall vertex shader
	auto wallVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(wallVertexShader, 1, &wallVertexShaderSrc, NULL);
	glCompileShader(wallVertexShader);


	// Wall fragment shader
	auto wallFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(wallFragmentShader, 1, &wallFragmentShaderSrc, NULL);
	glCompileShader(wallFragmentShader);

	// Create wall shader program
	auto wallShaderProgram = glCreateProgram();
	glAttachShader(wallShaderProgram, wallVertexShader);
	glAttachShader(wallShaderProgram, wallFragmentShader);
	glLinkProgram(wallShaderProgram);

	// Pacman vertex shader
	auto pacmanVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(pacmanVertexShader, 1, &pacmanVertexShaderSrc, NULL);
	glCompileShader(pacmanVertexShader);

	// ghost vertex shader
	auto ghostVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(ghostVertexShader, 1, &ghostVertexShaderSrc, NULL);
	glCompileShader(ghostVertexShader);

	// Entity Fragment Shader
	auto entityFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(entityFragmentShader, 1, &entityFragmentShaderSrc, NULL);
	glCompileShader(entityFragmentShader);

	// Create pacman shader program
	auto pacmanShaderProgram = glCreateProgram();
	glAttachShader(pacmanShaderProgram, pacmanVertexShader);
	glAttachShader(pacmanShaderProgram, entityFragmentShader);
	glLinkProgram(pacmanShaderProgram);

	// Create ghost shader program
	auto ghostShaderProgram = glCreateProgram();
	glAttachShader(ghostShaderProgram, ghostVertexShader);
	glAttachShader(ghostShaderProgram, entityFragmentShader);
	glLinkProgram(ghostShaderProgram);

	// Create pellet shader program
	auto pelletShaderProgram = glCreateProgram();
	glAttachShader(pelletShaderProgram, pacmanVertexShader);
	glAttachShader(pelletShaderProgram, entityFragmentShader);
	glLinkProgram(pelletShaderProgram);

	// Screen scaling
	glfwSetWindowSizeCallback(window, resizeCallback); // assign callback function
	glfwSetWindowSize(window, level.size()*10*2, level[1].size()*10*2); // Set new size

	//Pellet array creation
	vector<vector<GLuint>> pellets;
	//prepare array
	pellets = vector<vector<GLuint>>(level.size());
	for (int i = 0; i < level.size(); i++) {
		pellets[i] = vector<GLuint>(level[1].size());
	}

	// Square size everything is dependant on
	float sizex = 2 / (float)level.size();
	float sizey = 2 / (float)level[1].size();

	//create walls and pellets
	vector<GLuint> walls;
	for (int y = 0; y < level[1].size(); y++) {
		for (int x = 0; x < level.size(); x++) {
			if (level[x][y] == 1) { // if wall
				// scale * position - align map
				double a =  sizex*x-1+sizex/2;
				double b =  sizey*y-1+sizey/2;
				walls.push_back(CreateSquare(a, -b, sizex-0.02f));
			}
			else if (level[x][y] == 0 || level[x][y] == 3) { // if pellet or ghost (ghost cant eat pellets)
				double a = sizex * x - 1 + sizex / 2;
				double b = sizey * y - 1 + sizey / 2;
				pellets[x][y] = CreateSquare(a, -b, sizex / 4);
			}
		}
	}

	
	//Create pacman TODO: Change out for sprite rendering
	double a = sizex * pac->x - 1 + sizex / 2;
	double b = sizey * pac->y - 1 + sizey / 2;
	GLuint pacman = CreateSquare(a, -b, sizex - 0.02f);


	//create ghost TODO: change out for sprite rendering
	a = sizex * ghost->x - 1 + sizex / 2;
	b = sizey * ghost->y - 1 + sizey / 2;
	GLuint Ghost = CreateSquare(a, -b, sizex - 0.02f);


	// key catcher
	glfwSetKeyCallback(window, key_callback);

	double lastFrame = glfwGetTime();

	// ----------------------------
	// MAIN LOOP
	// ----------------------------
	while (!glfwWindowShouldClose(window)) {

		//time keeping
		timeTracker = glfwGetTime();
		deltaTime = timeTracker - lastFrame;
		lastFrame = timeTracker;


		//Game logic and input
		if (!gameOver && started) {
			ghost->checkForPacman(); // checks to kill pacman

			inputHandler(); // applies user inputs
			if ((timeTracker - 0.25) >= ghost->lastTime) { //ghost' move interval
				ghost->move(timeTracker);
			}
		}

		// Clear buffers to preset values
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw map
		for (int i = 0; i < walls.size(); i++) {
			glUseProgram(wallShaderProgram);
			glBindVertexArray(walls[i]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)0);
		}

		//Draw pellets
		int p = 0;
		for (int x = 0; x < level.size(); x++) {
			for (int y = 0; y < level[0].size(); y++) {
				if (level[x][y] == 0) {
					p++;
					glUseProgram(pelletShaderProgram);
					glBindVertexArray(pellets[x][y]);
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)0);
				}
			}
		}
		if (p == 0 && !gameOver) {
			cout << "Win!";
			gameOver = true;
		}

		//Draw pacman
		glUseProgram(pacmanShaderProgram);
		glBindVertexArray(pacman);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)0);

		if (pac->transform) { // if moving
			float offsetx = sizex * pac->startPosx;
			float offsety = sizey * pac->startPosy;
			lerp(pacmanShaderProgram, pac, sizex, sizey, offsetx, offsety);
		}

		//Draw ghost
		glUseProgram(ghostShaderProgram);
		glBindVertexArray(Ghost);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)0);

		if (ghost->transform) { // if moving
			float offsetx = sizex * -ghost->startPosx;
			float offsety = sizey * ghost->startPosy;
			lerp(ghostShaderProgram, ghost, sizex, sizey, offsetx, offsety);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;
	}

	//Termination of Stuff 
	glfwTerminate();

	glDeleteShader(wallVertexShader);
	glDeleteShader(wallFragmentShader);
	glDeleteProgram(wallShaderProgram);

	glDeleteShader(pacmanVertexShader);
	glDeleteShader(ghostVertexShader);
	glDeleteShader(entityFragmentShader);
	glDeleteProgram(pacmanShaderProgram);

	glDeleteProgram(ghostShaderProgram);

	glDeleteProgram(pelletShaderProgram);


	return 0;
}

/// <summary>
/// Changes size of screen and everything in it
/// </summary>
/// <param name="window"> GLFW window </param>
/// <param name="width"> New width </param>
/// <param name="height"> New height </param>
void resizeCallback(GLFWwindow* window, int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	int w = height * ASPECT;           // w is width adjusted for aspect ratio
	int left = (width - w) / 2;
	glViewport(left, 0, w, height);       // fix up the viewport to maintain aspect ratio
	glMatrixMode(GL_MODELVIEW);
}

//Input controller variables
bool input[4] = { false, false, false, false };
int inputID;
bool waitingInput = false;

/// <summary>
/// Called once every key is pressed and processes the command if valid
/// </summary>
/// <param name="window"> GLFW window </param>
/// <param name="key"> Key that was pressed </param>
/// <param name="scancode"> Not used, but required for method </param>
/// <param name="action"> Key press / release </param>
/// <param name="mods"> Not used, but required for method </param>
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	started = true; // starts the game
	int tempDirx = 0;
	int tempDiry = 0;
	inputID = key - 262;

	// if key press
	if (action == GLFW_PRESS) {
		switch (inputID) {
			case 0:	tempDirx =  1; tempDiry =  0;  break;
			case 1:	tempDirx = -1; tempDiry =  0;  break;
			case 2: tempDirx =  0; tempDiry =  1;  break;
			case 3: tempDirx =  0; tempDiry = -1;  break;
			default: return;
		}

		//Checks if input is valid, if not store it untill it is valid
		if (pac->checkDir(tempDirx, tempDiry)) {
			input[inputID] = true;
			pac->dirx = tempDirx; pac->diry = tempDiry;
		}
		else {
			nextMove(tempDirx,tempDiry);
		}
	}
	
	// if key release
	if (action == GLFW_RELEASE) {
		if (input[inputID]) {
			input[inputID] = false;
		}
	}
}

/// <summary>
/// Takes all player input and makes sure to not repeat actions and
/// apply waiting inputs
/// </summary>
void inputHandler() {

	if ((timeTracker - 0.25) >= pac->lastTime) { // if time for move
		if (waitingInput && pac->checkDir(pac->nextDirX, pac->nextDirY)) { // if stored input waiting && move is possible
			pac->dirx = pac->nextDirX; pac->diry = pac->nextDirY;
			waitingInput = false;
			pac->lastTime = timeTracker;
			pac->movePac(pac->dirx,pac->diry);
			//printMap();
			}
		else if(pac->checkDir(pac->dirx, pac->diry)){ // else use current direction
			pac->lastTime = timeTracker;
			pac->movePac(pac->dirx, pac->diry);
			//printMap();
		}
	}
}

/// <summary>
///  Takes input to be used in the future
/// </summary>
/// <param name="x"> new x direction </param>
/// <param name="y"> new y direction </param>
void nextMove(int x, int y) {
	waitingInput = true;
	pac->nextDirX = x;
	pac->nextDirY = y;
}

/// <summary>
/// Debug tool for printing mapstate to command line
/// </summary>
void printMap() {
	system("CLS");
	for (int i = 0; i < level[1].size(); i++) {
		for (int j = 0; j < level.size(); j++) {
			if (level[j][i] == 1) {
				cout << "L" << " ";
			}
			else if(level[j][i] == 2) {
				cout << "o" << " ";
			} else if(level[j][i] == 3) {
				cout << "3" << " ";
			}else {
				cout << "''";
			}
		}
		cout << endl;
	}
}

/// <summary>
//	Reads level file from path and sets it as current level
//  Takes any xand y dimensions from level file(highly flexible)
/// </summary>
/// <param name="path"> Path to level file</param>
void readLevel(string path) {
	int data;
	ifstream lvlFile(path);
    if (lvlFile)
    {	
		string size;
		lvlFile >> size;
		int xMax = stoi(size.substr(0,2));
		int yMax = stoi(size.substr(3));


		//Build array
		level = vector<vector<int>>(xMax);
		for (int i = 0; i < xMax; i++) {
			level[i] = vector<int>(yMax);
		}

		// Print current level
		cout << xMax << "*" << yMax << endl;
		for (int i = 0; i < yMax; i++) {
			for (int j = 0; j < xMax; j++) {
				lvlFile >> level[j][i];

				if (level[j][i] == 2) {
					pac = new Pacman(j, i);
				}

				cout << level[j][i] << " ";
			}
			cout << endl;
		}

    } else{
        cout << "\n --Unable to read file " << path;
	}
    lvlFile.close();

	
	//Generate ghost position
	//RNG seeded by current time in seconds since January 1st, 1970

	srand(time(NULL));
	int randX, randY;
	do {
		randX = rand() % (level.size()-1);
		randY = rand() % (level[1].size()-1);
	} while (level[randX][randY] != 0); // checks for tunnel
	ghost = new Ghost(randX, randY);
}

/// <summary>
///  Linearly interpolates from position a to b and considers if the entity is
///	 teleporting across the map, and should then NOT interpolate
/// </summary>
/// <param name="shader"> Entities shader to be transformed </param>
/// <param name="entity"> Entity to interpolate </param>
/// <param name="sizex"> needed for correct position calculations </param>
/// <param name="sizey"> needed for correct position calculations </param>
/// <param name="offsetx"> offset on x axis </param>
/// <param name="offsety"> offset on y axis </param>
void lerp(GLuint shader, Move *entity, float sizex, float sizey, float offsetx, float offsety) {
	//Wrapping functionality to stop entity from flying across the screen
	if (entity->wrap) {
		entity->linTime = 1;
	}
	else {
		entity->linTime += deltaTime * 5;
	}

	if (entity->linTime <= 1) {
		entity->wrap = false;
		float linx = (((sizex * (float)entity->pastX) * (1.0 - entity->linTime)) + (sizex * (float)entity->x * entity->linTime));
		float liny = (((-sizey * (float)entity->pastY) * (1.0 - entity->linTime)) - (sizey * (float)entity->y * entity->linTime));
		float x = offsetx + linx;
		float y = offsety + liny;
		Transform(shader, x, y);
	}
	else {
		entity->linTime = 0;
		entity->transform = false;
	}
}

/// <summary>
///  Transforms shaderProgram to new position in world
/// </summary>
/// <param name="shader"></param>
/// <param name="x"></param>
/// <param name="y"></param>
void Transform(const GLuint shader, float x, float y) {
	glm::mat4 transformation = glm::translate(glm::mat4(1), glm::vec3(x, y, 0));

	GLuint transformationmat = glGetUniformLocation(shader, "u_TransformationMat");
	glUniformMatrix4fv(transformationmat, 1, false, glm::value_ptr(transformation));
}

/// <summary>
///  Creates a square
/// </summary>
/// <param name="x"> position x </param>
/// <param name="y"> position y </param>
/// <param name="size"> size of square </param>
/// <returns></returns>
GLuint CreateSquare(double x, double y, float size)
{
	float s = size / 2;
	GLfloat square[4 * 3] =
	{
	  -s + x,  s + y, 0.0f,
	  -s + x, -s * ASPECT + y, 0.0f,
	   s + x, -s * ASPECT + y, 0.0f,
	   s + x,  s + y, 0.0f,
	};

	GLuint square_indices[6] = { 0,1,2,0,2,3 };

	GLuint vao;
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);

	GLuint ebo;
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (const void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);

	return vao;
}