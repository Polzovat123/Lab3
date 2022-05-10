#pragma once
#include "GLProgram.h"
#include <glm/glm.hpp>
#include "Math_3d.h"

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
layout (location = 2) in vec3 Normal;                                               \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
out vec2 TexCoord0;                                                                 \n\
out vec3 Normal0;                                                                   \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    TexCoord0 = TexCoord;                                                           \n\
    Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;                                     \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
struct DirectionalLight                                                             \n\
{                                                                                   \n\
    vec3 Color;                                                                     \n\
    float AmbientIntensity;                                                         \n\
    vec3 Direction;                                                                 \n\
    float DiffuseIntensity;                                                         \n\
};                                                                                  \n\
                                                                                    \n\
uniform DirectionalLight gDirectionalLight;                                         \n\
uniform sampler2D gSampler;                                                         \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    vec4 AmbientColor = vec4(gDirectionalLight.Color, 1.0f) *                       \n\
                        gDirectionalLight.AmbientIntensity;                         \n\
                                                                                    \n\
    float DiffuseFactor = dot(normalize(Normal0), -gDirectionalLight.Direction);    \n\
                                                                                    \n\
    vec4 DiffuseColor;                                                              \n\
                                                                                    \n\
    if (DiffuseFactor > 0){                                                         \n\
        DiffuseColor = vec4(gDirectionalLight.Color, 1.0f) *                        \n\
                       gDirectionalLight.DiffuseIntensity *                         \n\
                       DiffuseFactor;                                               \n\
    }                                                                               \n\
    else{                                                                           \n\
        DiffuseColor = vec4(0,0,0,0);                                               \n\
    }                                                                               \n\
                                                                                    \n\
    FragColor = texture2D(gSampler, TexCoord0.xy) *                                 \n\
                (AmbientColor + DiffuseColor);                                      \n\
}";

struct DirectLight
{
	glm::vec3 Color;
	float AmbientIntensity;
    glm::vec3 Direction;
    float DiffuseIntensity;
};

class LightingProgram : public GLProgram {
	GLuint gWorldLocation, gSampler, dirLightColor, dirLightAmbientIntensity, WVPLocation;
    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint Direction;
        GLuint DiffuseIntensity;
    } dirLightLocation;
    GLuint WorldMatrixLocation;
public:
	bool init() override {
		if (!GLProgram::init()) return false;

		if (!addShader(GL_VERTEX_SHADER, pVS)) return false;
		if (!addShader(GL_FRAGMENT_SHADER, pFS)) return false;

		if (!linkProgram()) return false;

        gWorldLocation= getUniformLocation("gWVP");
        WorldMatrixLocation = getUniformLocation("gWorld");
        gSampler= getUniformLocation("gSampler");
        dirLightLocation.Color = getUniformLocation("gDirectionalLight.Color");
        dirLightLocation.AmbientIntensity = getUniformLocation("gDirectionalLight.AmbientIntensity");
        dirLightLocation.Direction = getUniformLocation("gDirectionalLight.Direction");
        dirLightLocation.DiffuseIntensity = getUniformLocation("gDirectionalLight.DiffuseIntensity");

		return true;
	}

	void SGWP(const glm::mat4x4& value) {
		glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)&value[0][0]);
	}

    void SWMP(const glm::mat4x4& WorldInverse) {
        glUniformMatrix4fv(WorldMatrixLocation, 1, GL_TRUE, (const GLfloat*)&WorldInverse[0][0]);
    }

	void setTextureUnit(int textureUnit) {
		glUniform1i(gSampler, textureUnit);
	}
    
    void SDL(const DirectLight& Light){
        glUniform3f(dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
        glUniform1f(dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
        glm::vec3 Direction = Light.Direction;
        VectorsMath::Normalize(Direction);
        glUniform3f(dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
        glUniform1f(dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
    }
};