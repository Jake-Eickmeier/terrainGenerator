/************************************************
 *
 *          CSCI 4110 Assignment One
 *
 *  By Jake Eickmeier
 *
 ************************************************/

#include <Windows.h>
#include <gl/glew.h>
#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaders.h"
#include <stdio.h>
#include <iostream>

#include <vector>
#include <random>
#include <cstdlib>

GLuint program;			// shader programs
GLuint objVAO;			// the data to be displayed

unsigned int VAO;

int triangles;			// number of triangles
int window;

char vertexName[] = "a";
char fragmentName[] = "b";

double theta, phi;
double r;
float movementAmount;
bool wireFrameMode = false;

glm::mat4 projection;	// projection matrix

std::random_device rd;		//Generate a random seed 
std::default_random_engine generator(rd());
std::normal_distribution<double> distribution(0.0, 1.0);
std::normal_distribution<double> initialHeightDistribution(2.0, 2.0);

// Position of camera and frontfacing vector
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 1.0f, -1.0f);

float mouseSensitivity = 0.075f;
int windowWidth = 512;
int windowHeight = 512;
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = float(windowWidth) / 2.0;
float lastY = float(windowHeight) / 2.0;
glm::vec3 upDirection = { 0.0f, 1.0f, 0.0f };

//New code
//Initialize height field user input variables
int x_size;
int y_size;
int x_res;
int y_res;
int* x_size_inp;
int* y_size_inp;
int* x_res_inp;
int* y_res_inp;
std::vector<float> initial_heights;
std::vector<std::vector<float>> heightMap;


//New code
//Square step function
std::vector<std::vector<float>> squareStep(int chunk_size, int square_size, std::vector<std::vector<float>> inputField) {
	std::vector<std::vector<float>> outputField = inputField;
	int rx = square_size;
	int ry = square_size;
	int half_square = chunk_size / 2;
	for (int r = 0; r < rx; r += chunk_size) {
		for (int c = 0; c < ry; c += chunk_size) {
			//If not out of bounds
			if (((r + half_square) < rx) && ((c + half_square) < ry)) {
				//heightField[r + half_square][c + half_square] = 10;
				float sum = 0.0f;
				int count = 1;
				sum += outputField[r][c];
				if (r + chunk_size < rx) {
					sum += outputField[r + chunk_size][c];
					count++;
				}
				if (c + chunk_size < ry) {
					sum += outputField[r][c + chunk_size];
					count++;
				}
				if ((r + chunk_size < rx) && (c + chunk_size < ry)) {
					sum += outputField[r + chunk_size][c + chunk_size];
					count++;
				}
				outputField[r + half_square][c + half_square] = float(sum / count);
				//outputField[r + half_square][c + half_square] += float(distribution(generator));
				//Below I incorporate some impact from the "level" of the algorithm into the random impact so the features appear more natural
				outputField[r + half_square][c + half_square] += (float(distribution(generator)) * float(float(chunk_size) / float(square_size)));
			}
		}
	}
	return outputField;
}

//New code
//Diamond step function
std::vector<std::vector<float>> diamondStep(int chunk_size, int square_size, std::vector<std::vector<float>> inputField) {
	std::vector<std::vector<float>> outputField = inputField;
	int rx = square_size;
	int ry = square_size;
	int half_square = chunk_size / 2;
	for (int r = 0; r < ry; r += half_square) {			//Iterate column on outside
		//for (int c = 0; c < rx; c += chunk_size) {		//Iterate row in inside
		for (int c = (r + half_square) % chunk_size; c < rx; c += chunk_size) {
			float sum = 0.0f;
			int count = 0;
			//sum += heightField[c][r];
			if (c - half_square >= 0) {
				sum += outputField[r][c - half_square];
				count++;
				//std::cout << "Adding c-hs" << std::endl;
			}
			if (c + half_square < ry) {
				sum += outputField[r][c + half_square];
				count++;
				//std::cout << "Adding c+hs" << std::endl;
			}
			if (r - half_square >= 0) {
				sum += outputField[r - half_square][c];
				count++;
				//std::cout << "Adding r-hs" << std::endl;
			}
			if (r + half_square < rx) {
				sum += outputField[r + half_square][c];
				count++;
				//std::cout << "Adding r+hs" << std::endl;
			}
			outputField[r][c] = float(sum / count);
			//Below I incorporate some impact from the "level" of the algorithm into the random impact so the features appear more natural
			outputField[r][c] += (float(distribution(generator)) * float(float(chunk_size) / float(square_size)));
			//std::cout << "R = " << r << ",  C = " << c << "  " << "    |    ";
			//std::cout << "Sum: " << sum << ",  count: " << count << std::endl;
		}
		//}
	}
	return outputField;
}

//New code
std::vector<std::vector<float>> constructInitialHeightField(int x, int y, int rx, int ry, std::vector<float> initial_z) {
	
	//Construct dynamic 2D array from given resolution
	std::vector<std::vector<float>> heightField(rx, std::vector<float> (ry, 0));
	heightField[0][0] = initial_z[0];
	heightField[rx - 1][0] = initial_z[1];
	heightField[0][ry - 1] = initial_z[2];
	heightField[rx - 1][ry - 1] = initial_z[3];

	int square_size = rx;
	int chunk_size = rx - 1;
	while (chunk_size > 1) {
		//std::cout << "Chunk Size: " << chunk_size << std::endl;
		heightField = squareStep(chunk_size, square_size, heightField);
		heightField = diamondStep(chunk_size, square_size, heightField);
		chunk_size /= 2;
		
	}
	/*	Below code is for printing heightmap, useful for debugging
	for (int i = 0; i < ry; i++) {
		for (int j = 0; j < rx; j++) {
			std::cout << heightField[i][j] << " ";
			//Below is some trickery just to get this to print nicely to the console for debugging purposes
			char buf[64];
			sprintf(buf, "%f", heightField[i][j]);
			for (int a = 2; a < 7; a++) {
				if (buf[a] == '0') {
					if (a == 2) {
						std::cout << " ";
					}
					std::cout << " ";
				}
			}
		}
		std::cout << std::endl;
	}
	*/
	return heightField;
}

/*
 *  The init procedure creates the OpenGL data structures
 *  that contain the triangle geometry, compiles our
 *  shader program and links the shader programs to
 *  the data.
 */
void init() {
	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;
	int vs;
	int fs;
	GLfloat *vertices;
	GLfloat *normals;
	GLuint *indices;

	int nv;
	int nn;
	int ni;
	int i;
	float xmin, ymin, zmin;
	float xmax, ymax, zmax;
	char vname[256];
	char fname[256];

	movementAmount = 0.25f;

	heightMap = constructInitialHeightField(x_size, y_size, x_res, y_res, initial_heights);

	/*  Retrieve the vertex coordinate data */
	
	nv = x_res * y_res;
	///*
	vertices = new GLfloat[nv * 3];
	int tmp_count = 0;
	for (int i = 0; i < x_res; i++) {
		for (int j = 0; j < y_res; j++) {
			//vertices[tmp_count] = heightMap[i][j];
			//TODO: not sure if below is proper implementation but testing it out nonetheless
			vertices[tmp_count] = GLfloat((GLfloat(i)/GLfloat(x_res))*GLfloat(x_size));					//X coords
			tmp_count++;
			vertices[tmp_count] = GLfloat((GLfloat(j)/GLfloat(y_res))*GLfloat(y_size));					//Y coords
			tmp_count++;
			vertices[tmp_count] = GLfloat(heightMap[i][j]);		//Z coords
			tmp_count++;
		}
	}
	/*	//Debugging print statements
	std::cout << "Vertices tmp count: " << tmp_count << std::endl;
	for (int i = 0; i < nv*3; i += 3) {
		std::cout << "X: " << vertices[i] << "  | Y: " << vertices[i+1] << "  | Z: " << vertices[i+2] << " " << std::endl;
	}
	*/

	/*  Retrieve the triangle indices */
	///*
	ni = (x_res - 1) * (x_res - 1) * 6;	//I believe based on some quick calculations that there are 6
											//indices used per quad, so calculate the number of quads and then multiply by 6
	triangles = ni / 3;
	indices = new GLuint[ni];
	tmp_count = 0;
	for (int i = 0; i < x_res-1; i++) {
		for (int j = 0; j < y_res-1; j++) {
			//First triangle
			indices[tmp_count] = i * x_res + j;
			tmp_count++;
			indices[tmp_count] = (i + 1) * (x_res) + j;
			tmp_count++;
			indices[tmp_count] = i * x_res + j + 1;
			tmp_count++;

			//Second Triangle
			indices[tmp_count] = i * x_res + j + 1;;
			tmp_count++;
			indices[tmp_count] = (i + 1) * (x_res) + j;
			tmp_count++;
			indices[tmp_count] = (i + 1) * (x_res) + j + 1;
			tmp_count++;
		}
	}
	/*	//Debugging print statements
	std::cout << "Indices tmp count: " << tmp_count << std::endl;
	for (int i = 0; i < ni; i += 3) {
		std::cout << "V1: " << indices[i] << "  | V2: " << indices[i + 1] << "  | V3: " << indices[i + 2] << " " << std::endl;
	}
	*/
	//*/

	/*  Retrieve the vertex normals */	
	nn = nv * 3;
	normals = new GLfloat[nn];
	tmp_count = 0;
	glm::vec3 tmp_normal;
	//for(i=0; i<nn; i++) {
	for (int i = 0; i < x_res; i++) {
		for (int j = 0; j < y_res; j++) {
			if ((i < x_res - 1) && (j < y_res -1)) {
				tmp_normal =
					glm::normalize(
						glm::cross(
							(glm::vec3(vertices[(tmp_count*3) + (x_res*3)], vertices[(tmp_count*3) + (x_res*3) + 1], vertices[(tmp_count*3) + (x_res*3) + 2]) -
								glm::vec3(vertices[(tmp_count*3)], vertices[(tmp_count*3) + 1], vertices[(tmp_count*3) + 2])),
							(glm::vec3(vertices[(tmp_count*3) + (x_res*3) + 3], vertices[(tmp_count*3) + (x_res*3) + 4], vertices[(tmp_count*3) + (x_res*3) + 5]) -
								glm::vec3(vertices[(tmp_count*3)], vertices[(tmp_count*3) + 1], vertices[(tmp_count*3) + 2]))
						)
					);
				normals[(tmp_count*3)] = GLfloat(tmp_normal.x);
				normals[(tmp_count * 3) + 1] = GLfloat(tmp_normal.y);
				normals[(tmp_count * 3) + 2] = GLfloat(tmp_normal.z);
				tmp_count++;
			}
			else {		//If reaching for other edges would push us out of bounds, just copy previously calculated normal
				normals[(tmp_count * 3)] = GLfloat(tmp_normal.x);
				normals[(tmp_count * 3) + 1] = GLfloat(tmp_normal.y);
				normals[(tmp_count * 3) + 2] = GLfloat(tmp_normal.z);
				tmp_count++;
			}
		}
	}
	/*	//Debugging print statements
	std::cout << "Normals tmp count: " << tmp_count << std::endl;
	for (int i = 0; i < nn; i += 3) {
		std::cout << "nx: " << normals[i] << "  | ny: " << normals[i + 1] << "  | nz: " << normals[i + 2] << " " << std::endl;
	}
	*/
	

	

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	/*
	 *  load the vertex coordinate data
	 */
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (nv*2)*3*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nv*3*sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nv*3*sizeof(GLfloat), nn*sizeof(GLfloat), normals);
	
	/*
	 *  load the vertex indexes
	 */
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni * sizeof(GLuint), indices, GL_STATIC_DRAW);

	/*
	 *  compile and build the shader program
	 */
	vs = buildShader(GL_VERTEX_SHADER, "terrainGenerator.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "terrainGenerator.fs");
	program = buildProgram(vs,fs,0);

	/*
	 *  link the vertex coordinates to the vPosition
	 *  variable in the vertex program.  Do the same
	 *  for the normal vectors.
	 */
	glUseProgram(program);
	vPosition = glGetAttribLocation(program,"vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0); //new
	glEnableVertexAttribArray(vPosition);
	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*) (nv*3*sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(vNormal);

}

/*
 *  Executed each time the window is resized,
 *  usually once at the start of the program.
 */
void framebufferSizeCallback(GLFWwindow *window, int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).

	if (h == 0)
		h = 1;

	float ratio = 1.0f * w / h;

	glfwMakeContextCurrent(window);

	glViewport(0, 0, w, h);

	projection = glm::perspective(0.7f, ratio, 1.0f, 800.0f);

}

/*
 *  This procedure is called each time the screen needs
 *  to be redisplayed
 */
void display() {
	glm::mat4 view;
	int modelViewLoc;
	int projectionLoc;
	int normalLoc;

	view = glm::lookAt(cameraPos,
			cameraFront + cameraPos,
			upDirection);

	glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view)));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	
	modelViewLoc = glGetUniformLocation(program,"modelView");
	glUniformMatrix4fv(modelViewLoc, 1, 0, glm::value_ptr(view));
	projectionLoc = glGetUniformLocation(program,"projection");
	glUniformMatrix4fv(projectionLoc, 1, 0, glm::value_ptr(projection));
	normalLoc = glGetUniformLocation(program,"normalMat");
	glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));

	
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 3*triangles, GL_UNSIGNED_INT, 0);

}

/*
 *  Called each time a key is pressed on
 *  the keyboard.
 */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += movementAmount * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= movementAmount * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, upDirection)) * movementAmount;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, upDirection)) * movementAmount;
	}


	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		cameraPos -= glm::vec3(0.0f, 0.0f, 1.0f);
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		cameraPos += glm::vec3(0.0f, 0.0f, 1.0f);
	}

	if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
		if (wireFrameMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireFrameMode = false;
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireFrameMode = true;
		}
		
	}
	
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos; 
	lastX = xpos;
	lastY = ypos;

	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	
	glm::vec3 tmp_front;
	tmp_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	tmp_front.y = sin(glm::radians(pitch));
	tmp_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(tmp_front);

}

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char **argv) {
	GLFWwindow *window;

	//New code
	//Parse user input, gather required inputs for height field generation

	if (argc == 1) {	//If the user doesn't specify inputs, just choose something that will look decent
		x_size = 6;
		y_size = 6;
		x_res = 513;	//These need to be (2^n) +1
		y_res = 513;
		initial_heights = { 1.0f, 3.0f, 4.0f, 2.0f };
	}
	else if (argc == 5) {
		x_size = atoi(argv[1]);
		y_size = atoi(argv[2]);
		x_res = atoi(argv[3]);
		y_res = atoi(argv[4]);
		initial_heights = { 1.0f, 3.0f, 4.0f, 2.0f };	//Default initial heights if user doesn't input any - likely to look decent first time
	}
	else if (argc == 6) {
		x_size = atoi(argv[1]);
		y_size = atoi(argv[2]);
		x_res = atoi(argv[3]);
		y_res = atoi(argv[4]);
		if (std::string(argv[5]) == "r" || std::string(argv[5]) == "random") {
			int generating = true;
			float a = float(initialHeightDistribution(generator));
			float b = float(initialHeightDistribution(generator));
			float c = float(initialHeightDistribution(generator));
			float d = float(initialHeightDistribution(generator));
			while (generating) {
				if (a > 0 && b > 0 && c > 0 && d > 0) {			//Having negative numbers will mess up height field calculations so we want to avoid it
					generating = false;
				}
				else {		//Generate a new set until a set with only positive numbers is created
					a = float(initialHeightDistribution(generator));
					b = float(initialHeightDistribution(generator));
					c = float(initialHeightDistribution(generator));
					d = float(initialHeightDistribution(generator));
				}
			}
			
			initial_heights = { a, b, c, d };
		}
		else {
			initial_heights = { 1.0f, 3.0f, 4.0f, 2.0f };
		}
	}
	else if (argc == 9) {
		x_size = atoi(argv[1]);
		y_size = atoi(argv[2]);
		x_res = atoi(argv[3]);
		y_res = atoi(argv[4]);
		initial_heights = { 
			float(atoi(argv[5])),
			float(atoi(argv[6])),
			float(atoi(argv[7])),
			float(atoi(argv[8]))
		};
	}
	else {	//If the user doesn't specify valid inputs, just choose something that will look decent and alert the user
		x_size = 6;
		y_size = 6;
		x_res = 513;	//These need to be (2^n) +1
		y_res = 513;
		initial_heights = { 1.0f, 3.0f, 4.0f, 2.0f };
		std::cout << "Input unsuported, running program with default parameters" << std::endl;
	}

	// start by setting error callback in case something goes wrong
	glfwSetErrorCallback(error_callback);

	// initialize glfw

	if (!glfwInit()) {
		fprintf(stderr, "can't initialize GLFW\n");
	}

	// create the window used by our application

	window = glfwCreateWindow(windowWidth, windowHeight, "Terrain Generation Result", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	
	glfwSetCursorPosCallback(window, mouse_callback);
	// establish framebuffer size change and input callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwSetKeyCallback(window, key_callback);
	/*
	 *  initialize glew
	 */
	glfwMakeContextCurrent(window);
	GLenum error = glewInit();
	if(error != GLEW_OK) {
		printf("Error starting GLEW: %s\n",glewGetErrorString(error));
		exit(0);
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	theta = 0.5;
	phi = 1.5;
	r = 2.0;

	init();

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0,1.0,1.0,1.0);
	glViewport(0, 0, 512, 512);

	projection = glm::perspective(0.7f, 1.0f, 1.0f, 800.0f);

	glfwSwapInterval(1);

	// GLFW main loop, display model, swapbuffer and check for input

	while (!glfwWindowShouldClose(window)) {
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

}