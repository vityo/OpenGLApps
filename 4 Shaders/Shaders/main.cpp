#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

const GLuint WIDTH = 800, HEIGHT = 600;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	// вершинный шейдер
	const GLchar* vertexShaderSource = "#version 330 core\n"
		"layout(location = 0) in vec3 position;\n"
		"out vec4 vertexColor;\n"
		"void main() {\n"
		"gl_Position = vec4(position, 1.0);\n"
		"vertexColor = vec4(0.0f, 0.0f, 0.5f, 1.0f); }";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR vertexShader compile\n" << infoLog << std::endl;
	}

	// фрагментный шейдер
	const GLchar* fragmentShaderSource = "#version 330 core\n"
		"in vec4 vertexColor;\n"
		"out vec4 color;\n"
		"void main() {\n"
		"color = vertexColor; }";

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR fragmentShader compile\n" << infoLog << std::endl;
	}

	// шейдерная программа
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR shaderProgram link\n" << infoLog << std::endl;
	}

	// вершины, объекты VAO VBO
	GLfloat vertices[] = {
		-0.75f, 0.0f, 0.0f,
		-0.25f, 0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};
	GLuint VBO;
	glGenBuffers(1, &VBO);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	// фрагментный шейдер с переменным цветом
	const GLchar* fragmentShaderVarySource = "#version 330 core\n"
		"out vec4 color;\n"
		"uniform vec4 ourColor;\n"
		"void main() {\n"
		"color = ourColor; }";

	GLuint fragmentShaderVary = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderVary, 1, &fragmentShaderVarySource, NULL);
	glCompileShader(fragmentShaderVary);

	glGetShaderiv(fragmentShaderVary, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShaderVary, 512, NULL, infoLog);
		std::cout << "ERROR fragmentShaderVary compile\n" << infoLog << std::endl;
	}

	// шейдерная программа c переменным цветом
	GLuint shaderProgramVary = glCreateProgram();
	glAttachShader(shaderProgramVary, vertexShader);
	glAttachShader(shaderProgramVary, fragmentShaderVary);
	glLinkProgram(shaderProgramVary);

	glGetProgramiv(shaderProgramVary, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(shaderProgramVary, 512, NULL, infoLog);
		std::cout << "ERROR shaderProgramVary link\n" << infoLog << std::endl;
	}

	// удаляем ненужное
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(fragmentShaderVary);

	//wireframe glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // off glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)

	// вершины, объекты VAO VBO для треугольника c переменным цветом
	GLfloat verticesVary[] = {
		0.25f, 0.0f, 0.0f,
		0.75f, 0.0f, 0.0f,
		0.5f,  0.5f, 0.0f
	};
	GLuint VBOVary;
	glGenBuffers(1, &VBOVary);

	GLuint VAOVary;
	glGenVertexArrays(1, &VAOVary);

	glBindVertexArray(VAOVary);

	glBindBuffer(GL_ARRAY_BUFFER, VBOVary);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesVary), verticesVary, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	Shader shaderColored("../ShadersScripts/vertexShaderColored.vsh", "../ShadersScripts/fragmentShader.fsh");
	Shader shaderShift("../ShadersScripts/vertexShaderShift.vsh", "../ShadersScripts/fragmentShader.fsh");

	// вершины и цвета, объекты VAO VBO для треугольника c разными по цвету вершинами
	GLfloat verticesColored[] = {
		// Positions         // Colors
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
	};

	GLuint VBOColored;
	glGenBuffers(1, &VBOColored);

	GLuint VAOColored;
	glGenVertexArrays(1, &VAOColored);

	glBindVertexArray(VAOColored);

	glBindBuffer(GL_ARRAY_BUFFER, VBOColored);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesColored), verticesColored, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.298f, 0.451f, 0.773f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		GLfloat timeValue = static_cast<GLfloat>(glfwGetTime());
		GLfloat blueValue = static_cast<GLfloat>(sin(timeValue) * 0.5f + 0.5f);
		GLint vertexColorLocation = glGetUniformLocation(shaderProgramVary, "ourColor");
		glUseProgram(shaderProgramVary);
		glUniform4f(vertexColorLocation, 0.0f, 0.0f, blueValue, 1.0f);
		glBindVertexArray(VAOVary);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		shaderColored.use();
		glBindVertexArray(VAOColored);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		GLfloat shiftX = static_cast<GLfloat>(cos(timeValue) * 0.5f);
		GLint shiftLocation = glGetUniformLocation(shaderShift.program, "shift");
		shaderShift.use();
		glUniform2f(shiftLocation, shiftX, 0.0f);
		glBindVertexArray(VAOColored);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}