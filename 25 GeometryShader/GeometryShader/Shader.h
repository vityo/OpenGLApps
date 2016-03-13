#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader {
public:
	GLuint program;

	Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath);
	void use();
};