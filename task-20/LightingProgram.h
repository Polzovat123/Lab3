#pragma once
#include "GLProgram.h"
#include <glm/glm.hpp>
#include "Math_3d.h"
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define MAX_POINT_LIGHTS 3

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
    TexCoord0   = TexCoord;                                                         \n\
    Normal0     = (gWorld * vec4(Normal, 0.0)).xyz;                                 \n\
    WorldPos0   = (gWorld * vec4(Position, 1.0)).xyz;                               \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
const int MAX_POINT_LIGHTS = 2;                                                     \n\
const int MAX_SPOT_LIGHTS = 2;                                                      \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
in vec3 WorldPos0;                                                                  \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
struct BaseLight                                                                    \n\
{                                                                                   \n\
    vec3 Color;                                                                     \n\
    float AmbientIntensity;                                                         \n\
    float DiffuseIntensity;                                                         \n\
};                                                                                  \n\
                                                                                    \n\
struct DirectionalLight                                                             \n\
{                                                                                   \n\
    BaseLight Base;                                                          \n\
    vec3 Direction;                                                                 \n\
};                                                                                  \n\
                                                                                    \n\
struct Attenuation                                                                  \n\
{                                                                                   \n\
    float Constant;                                                                 \n\
    float Linear;                                                                   \n\
    float Exp;                                                                      \n\
};                                                                                  \n\
                                                                                    \n\
struct PointLight                                                                           \n\
{                                                                                           \n\
    BaseLight Base;                                                                  \n\
    vec3 Position;                                                                          \n\
    Attenuation Atten;                                                                      \n\
};                                                                                          \n\
                                                                                            \n\
struct SpotLight                                                                            \n\
{                                                                                           \n\
    PointLight Base;                                                                 \n\
    vec3 Direction;                                                                         \n\
    float Cutoff;                                                                           \n\
};                                                                                          \n\
                                                                                            \n\
uniform int gNumPointLights;                                                                \n\
uniform int gNumSpotLights;                                                                 \n\
uniform DirectionalLight gDirectionalLight;                                                 \n\
uniform PointLight gPointLights[MAX_POINT_LIGHTS];                                          \n\
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];                                             \n\
uniform sampler2D gSampler;                                                                 \n\
uniform vec3 gEyeWorldPos;                                                                  \n\
uniform float gMatSpecularIntensity;                                                        \n\
uniform float gSpecularPower;                                                               \n\
                                                                                            \n\
vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal)            \n\
{                                                                                           \n\
    vec4 AmbientColor = vec4(Light.Color, 1.0f) * Light.AmbientIntensity;                   \n\
    float DiffuseFactor = dot(Normal, -LightDirection);                                     \n\
                                                                                            \n\
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  \n\
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  \n\
                                                                                            \n\
    if (DiffuseFactor > 0) {                                                                \n\
        DiffuseColor = vec4(Light.Color, 1.0f) * Light.DiffuseIntensity * DiffuseFactor;    \n\
                                                                                            \n\
        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);                             \n\
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     \n\
        float SpecularFactor = dot(VertexToEye, LightReflect);                              \n\
        SpecularFactor = pow(SpecularFactor, gSpecularPower);                               \n\
        if (SpecularFactor > 0) {                                                           \n\
            SpecularColor = vec4(Light.Color, 1.0f) *                                       \n\
                            gMatSpecularIntensity * SpecularFactor;                         \n\
        }                                                                                   \n\
    }                                                                                       \n\
                                                                                            \n\
    return (AmbientColor + DiffuseColor + SpecularColor);                                   \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcDirectionalLight(vec3 Normal)                                                      \n\
{                                                                                           \n\
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal);  \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcPointLight( PointLight l, vec3 Normal)                                       \n\
{                                                                                           \n\
    vec3 LightDirection = WorldPos0 - l.Position;                                           \n\
    float Distance = length(LightDirection);                                                \n\
    LightDirection = normalize(LightDirection);                                             \n\
                                                                                            \n\
    vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal);                         \n\
    float Attenuation =  l.Atten.Constant +                                                 \n\
                         l.Atten.Linear * Distance +                                        \n\
                         l.Atten.Exp * Distance * Distance;                                 \n\
                                                                                            \n\
    return Color / Attenuation;                                                             \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcSpotLight( SpotLight l, vec3 Normal)                                         \n\
{                                                                                           \n\
    vec3 LightToPixel = normalize(WorldPos0 - l.Base.Position);                             \n\
    float SpotFactor = dot(LightToPixel, l.Direction);                                      \n\
                                                                                            \n\
    if (SpotFactor > l.Cutoff) {                                                            \n\
        vec4 Color = CalcPointLight(l.Base, Normal);                                        \n\
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));                   \n\
    }                                                                                       \n\
    else {                                                                                  \n\
        return vec4(0,0,0,0);                                                               \n\
    }                                                                                       \n\
}                                                                                           \n\
                                                                                            \n\
void main()                                                                                 \n\
{                                                                                           \n\
    vec3 Normal = normalize(Normal0);                                                       \n\
    vec4 TotalLight = CalcDirectionalLight(Normal);                                         \n\
                                                                                            \n\
    for (int i = 0 ; i < gNumPointLights ; i++) {                                           \n\
        TotalLight += CalcPointLight(gPointLights[i], Normal);                              \n\
    }                                                                                       \n\
                                                                                            \n\
    for (int i = 0 ; i < gNumSpotLights ; i++) {                                            \n\
        TotalLight += CalcSpotLight(gSpotLights[i], Normal);                                \n\
    }                                                                                       \n\
                                                                                            \n\
    FragColor = texture2D(gSampler, TexCoord0.xy) * TotalLight;                             \n\
}";

struct BaseLight {
    glm::vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;

    BaseLight() {
        Color = glm::vec3{ 0.0f, 0.0f, 0.0f };
        AmbientIntensity = 0.0f;
        DiffuseIntensity = 0.0f;
    }
};

struct PointLight : public BaseLight {
    glm::vec3 Position;

    struct {
        float Constant;
        float Linear;
        float Exp;
    } Attenuation; // this constsnt uses in calculation distanxe
    // L1 / (const + linear * dist + exp * dist * dist)

    PointLight() {
        Position = glm::vec3{ 0.0f, 0.0f, 0.0f };
        Attenuation.Constant = 1.0f;
        Attenuation.Linear = 0.0f;
        Attenuation.Exp = 0.0f;
    }
};

struct DirectLight : public BaseLight {
    glm::vec3 Direction;

    DirectLight() {
        Direction = glm::vec3{ 0.0f, 0.0f, 0.0f };
    }
};

struct SpotLight : public PointLight{
    glm::vec3 Direction;
    float Cutoff;

    SpotLight()
    {
        Direction = glm::vec3{ 0.0f, 0.0f, 0.0f };
        Cutoff = 0.0f;
    }
};

class LightingProgram : public GLProgram {
    static const unsigned int MAX_SPOT_LIGHTS = 2;
	GLuint gWorldLocation, WorldMatrixLocation, gSampler;
    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint Direction;
        GLuint DiffuseIntensity;
    } dirLightLocation;
    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        struct {
            GLuint c;
            GLuint linear;
            GLuint exp;
        } Atten;
    }pointLightLoc[MAX_POINT_LIGHTS];
    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        GLuint Direction;
        GLuint Cutoff;
        struct {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } m_spotLightsLocation[MAX_SPOT_LIGHTS];
    GLuint eyeWorlPosition, intensityLocation, powerLocation, numSpotLightsLocation;
    GLuint numPointLightsLocation;
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

        // 20 lesson
        numPointLightsLocation = getUniformLocation("gNumPointLights");

        // 21 lesson
        numSpotLightsLocation = getUniformLocation("gNumSpotLights");
        for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(pointLightLoc); i++) {
            char name[128];
            memset(name, 0, sizeof(name));
            snprintf(name, sizeof(name), "gPointLights[%d].Base.Color", i);
            pointLightLoc[i].Color = getUniformLocation(name);

            //snprintf(Name, sizeof(Name), "gPointLights[%d].Base.AmbientIntensity", i);
            pointLightLoc[i].AmbientIntensity = getUniformLocation(name);

            //snprintf(Name, sizeof(Name), "gPointLights[%d].Position", i);
            pointLightLoc[i].Position = getUniformLocation(name);

            //snprintf(Name, sizeof(Name), "gPointLights[%d].Base.DiffuseIntensity", i);
            pointLightLoc[i].DiffuseIntensity = getUniformLocation(name);

            //snprintf(Name, sizeof(Name), "gPointLights[%d].Atten.Constant", i);
            pointLightLoc[i].Atten.c = getUniformLocation(name);

            //snprintf(Name, sizeof(Name), "gPointLights[%d].Atten.Linear", i);
            pointLightLoc[i].Atten.linear = getUniformLocation(name);

            //snprintf(Name, sizeof(Name), "gPointLights[%d].Atten.Exp", i);
            pointLightLoc[i].Atten.exp = getUniformLocation(name);
        }
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

    // lesson 19
    void SIL(float intensity) {
        glUniform1f(intensityLocation, intensity);
    }

    void SPL(float power) {
        glUniform1f(powerLocation, power);
    }

    void SEWP(const glm::vec3& eyeWorlPos) {
        glUniform3f(eyeWorlPosition, eyeWorlPos.x, eyeWorlPos.y, eyeWorlPos.z);
    }

    // lesson 20
    void SPL(unsigned int numLights, const PointLight* pLights) {
        glUniform1i(numPointLightsLocation, numLights);

        for (unsigned int i = 0; i < numLights; i++) {
            glUniform3f(pointLightLoc[i].Color, pLights[i].Color.x, pLights[i].Color.y, pLights[i].Color.z);
            glUniform1f(pointLightLoc[i].AmbientIntensity, pLights[i].AmbientIntensity);
            glUniform1f(pointLightLoc[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
            glUniform3f(pointLightLoc[i].Position, pLights[i].Position.x, pLights[i].Position.y, pLights[i].Position.z);
            glUniform1f(pointLightLoc[i].Atten.c, pLights[i].Attenuation.Constant);
            glUniform1f(pointLightLoc[i].Atten.linear, pLights[i].Attenuation.Linear);
            glUniform1f(pointLightLoc[i].Atten.exp, pLights[i].Attenuation.Exp);
        }
    }

    void SSL(unsigned int NumLights, const SpotLight* pLights) {
        glUniform1i(numSpotLightsLocation, NumLights);

        for (unsigned int i = 0; i < NumLights; i++) {
            glUniform3f(m_spotLightsLocation[i].Color, pLights[i].Color.x, pLights[i].Color.y, pLights[i].Color.z);
            glUniform1f(m_spotLightsLocation[i].AmbientIntensity, pLights[i].AmbientIntensity);
            glUniform1f(m_spotLightsLocation[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
            glUniform3f(m_spotLightsLocation[i].Position, pLights[i].Position.x, pLights[i].Position.y, pLights[i].Position.z);
            glm::vec3 Direction = pLights[i].Direction;
            VectorsMath::Normalize(Direction);
            glUniform3f(m_spotLightsLocation[i].Direction, Direction.x, Direction.y, Direction.z);
            glUniform1f(m_spotLightsLocation[i].Cutoff, cosf(ToRadian(pLights[i].Cutoff)));
            glUniform1f(m_spotLightsLocation[i].Atten.Constant, pLights[i].Attenuation.Constant);
            glUniform1f(m_spotLightsLocation[i].Atten.Linear, pLights[i].Attenuation.Linear);
            glUniform1f(m_spotLightsLocation[i].Atten.Exp, pLights[i].Attenuation.Exp);
        }
    }
};