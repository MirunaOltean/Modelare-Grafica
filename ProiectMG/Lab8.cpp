#include <glad/glad.h>
#include <glfw3.h>

#include <filesystem>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION  
#include <stb_image.h>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(-20.0f, 20.0f, 5.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float kAValue = 0.5;

int floorTexture, roadTexture, road2Texture;

unsigned int planeVAO, planeVBO;
unsigned int quadVAO, quadVBO;
unsigned int cloudsVAO, cloudsVBO;
unsigned int roadVAO, roadVBO;
unsigned int road2VAO, road2VBO;

void DrawFloor(Shader& shader)
{
	float planeVertices[] = {
		// positions          // texture Coords 
		//x      //z
	   -70.0f, -0.5f,  70.0f,  0.0f, 2.0f,    //1
		70.0f, -0.5f,  70.0f,  2.0f, 2.0f,    //4
		70.0f, -0.5f, -70.0f,  2.0f, 0.0f,    //3

		70.0f, -0.5f, -70.0f,  2.0f, 0.0f,   //5
	   -70.0f, -0.5f, -70.0f,  0.0f, 0.0f,   //6
	   -70.0f, -0.5f,  70.0f,  0.0f, 2.0f     //2
	};

	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	shader.use();
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 110.0f);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	glBindVertexArray(planeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	shader.setMat4("model", glm::mat4(1.0f));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void DrawRoad(Shader& shader)
{
	float roadVertices[] = {

	   -60.5f,   -0.4f,  -20.0f,   0.0f,  1.0f,   //top-left
		60.5f,   -0.4f,  -20.0f,   1.0f,  1.0f,   //top-right
		60.5f,   -0.4f,  -60.0f,   1.0f,  0.0f,   //bottom-right

		60.5f,   -0.4f,  -60.0f,   1.0f,  0.0f,   //bottom-right
	   -60.5f,   -0.4f,  -60.0f,   0.0f,  0.0f,   // bottom-left
	   -60.5f,   -0.4f,  -20.0f,   0.0f,  1.0f    //top-left
	};

	float road2Vertices[] = {

	   -60.5f,   -0.4f,   50.0f,   0.0f,  1.0f,   //top-left
	   -20.5f,   -0.4f,   50.0f,   1.0f,  1.0f,   //top-right
	   -20.5f,   -0.4f,  -20.0f,   1.0f,  0.0f,   //bottom-right

	   -20.5f,   -0.4f,  -20.0f,   1.0f,  0.0f,   //bottom-right
	   -60.5f,   -0.4f,  -20.0f,   0.0f,  0.0f,   // bottom-left
	   -60.5f,   -0.4f,   50.0f,   0.0f,  1.0f    //top-left
	};

	// road
	glGenVertexArrays(1, &roadVAO);
	glGenBuffers(1, &roadVBO);
	glBindVertexArray(roadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, roadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(roadVertices), &roadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	//road2
	glGenVertexArrays(1, &road2VAO);
	glGenBuffers(1, &road2VBO);
	glBindVertexArray(road2VAO);
	glBindBuffer(GL_ARRAY_BUFFER, road2VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(road2Vertices), &road2Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	shader.use();
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 110.0f);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	// road
	glBindVertexArray(roadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, roadTexture);
	shader.setMat4("model", glm::mat4(1.0f));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// road2
	glBindVertexArray(road2VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, road2Texture);
	shader.setMat4("model", glm::mat4(1.0f));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void DrawQuad()
{
	float quadVertices[] = { 
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void DrawObjects(Shader& ourShader,Model plane,Model tower,Model hangar,Model trees)
{
	ourShader.use();
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 110.0f);
	ourShader.setMat4("view", view);
	ourShader.setMat4("projection", projection);

	//plane 1
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.4f, 40.0f));
	ourShader.setMat4("model", model);
	plane.Draw(ourShader);

	//plane 2
    glm::mat4 model2 = glm::mat4(1.0f);
	model2 = glm::translate(model2, glm::vec3(45.0f, 1.4f, 0.0f));
	model2 = glm::rotate(model2,glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ourShader.setMat4("model", model2);
	plane.Draw(ourShader);

	//control-tower
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-20.0f, -0.5f, -20.0f));
	ourShader.setMat4("model", model);
	tower.Draw(ourShader);

	//hangar 1
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(45.0f, -0.5f, 45.0f));
	ourShader.setMat4("model", model);
	hangar.Draw(ourShader);

	//hangar 2
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -0.5f, 45.0f));
	ourShader.setMat4("model", model);
	hangar.Draw(ourShader);

	//trees
	float x = 60.0f;
	for (int i = 0; i < 7; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, -0.5f, -15.0f));
		ourShader.setMat4("model", model);
		trees.Draw(ourShader);
		x = x - 12.0f;
	}

	x = 65.0f;
	for(int i=0;i<12;i++)
	{ 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, -0.5f, -65.0f));
		ourShader.setMat4("model", model);
		trees.Draw(ourShader);
		x = x - 12.0f;
	}
}

int main(int argc, char** argv)
{
	std::string strFullExeFileName = argv[0];
	std::string strExePath;
	const size_t last_slash_idx = strFullExeFileName.rfind('\\');
	if (std::string::npos != last_slash_idx)
	{
		strExePath = strFullExeFileName.substr(0, last_slash_idx);
	}

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	glm::vec3 lightPos(0.0f, 0.0f, 2.0f);

	Shader shader("5.1.framebuffers.vs", "5.1.framebuffers.fs");
	Shader screenShader("5.1.framebuffers_screen.vs", "5.1.framebuffers_screen.fs");
	Shader ourShader("model_loading.vs", "model_loading.fs");


	Model plane(std::filesystem::path("Resources/plane.obj").string());
	Model tower(std::filesystem::path("Resources/Tower_Control.obj").string());
	Model hangar(std::filesystem::path("Resources/Hangar.obj").string());
	Model trees(std::filesystem::path("Resources/Trees/Tree.obj").string());

	floorTexture = loadTexture(std::filesystem::path("Resources/grass.jpg").string().c_str());
	roadTexture = loadTexture(std::filesystem::path("Resources/road.jpg").string().c_str());
	road2Texture = loadTexture(std::filesystem::path("Resources/road.jpg").string().c_str());

	shader.use();
	shader.setInt("texture1", 0);

	screenShader.use();
	screenShader.setInt("screenTexture", 0);

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); 
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); 
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// draw as wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST); 

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawFloor(shader);
		DrawRoad(shader);
		DrawObjects(ourShader,plane,tower,hangar,trees);
	

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		DrawQuad();
		screenShader.use();
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	
		glDrawArrays(GL_TRIANGLES, 0, 6);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glDeleteVertexArrays(1, &planeVAO);
	glDeleteVertexArrays(1, &roadVAO);
	glDeleteVertexArrays(1, &road2VAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &roadVBO);
	glDeleteBuffers(1, &road2VBO);
	glDeleteBuffers(1, &quadVBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
