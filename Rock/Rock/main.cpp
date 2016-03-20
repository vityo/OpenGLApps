#include <iostream>
#include <map>

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

using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
GLfloat lastFrame = 0.0f;

map<int, bool> keys; // зажатость управляющих камерой кнопок

GLuint cubeVAO = 0;
GLuint cubeVBO = 0;
GLuint texture = 0;

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

GLuint loadTexture(GLchar* path) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return textureID;
}

void RenderCube() {
	if (cubeVAO == 0) {
		GLfloat vertices[] = {
			// Back face
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
			// Front face
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
			// Left face
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			// Right face
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
			// Bottom face
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// Top face
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void RenderScene(Shader& shader) {
	glm::vec3 colorAdd(0.0f, 0.0f, 0.0f);
	glm::mat4 model;
	model = glm::scale(model, glm::vec3(10.0));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDisable(GL_CULL_FACE);
	glUniform1i(glGetUniformLocation(shader.program, "reverse_normals"), static_cast<GLboolean>(true));
	glUniform3fv(glGetUniformLocation(shader.program, "colorAdd"), 1, &colorAdd[0]);
	RenderCube();
	glUniform1i(glGetUniformLocation(shader.program, "reverse_normals"), static_cast<GLboolean>(false));
	glEnable(GL_CULL_FACE);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	colorAdd = glm::vec3(1.0f, 0.0f, 0.0f);
	glUniform3fv(glGetUniformLocation(shader.program, "colorAdd"), 1, &colorAdd[0]);
	RenderCube();
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
	model = glm::scale(model, glm::vec3(1.5));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	colorAdd = glm::vec3(0.0f, 1.0f, 0.0f);
	glUniform3fv(glGetUniformLocation(shader.program, "colorAdd"), 1, &colorAdd[0]);
	RenderCube();
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	colorAdd = glm::vec3(0.0f, 0.0f, 1.0f);
	glUniform3fv(glGetUniformLocation(shader.program, "colorAdd"), 1, &colorAdd[0]);
	RenderCube();
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	colorAdd = glm::vec3(1.0f, 1.0f, 0.0f);
	glUniform3fv(glGetUniformLocation(shader.program, "colorAdd"), 1, &colorAdd[0]);
	RenderCube();
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(1.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	colorAdd = glm::vec3(0.0f, 1.0f, 1.0f);
	glUniform3fv(glGetUniformLocation(shader.program, "colorAdd"), 1, &colorAdd[0]);
	RenderCube();
}

int main() {
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

	// текстуры
	texture = loadTexture("../Resources/laminat.jpg");

	// шейдеры
	Shader shadowMapShader("../Shaders/shadow_mapping_depth.vsh", "../Shaders/shadow_mapping_depth.gsh", "../Shaders/shadow_mapping_depth.fsh");
	Shader shader("../Shaders/vertexShader.vsh", "", "../Shaders/fragmentShader.fsh");
	Shader shaderRock("../Shaders/rock.vsh", "", "../Shaders/rock.fsh");
	Shader shaderMirror("../Shaders/mirror.vsh", "../Shaders/mirror.gsh", "../Shaders/mirror.fsh");
	//Shader shaderMirror("../Shaders/vertexShader.vsh", "../Shaders/mirror.gsh", "../Shaders/fragmentShader.fsh");
	//Shader shaderMirror("../Shaders/mirror.vsh", "", "../Shaders/mirror.fsh");

	//Model rock("../Resources/stone/stone.obj");
	Model earth("../Resources/earth/earth.obj");
	//Model sculpt("../Resources/sculpt/sculpt.obj");
	//Model rock2("../Resources/Rock2/obj.obj");

	glm::vec3 lightPos(2.0f, 3.5f, 1.0f);
	glm::vec3 objectPos(0.0f, -3.0f, 0.0f);

	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	GLuint depthCubemap;
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

	for (GLuint i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "ERROR framebuffer is not complete" << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	const GLuint MIRROR_WIDTH = 1024, MIRROR_HEIGHT = 1024;
	GLuint mirrorFBO;
	glGenFramebuffers(1, &mirrorFBO);
	GLuint mirrorCubemap;
	glGenTextures(1, &mirrorCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mirrorCubemap);
	for (GLuint i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, MIRROR_WIDTH, MIRROR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
/*	GLuint depthMirrorCubemap;
	glGenTextures(1, &depthMirrorCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthMirrorCubemap);
	for (GLuint i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, MIRROR_WIDTH, MIRROR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	*/
/*	GLuint texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	*/

	glBindFramebuffer(GL_FRAMEBUFFER, mirrorFBO);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mirrorCubemap, 0);
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMirrorCubemap, 0);
//	glDrawBuffer(GL_NONE);
//	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "ERROR framebuffer is not complete" << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glClearColor(0.0f, 0.0f, 0.15f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
		GLfloat deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		do_movement(deltaTime);

		GLfloat aspect = static_cast<GLfloat>(SHADOW_WIDTH) / static_cast<GLfloat>(SHADOW_HEIGHT);
		GLfloat near = 1.0f;
		GLfloat far = 25.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
		vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		shadowMapShader.use();
		for (GLuint i = 0; i < 6; ++i) {
			glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.program, ("shadowTransforms[" + to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
		}
		glUniform1f(glGetUniformLocation(shadowMapShader.program, "far_plane"), far);
		glUniform3fv(glGetUniformLocation(shadowMapShader.program, "lightPos"), 1, &lightPos[0]);
		RenderScene(shadowMapShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<GLfloat>(WIDTH) / static_cast<GLfloat>(HEIGHT), 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniform3fv(glGetUniformLocation(shader.program, "lightPos"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(shader.program, "viewPos"), 1, &camera.Position[0]);
		glUniform1f(glGetUniformLocation(shader.program, "far_plane"), far);
		glUniform1i(glGetUniformLocation(shader.program, "diffuseTexture"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shader.program, "depthMap"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		RenderScene(shader);

		glViewport(0, 0, MIRROR_WIDTH, MIRROR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, mirrorFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderMirror.use();
		vector<glm::mat4> mirrorTransforms;
		mirrorTransforms.push_back(shadowProj * glm::lookAt(objectPos, objectPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		mirrorTransforms.push_back(shadowProj * glm::lookAt(objectPos, objectPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		mirrorTransforms.push_back(shadowProj * glm::lookAt(objectPos, objectPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		mirrorTransforms.push_back(shadowProj * glm::lookAt(objectPos, objectPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		mirrorTransforms.push_back(shadowProj * glm::lookAt(objectPos, objectPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		mirrorTransforms.push_back(shadowProj * glm::lookAt(objectPos, objectPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
		for (GLuint i = 0; i < 6; ++i) {
			glUniformMatrix4fv(glGetUniformLocation(shaderMirror.program, ("mirrorTransforms[" + to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(mirrorTransforms[i]));
		}
		glUniform3fv(glGetUniformLocation(shaderMirror.program, "lightPos"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(shaderMirror.program, "viewPos"), 1, &objectPos[0]);
		glUniform1f(glGetUniformLocation(shaderMirror.program, "far_plane"), far);
		glUniform1i(glGetUniformLocation(shaderMirror.program, "diffuseTexture"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shaderMirror.program, "depthMap"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		RenderScene(shaderMirror);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		

		glViewport(0, 0, WIDTH, HEIGHT);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderRock.use();
		glUniformMatrix4fv(glGetUniformLocation(shaderRock.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderRock.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 model;
		model = glm::translate(model, objectPos);
		//model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		static GLfloat rotateCurrent = 0.0f;
		rotateCurrent += deltaTime * 10.0f;
		model = glm::rotate(model, glm::radians(rotateCurrent), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		//earth!!
		//model = glm::scale(model, glm::vec3(0.003f, 0.003f, 0.003f));
		glUniformMatrix4fv(glGetUniformLocation(shaderRock.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(glGetUniformLocation(shaderRock.program, "lightPos"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(shaderRock.program, "viewPos"), 1, &camera.Position[0]);
		//glUniform1i(glGetUniformLocation(shaderRock.program, "mirrorMap"), 3);
		//glActiveTexture(GL_TEXTURE3);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		//RenderCube();
		//glUniform1i(glGetUniformLocation(shaderRock.program, "texture_my"), 3);
		//glActiveTexture(GL_TEXTURE3);
		//glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glUniform1i(glGetUniformLocation(shaderRock.program, "texture_mirror"), 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mirrorCubemap);
		//rock.Draw(shaderRock);
		//RenderCube();
		earth.Draw(shaderRock);
		//sculpt.Draw(shaderRock);
		//rock2.Draw(shaderRock);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);

		/*
		glUniform1i(glGetUniformLocation(shaderRock.program, "texture_mirror"), 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mirrorCubemap);
		rock.Draw(shaderRock);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);
		*/

		glfwSwapBuffers(window);
	}

	// удаляем, завершаем
	glDeleteBuffers(1, &cubeVBO);
	glDeleteVertexArrays(1, &cubeVAO);

	glfwTerminate();

	return 0;
}