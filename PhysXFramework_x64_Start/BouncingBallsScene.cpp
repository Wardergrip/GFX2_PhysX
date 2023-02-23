#include "stdafx.h"
#include "BouncingBallsScene.h"

#include "PhysxManager.h"
#include "SpherePosColorNorm.h"

void BouncingBallsScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	const auto pPhysX = PhysxManager::GetInstance()->GetPhysics();
	const PxMaterial* pDefaultMaterial = pPhysX->createMaterial(.5f, .5f, .2f);


	// MATS
	const PxMaterial* pBouncingMaterial = pPhysX->createMaterial(.5f, .5f, 1);
	const PxMaterial* pHalfBouncingMaterial = pPhysX->createMaterial(.5f, .5f, .5f);
	const PxMaterial* pNonBouncingMaterial = pPhysX->createMaterial(.5f, .5f, 0);

	// BALLS
	const float radius{ 1.f };
	const int slices{ 10 };
	const int stacks{ 10 };
	const DirectX::XMFLOAT4 color{ DirectX::Colors::Green };
	for (size_t i{ 0 }; i < 3; ++i)
	{
		const PxMaterial* pMat{};
		switch (i)
		{
		case 0:
			pMat = pBouncingMaterial;
			break;
		case 1:
			pMat = pHalfBouncingMaterial;
			break;
		case 2:
			pMat = pNonBouncingMaterial;
			break;
		default:
			pMat = pDefaultMaterial;
			break;
		}

		m_pBalls.push_back(new SpherePosColorNorm(radius, slices, stacks, color));
		m_pBalls.back()->Translate((i * radius * 2) + i * radius - radius * 3, m_DropHeight, 0.f);
		AddGameObject(m_pBalls.back());

		// SPHEREACTOR
		PxRigidDynamic* pSphereActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
		PxSphereGeometry sphereGeometry{ radius };

		PxRigidActorExt::createExclusiveShape(*pSphereActor, sphereGeometry, *pMat);

		//Link SPHERE with SPHERE_ACTOR
		m_pBalls.back()->AttachRigidActor(pSphereActor);
	}

	//GROUNDPLANE
	auto pGroundActor = pPhysX->createRigidStatic(PxTransform{ PxQuat{PxPiDivTwo,PxVec3{0,0,1}} });
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, *pDefaultMaterial);
	m_pPhysxScene->addActor(*pGroundActor);
}

void BouncingBallsScene::Update()
{
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, 'R'))
	{
		for (auto ball : m_pBalls)
		{
			auto pos = ball->GetPosition();
			ball->Translate(pos.x, m_DropHeight, pos.z);
		}
	}
}

void BouncingBallsScene::Draw() const
{
}

void BouncingBallsScene::OnSceneActivated()
{
}

void BouncingBallsScene::OnSceneDeactivated()
{
}
