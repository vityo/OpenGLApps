#include <iostream>
#include <map>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;

map<int, bool> keys; // зажатость управляющих камерой кнопок

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

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);

	if (!window) {
		cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	// шейдеры
	Shader shader("../Shaders/vertexShader.vsh", "../Shaders/fragmentShader.fsh");

	// модель
	Model modelObj("../Resources/nanosuit/nanosuit.obj");

	// позиции
	glm::vec3 pointLightPositions[] = {
		glm::vec3(2.3f, -1.6f, -3.0f),
		glm::vec3(-1.7f, 0.9f, 1.0f)
	};

	// управление
	keys[GLFW_KEY_W] = false;
	keys[GLFW_KEY_S] = false;
	keys[GLFW_KEY_A] = false;
	keys[GLFW_KEY_D] = false;
	
	// время кадра
	GLfloat deltaTime = 0.0f;
	GLfloat timeLast = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		GLfloat timeCurrent = static_cast<GLfloat>(glfwGetTime());
		deltaTime = timeCurrent - timeLast;
		timeLast = timeCurrent;

		glfwPollEvents();
		do_movement(deltaTime);

		glEnable(GL_DEPTH_TEST);

		glClearColor(0.5f, 0.45f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<GLfloat>(WIDTH) / static_cast<GLfloat>(HEIGHT), 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glUniform3f(glGetUniformLocation(shader.program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);

		glUniform3f(glGetUniformLocation(shader.program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(shader.program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader.program, "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shader.program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader.program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader.program, "pointLights[0].linear"), 0.009);
		glUniform1f(glGetUniformLocation(shader.program, "pointLights[0].quadratic"), 0.0032);

		glUniform3f(glGetUniformLocation(shader.program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(shader.program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader.program, "pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shader.program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader.program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader.program, "pointLights[1].linear"), 0.009);
		glUniform1f(glGetUniformLocation(shader.program, "pointLights[1].quadratic"), 0.0032);

		modelObj.Draw(shader);

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}