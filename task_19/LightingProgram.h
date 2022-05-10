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
out vec3 WorldPos0;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    TexCoord0 = TexCoord;                                                           \n\
    Normal0   = (gWorld * vec4(Normal, 0.0)).xyz;                                   \n\
    WorldPos0 = (gWorld * vec4(Position, 1.0)).xyz;                                 \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
in vec3 WorldPos0;                                                                  \n\
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
uniform vec3 gEyeWorldPos;                                                          \n\
uniform float gMatSpecularIntensity;                                                \n\
uniform float gSpecularPower;                                                       \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    vec4 AmbientColor = vec4(gDirectionalLight.Color, 1.0f) *                       \n\
                        gDirectionalLight.AmbientIntensity;                         \n\
                                                                                    \n\
    vec3 LightDirection = -gDirectionalLight.Direction;                             \n\
    vec3 Normal = normalize(Normal0);                                               \n\
                                                                                    \n\
    float DiffuseFactor = dot(Normal, LightDirection);                              \n\
                                                                                    \n\
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                          \n\
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                          \n\
                                                                                    \n\
    if (DiffuseFactor > 0){                                                         \n\
        DiffuseColor = vec4(gDirectionalLight.Color, 1.0f) *                        \n\
                       gDirectionalLight.DiffuseIntensity *                         \n\
                       DiffuseFactor;                                               \n\
                                                                                    \n\
        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);                     \n\
        vec3 LightReflect = normalize(reflect(gDirectionalLight.Direction, Normal));\n\
        float SpecularFactor = dot(VertexToEye, LightReflect);                      \n\
        SpecularFactor = pow(SpecularFactor, gSpecularPower);                       \n\
                                                                                    \n\
        if (SpecularFactor > 0){                                                    \n\
            SpecularColor = vec4(gDirectionalLight.Color, 1.0f) *                   \n\
                            gMatSpecularIntensity *                                 \n\
                            SpecularFactor;                                         \n\
        }                                                                           \n\
    }                                                                               \n\
                                                                                    \n\
    FragColor = texture2D(gSampler, TexCoord0.xy) *                                 \n\
                (AmbientColor + DiffuseColor + SpecularColor);                      \n\
}";

struct DirectLight
{
	glm::vec3 Color;
	float AmbientIntensity;
    glm::vec3 Direction;
    float DiffuseIntensity;
};

class LightingProgram : public GLProgram {
	GLuint gWorldLocation, WorldMatrixLocation, gSampler;
    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint Direction;
        GLuint DiffuseIntensity;
    } dirLightLocation;
    GLuint eyeWorlPosition, intensityLocation, powerLocation;

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
        // 19 lesson
        eyeWorlPosition = getUniformLocation("gEyeWorldPos");
        intensityLocation = getUniformLocation("gMatSpecularIntensity");
        powerLocation = getUniformLocation("gSpecularPower");

		return true;
	}

	void SGWP(const glm::mat4x4& value) {
		glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)&value[0][0]);
	}

    void SWMP(const glm::mat4x4& worldInverse) {
        glUniformMatrix4fv(WorldMatrixLocation, 1, GL_TRUE, (const GLfloat*)&worldInverse[0][0]);
    }

	void setTextureUnit(int textureUnit) {
		glUniform1i(gSampler, textureUnit);
	}
    
    void SDL(const DirectLight& light){
        glUniform3f(dirLightLocation.Color, light.Color.x, light.Color.y, light.Color.z);
        glUniform1f(dirLightLocation.AmbientIntensity, light.AmbientIntensity);
        glm::vec3 Direction = light.Direction;
        VectorsMath::Normalize(Direction);
        glUniform3f(dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
        glUniform1f(dirLightLocation.DiffuseIntensity, light.DiffuseIntensity);
    }

    void SIL(float intensity) {
        glUniform1f(intensityLocation, intensity);
    }

    void SPL(float power) {
        glUniform1f(powerLocation, power);
    }

    void SEWP(const glm::vec3& eyeWorlPos) {
        glUniform3f(eyeWorlPosition, eyeWorlPos.x, eyeWorlPos.y, eyeWorlPos.z);
    }
};