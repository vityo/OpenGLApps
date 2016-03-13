#include "Shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath) {
	std::string vertexCode;
	std::string geometryCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream gShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);

	try {
		vShaderFile.open(vertexPath);
		std::stringstream vShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		vShaderFile.close();
		vertexCode = vShaderStream.str();

		gShaderFile.open(geometryPath);
		std::stringstream gShaderStream;
		gShaderStream << gShaderFile.rdbuf();
		gShaderFile.close();
		geometryCode = gShaderStream.str();

		fShaderFile.open(fragmentPath);
		std::stringstream fShaderStream;
		fShaderStream << fShaderFile.rdbuf();
		fShaderFile.close();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR file read" << std::endl;
	}

	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* gShaderCode = geometryCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	GLuint vertex, geometry, fragment;
	GLint success;
	GLchar infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR vertex shader compile" << infoLog << std::endl;
	};

	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &gShaderCode, NULL);
	glCompileShader(geometry);

	glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(geometry, 512, NULL, infoLog);
		std::cout << "ERROR geometry shader compile" << infoLog << std::endl;
	};

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR fragment shader compile" << infoLog << std::endl;
	};

	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, geometry);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR shader program link" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() {
	glUseProgram(program);
}
