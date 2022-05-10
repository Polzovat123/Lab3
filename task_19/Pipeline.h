#pragma once 
#include <glm/glm.hpp> 
#include "Math_3d.h" 

#define M_PI 3.14159265358979323846 
#define ToRadian(x) ((x) * M_PI / 180.0f) 
#define ToDegree(x) ((x) * 180.0f / M_PI) 



class Pipeline {
	struct PerspectiveProjInfo {
		float FOV;
		float Width;
		float Height;
		float zNear;
		float zFar;
	};

	struct {
		glm::vec3 Pos;
		glm::vec3 Target;
		glm::vec3 Up;
	} m_camera;

	const int x = 0, y = 1, z = 2;

	glm::vec3
		vScale{ 1.0f, 1.0f, 1.0f },
		vRotate{ 0.0f, 0.0f, 0.0f },
		vTranslation{ 0.0f, 0.0f, 0.0f };

	glm::mat4 mTransformation;

	PerspectiveProjInfo perspectiveProjInfo{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	glm::mat4 InitScaleTransform(float x, float y, float z) {
		return glm::mat4{
		 {x, 0.0f, 0.0f, 0.0f},
		 {0.0f, y, 0.0f, 0.0f},
		 {0.0f, 0.0f, z, 0.0f},
		 {0.0f, 0.0f, 0.0f, 1.0f}
		};
	}

	glm::mat4 InitRotateTransform(float _x, float _y, float _z) {
		glm::mat4 xm, ym, zm;

		float x = ToRadian(_x);
		float y = ToRadian(_y);
		float z = ToRadian(_z);

		xm = {
		 {1.0f, 0.0f, 0.0f, 0.0f},
		 {0.0f, cosf(x), -sinf(x), 0.0f},
		 {0.0f, sinf(x), cosf(x), 0.0f},
		 {0.0f, 0.0f, 0.0f, 1.0f},
		};

		ym = {
		 {cosf(y), 0.0f, -sinf(y), 0.0f},
		 {0.0f, 1.0f, 0.0f, 0.0f},
		 {sinf(y), 0.0f, cosf(y), 0.0f},
		 {0.0f, 0.0f, 0.0f, 1.0f},
		};

		zm = {
		 {cosf(z), -sinf(z), 0.0f, 0.0f},
		 {sinf(z), cosf(z), 0.0f, 0.0f},
		 {0.0f, 0.0f, 1.0f, 0.0f},
		 {0.0f, 0.0f, 0.0f, 1.0f},
		};

		return xm * ym * zm;
	}

	glm::mat4 InitTranslationTransform(float x, float y, float z) {
		return glm::mat4{
		 {1.0f, 0.0f, 0.0f, x},
		 {0.0f, 1.0f, 0.0f, y},
		 {0.0f, 0.0f, 1.0f, z},
		 {0.0f, 0.0f, 0.0f, 1.0f},
		};
	}

	glm::mat4 InitPerspectiveProj(float FOV, float Width, float Height, float zNear, float zFar) {
		const float ar = Width / Height;
		const float zRange = zNear - zFar;
		const float tanHalfFOV = tanf(ToRadian(FOV / 2.0));

		return glm::mat4{
		 {1.0f / (ar * tanHalfFOV), 0.0f, 0.0f, 0.0f},
		 {0.0f, 1.0f / tanHalfFOV, 0.0f, 0.0f},
		 {0.0f, 0.0f, (-zNear - zFar) / zRange, (2.0f * zFar * zNear) / zRange},
		 {0.0f, 0.0f, 1.0f, 0.0f}
		};
	}

	glm::mat4 InitCameraTransform(glm::vec3 Target, glm::vec3 Up) {
		glm::vec3 N = glm::vec3(Target);
		VectorsMath::Normalize(N);
		glm::vec3 _U = glm::vec3(Up);
		VectorsMath::Normalize(_U);
		glm::vec3 U = VectorsMath::Cross(_U, Target);
		glm::vec3 V = VectorsMath::Cross(N, U);

		return glm::mat4{
		 {U[x], U[y], U[z], 0.0f},
		 {V[x], V[y], V[z], 0.0f},
		 {N[x], N[y], N[z], 0.0f},
		 {0.0f, 0.0f, 0.0f, 1.0f},
		};
	}

public:
	void Scale(float x, float y, float z) {
		vScale = { x, y, z };
	}

	void WorldPos(float x, float y, float z) {
		vTranslation = { x, y, z };
	}

	void Rotate(float x, float y, float z) {
		vRotate = { x, y, z };
	}

	void SetPerspectiveProj(float FOV, float Width, float Height, float zNear, float zFar) {
		perspectiveProjInfo = { FOV, Width, Height, zNear, zFar };
	}

	void SetCamera(glm::vec3 pos, glm::vec3 target, glm::vec3 up) {
		m_camera = { pos, target, up };
	}

	glm::mat4 GetTrans() {
		glm::mat4 ScaleTrans, RotateTrans, TranslationTrans, CameraTranslationTrans, CameraRotateTrans, PersProjTrans;

		ScaleTrans = InitScaleTransform(vScale[x], vScale[y], vScale[z]);
		RotateTrans = InitRotateTransform(vRotate[x], vRotate[y], vRotate[z]);
		TranslationTrans = InitTranslationTransform(vTranslation[x], vTranslation[y], vTranslation[z]);
		CameraTranslationTrans = InitTranslationTransform(-m_camera.Pos[x], -m_camera.Pos[y], -m_camera.Pos[z]);
		CameraRotateTrans = InitCameraTransform(m_camera.Target, m_camera.Up);
		PersProjTrans = InitPerspectiveProj(perspectiveProjInfo.FOV, perspectiveProjInfo.Width, perspectiveProjInfo.Height, perspectiveProjInfo.zNear, perspectiveProjInfo.zFar);

		//return PersProjTrans * CameraRotateTrans * CameraTranslationTrans * TranslationTrans * RotateTrans * ScaleTrans; 
		//return PersProjTrans * CameraRotateTrans * CameraTranslationTrans * TranslationTrans *
		//RotateTrans* ScaleTrans;
		return ScaleTrans * RotateTrans * TranslationTrans;
	}
};