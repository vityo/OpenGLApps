#include "Shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath) {
	program = glCreateProgram();

	attachShader(vertexPath, GL_VERTEX_SHADER);

	if (geometryPath != "") {
		attachShader(geometryPath, GL_GEOMETRY_SHADER);
	}

	attachShader(fragmentPath, GL_FRAGMENT_SHADER);

	glLinkProgram(program);
}

void Shader::attachShader(const GLchar* path, GLenum type) {
	std::string shaderCode;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::badbit);

	try {
		shaderFile.open(path);
		if (!shaderFile.good()) {
			std::cout << "ERROR file not found" << std::endl;
		}
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR file read" << std::endl;
	}

	const GLchar* shaderCodeGL = shaderCode.c_str();

	GLuint shader;
	GLint success;
	GLchar infoLog[512];

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderCodeGL, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR shader compile" << infoLog << std::endl;
	};

	glAttachShader(program, shader);
	glDeleteShader(shader);
}

void Shader::use() {
	glUseProgram(program);
}
