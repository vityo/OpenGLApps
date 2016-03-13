#include <iostream>
#include <map>
#include <string>
#include <random>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

const GLuint WIDTH = 800, HEIGHT = 600;
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
Camera camera(glm::vec3(0.0f, 0.0f, 30.0f));
bool firstMouse = true;
GLfloat timeLast = 0.0f;

std::map<int, bool> keys; // зажатость управляющих камерой кнопок

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
		std::cout << "Failed to create GLFW window" << std::endl;
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
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	#pragma endregion

	#pragma region object_initialization
	Model modelRock = Model("../Resources/rock/rock.obj");
	Model modelPlanet = Model("../Resources/planet/planet.obj");

	GLfloat radius = 150.0f;
	GLfloat offset = 25.0f;
	GLuint amount = 100000;
	vector<glm::mat4> modelMatrices;
	modelMatrices.resize(amount);

	random_device rd;
	mt19937 generator(rd());
	uniform_real_distribution<GLfloat> distribution(0.0f, 1.0f);

	generate_n(modelMatrices.begin(), amount, [&amount, &radius, &offset, &generator, &distribution]() -> glm::mat4 {
		glm::mat4 model;
		static int i = 0;
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		++i;
		GLfloat displacement = distribution(generator) * 2.0f * offset - offset;
		GLfloat x = sin(angle) * radius + displacement;
		displacement = distribution(generator) * 2.0f * offset - offset;
		GLfloat y = displacement * 0.4f;
		displacement = distribution(generator) * 2.0f * offset - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));
		GLfloat scale = distribution(generator) * 0.2f + 0.05f;
		model = glm::scale(model, glm::vec3(scale));
		GLfloat rotAngle = distribution(generator) * 360.0f;
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
		return model;
	});

	for(Mesh mesh : modelRock.meshes) {
		GLuint VAO = mesh.VAO;

		GLuint buffer;
		glBindVertexArray(VAO);
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

		GLsizei vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(2 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(3 * vec4Size));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	#pragma endregion

	// шейдеры
	Shader shader("../Shaders/vertexShader.vsh", "", "../Shaders/fragmentShader.fsh");
	Shader shaderInstance("../Shaders/vertexShaderInstance.vsh", "", "../Shaders/fragmentShader.fsh");

	while (!glfwWindowShouldClose(window)) {
		GLfloat timeCurrent = static_cast<GLfloat>(glfwGetTime());
		GLfloat deltaTime = timeCurrent - timeLast;
		timeLast = timeCurrent;

		glfwPollEvents();
		do_movement(deltaTime);

		glEnable(GL_DEPTH_TEST);

		glClearColor(0.0f, 0.0f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		GLuint modelLoc = glGetUniformLocation(shader.program, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<GLfloat>(WIDTH) / static_cast<GLfloat>(HEIGHT), 0.1f, 600.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		modelPlanet.Draw(shader);

	/*	for (GLuint i = 0; i < amount; i++) {
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrices[i]));
			modelRock.Draw(shader);
		}
		*/
		shaderInstance.use();

		glUniformMatrix4fv(glGetUniformLocation(shaderInstance.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderInstance.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindTexture(GL_TEXTURE_2D, modelRock.textures_loaded[0].id);

		for (Mesh mesh : modelRock.meshes) {
			glBindVertexArray(mesh.VAO);
			glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}

		glfwSwapBuffers(window);
	}

	// удаляем, завершаем
	glfwTerminate();

	return 0;
}