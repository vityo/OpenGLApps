#include <iostream>
#include <map>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
GLfloat timeLast = 0.0f;

map<int, bool> keys; // зажатость управляющих камерой кнопок

#pragma region user_input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (keys.find(key) != keys.end()) {
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	GLfloat xPosGL = static_cast<GLfloat>(xPos);
	GLfloat yPosGL = static_cast<GLfloat>(yPos);

	if (firstMouse) {
		lastX = xPosGL;
		lastY = yPosGL;
		firstMouse = false;
	}

	GLfloat xoffset = xPosGL - lastX;
	GLfloat yoffset = lastY - yPosGL;
	lastX = xPosGL;
	lastY = yPosGL;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<GLfloat>(yoffset));
}

void do_movement(GLfloat deltaTime) {
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}
#pragma endregion

GLuint loadTexture(GLchar* path) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return textureID;
}

int main() {
	#pragma region setup_window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);

	if (!window) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// управление
	keys[GLFW_KEY_W] = false;
	keys[GLFW_KEY_S] = false;
	keys[GLFW_KEY_A] = false;
	keys[GLFW_KEY_D] = false;
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cout << "Failed to initialize GLEW" << endl;
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	#pragma endregion

	#pragma region object_initialization
	GLfloat quadVertices[] = {
		// Positions     // Colors
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};

	glm::vec2 translations[100];
	int index = 0;
	GLfloat offset = 0.1f;
	for (GLint y = -10; y < 10; y += 2)
	{
		for (GLint x = -10; x < 10; x += 2)
		{
			glm::vec2 translation;
			translation.x = (GLfloat)x / 10.0f + offset;
			translation.y = (GLfloat)y / 10.0f + offset;
			translations[index++] = translation;
		}
	}

	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribDivisor(2, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	#pragma endregion

	// шейдеры
	Shader shader("../Shaders/vertexShader.vsh", "", "../Shaders/fragmentShader.fsh");

	while (!glfwWindowShouldClose(window)) {
		GLfloat timeCurrent = static_cast<GLfloat>(glfwGetTime());
		GLfloat deltaTime = timeCurrent - timeLast;
		timeLast = timeCurrent;

		glfwPollEvents();
		do_movement(deltaTime);

		glEnable(GL_DEPTH_TEST);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		glBindVertexArray(quadVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	// удаляем, завершаем
	glDeleteBuffers(1, &instanceVBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);

	glfwTerminate();

	return 0;
}