#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdio>
#include "Main.h"

namespace GLUTBackend {
	ICallbacks* callbacks = nullptr;


	void RenderSceneCB() {
		callbacks->RenderSceneCB();
	}

	void IdleCB() {
		callbacks->IdleCB();
	}

	void InitCallbacks() {
		glutDisplayFunc(RenderSceneCB);
		glutIdleFunc(IdleCB);
	}

	void Init(int argc, char** argv) {
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	}

	void CreateWin(int width, int height, int x, int y, const char* title) {
		glutInitWindowSize(width, height);
		glutInitWindowPosition(x, y);
		glutCreateWindow(title);

		GLenum res = glewInit();
		if (res != GLEW_OK)
		{
			fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
			throw "error";
		}
	}



	void Run(ICallbacks* pCallbacks = nullptr) {
		if (pCallbacks == nullptr) {
			printf("[GLUTBackend]: Callbacks aren't specified!");
		}


		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		callbacks = pCallbacks;

		InitCallbacks();
		glutMainLoop();
	}
};
