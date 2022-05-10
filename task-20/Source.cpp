#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <cstdio>
#include "Pipeline.h"
#include "LightingProgram.h"
#include "Texture.h"
#include "GLUTBackend.h"
#include "Main.h"
using namespace std;

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

int main(int argc, char** argv) {
	GLUTBackend::Init(argc, argv);
	GLUTBackend::CreateWin(WINDOW_WIDTH, WINDOW_HEIGHT, 50, 50, "AVE MARIA");

	Magick::InitializeMagick(nullptr);

	Main *app = new Main();
	
	if (!app->Init()) return 1;

	GLUTBackend::Run(app);

	delete app;
}