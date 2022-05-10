#pragma once 
#include <GL/glew.h> 
#include <GL/freeglut.h> 
#include <glm/glm.hpp> 
#include "ICallbacks.h" 
#include "Pipeline.h" 
#include "Texture.h" 
#include "LightingProgram.h" 
#include "Math_3d.h"
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))


struct Vertex {
    glm::vec3 m_pos;
    glm::vec2 m_tex;
    glm::vec3 m_normal;

    Vertex() {}

    Vertex(glm::vec3 pos, glm::vec2 tex)
    {
        m_pos = pos;
        m_tex = tex;
        m_normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }
};

class Main : public ICallbacks {
    int WINDOW_WIDTH = 1024;
    int WINDOW_HEIGHT = 768;

    GLuint gWorldLocation;
    GLuint VBO;
    GLuint IBO;
    Texture* pTexture = nullptr;
    LightingProgram* pLighting = nullptr;
    const char* texturePath = "D:\\test.jpg";

    DirectLight dirLight = { {1.0f, 1.0f, 1.0f}, 0.5f, 
                             {1.0f, 0.0f, 0.0f}, 0.75f };


    void CreateVertices() {
        unsigned int Indices[] = { 
             0, 3, 1,
             1, 3, 2,
             2, 3, 0,
             1, 2, 0 
        };

        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

        Vertex input[4] = {
          Vertex(glm::vec3 {-1.0f, -1.0f, 0.5773f}, glm::vec2 {0.0f, 0.0f}),
          Vertex(glm::vec3 {0.0f, -1.0f, -1.1547}, glm::vec2 {0.5f, 0.0f}),
          Vertex(glm::vec3 {1.0f, -1.0f, 0.5773f}, glm::vec2 {1.0f, 0.0f}),
          Vertex(glm::vec3 {0.0f, 1.0f, 0.0f}, glm::vec2 {0.5f, 1.0f}),
        };
        Vertex cube[8] = {
            Vertex(glm::vec3 {0.0f, 0.0f, 0.0f}, glm::vec2 {0.0f, 0.0f}),
            Vertex(glm::vec3 {1.0f, 0.0f, 0.0f}, glm::vec2 {0.0f, 0.0f}),
            Vertex(glm::vec3 {1.0f, 0.0f, 1.0f}, glm::vec2 {0.0f, 0.0f}),
            Vertex(glm::vec3 {0.0f, 0.0f, 1.0f}, glm::vec2 {0.0f, 0.0f}),
            Vertex(glm::vec3 {0.0f, 1.0f, 0.0f}, glm::vec2 {0.0f, 0.0f}),
            Vertex(glm::vec3 {1.0f, 1.0f, 0.0f}, glm::vec2 {0.0f, 0.0f}),
            Vertex(glm::vec3 {0.0f, 1.0f, 1.0f}, glm::vec2 {0.0f, 0.0f}),
            Vertex(glm::vec3 {1.0f, 1.0f, 1.0f}, glm::vec2 {0.0f, 0.0f}),
        };

        unsigned int VertexCount = ARRAY_SIZE_IN_ELEMENTS(input);

        CalcNormals(Indices, ARRAY_SIZE_IN_ELEMENTS(Indices), input, VertexCount);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(input), input, GL_STATIC_DRAW);
    }

public:
    ~Main() {
        delete pTexture;
        delete pLighting;
    }

    bool Init() {
        CreateVertices();

        pLighting = new LightingProgram();

        if (!pLighting->init()) return false;

        pLighting->use();

        pLighting->setTextureUnit(0);


        pTexture = new Texture(GL_TEXTURE_2D, texturePath);

        if (!pTexture->Load()) {
            return false;
        }

        return true;
    }

    void RenderSceneCB() override {
        glClear(GL_COLOR_BUFFER_BIT);

        static float v = 0.1f;
        v += 0.001f;

        Pipeline p;
        //p.Scale(0.1f, 0.1f, 0.1f); 
        p.WorldPos(0.0f, 0.0f, 1.0f);
        p.Rotate(0.0f, v, 0.0f); 

        const glm::mat4x4& WorldTransformation = p.GetTrans();
        pLighting->SGWP(WorldTransformation);
        pLighting->SWMP(WorldTransformation);

        pLighting->SDL(dirLight);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        pTexture->Bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        glutSwapBuffers();
    }

    void CalcNormals(const unsigned int* pIndices, unsigned int IndexCount, Vertex* pVertices, unsigned int VertexCount)
    {
        for (unsigned int i = 0; i < IndexCount; i += 3) {
            unsigned int Index0 = pIndices[i];
            unsigned int Index1 = pIndices[i + 1];
            unsigned int Index2 = pIndices[i + 2];
            glm::vec3 v1 = pVertices[Index1].m_pos - pVertices[Index0].m_pos;
            glm::vec3 v2 = pVertices[Index2].m_pos - pVertices[Index0].m_pos;
            glm::vec3 Normal = VectorsMath::Cross(v1, v2);
            VectorsMath::Normalize(Normal);

            pVertices[Index0].m_normal += Normal;
            pVertices[Index1].m_normal += Normal;
            pVertices[Index2].m_normal += Normal;
        }

        for (unsigned int i = 0; i < VertexCount; i++) {
            VectorsMath::Normalize(pVertices[i].m_normal);
        }
    }

    void IdleCB() override {
        RenderSceneCB();
    }
};