#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdio>
using namespace std;

class GLProgram {

protected:
	GLuint program;

	bool compileShader(GLuint ShaderObj) {
		glCompileShader(ShaderObj);

		GLint success;
		glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLchar InfoLog[1024];
			glGetShaderInfoLog(ShaderObj, sizeof(InfoLog), NULL, InfoLog);
			fprintf(stderr, "Error compiling shader: '%s'\n", InfoLog);
			return false;
		}

		return true;
	}


	bool linkProgram() {
		glLinkProgram(program);
		GLint Success;
		GLchar* ErrorLog = nullptr;
		glGetProgramiv(program, GL_LINK_STATUS, &Success);
		if (Success == 0) {
			glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
			return false;
		}

		glValidateProgram(program);
		glGetProgramiv(program, GL_VALIDATE_STATUS, &Success);
		if (!Success) {
			glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
			exit(1);
		}

		return true;
	}

	bool addShader(GLenum type, const GLchar* text) {
		GLuint ShaderObj = glCreateShader(type);
		const GLchar* p[1];
		p[0] = text;
		GLint Lengths[1];
		Lengths[0] = strlen(text);
		glShaderSource(ShaderObj, 1, p, Lengths);

		if (!compileShader(ShaderObj)) return false;

		glAttachShader(program, ShaderObj);

		return true;
	}

public:

	virtual bool init() {
		program = glCreateProgram();

		if (!program) {
			return false;
		}

		return true;
	}

	void use() {
		glUseProgram(program);
	}

	GLuint getUniformLocation(const char* name) {
		return glGetUniformLocation(program, (GLchar*)name);
	}
};
