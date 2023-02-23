#include "stdafx.h"
#include "FrictionTestScene.h"

#include "PhysxManager.h"
#include "CubePosColorNorm.h"

void FrictionTestScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	const auto pPhysX = PhysxManager::GetInstance()->GetPhysics();
	const PxMaterial* pDefaultMaterial = pPhysX->createMaterial(.5f, .5f, .2f);

	// MATS
	const PxMaterial* pRampMaterial = pPhysX->createMaterial(0, 0, 0);
	const PxMaterial* pFrictionlessMaterial = pPhysX->createMaterial(0, 0, 0);
	const PxMaterial* pHalfFrictionMaterial = pPhysX->createMaterial(0.5f, 0.5f, 0);
	const PxMaterial* pFullFrictionMaterial = pPhysX->createMaterial(1.f, 1.f, 0);

	//GROUNDPLANE
	auto pGroundActor = pPhysX->createRigidStatic(PxTransform{ PxQuat{PxPiDivTwo,PxVec3{0,0,1}} });
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, *pFrictionlessMaterial);
	m_pPhysxScene->addActor(*pGroundActor);

	// CUBES
	const XMFLOAT3 actorDimensions{ 1.f,1.f,1.f };
	for (size_t i{ 0 }; i < 3; ++i)
	{
		const PxMaterial* pMat{};
		switch (i)
		{
		case 0:
			pMat = pFrictionlessMaterial;
			break;
		case 1:
			pMat = pHalfFrictionMaterial;
			break;
		case 2:
			pMat = pFullFrictionMaterial;
			break;
		default:
			pMat = pDefaultMaterial;
			break;
		}

		m_pCubes.push_back(new CubePosColorNorm(actorDimensions.x, actorDimensions.y, actorDimensions.z));
		AddGameObject(m_pCubes.back());
		// CUBEACTOR
		PxRigidDynamic* pCubeActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
		PxBoxGeometry boxGeometry{ actorDimensions.x * 0.5f, actorDimensions.y * 0.5f, actorDimensions.y * 0.5f };

		PxRigidActorExt::createExclusiveShape(*pCubeActor, boxGeometry, *pMat);

		//Link CUBE with CUBE_ACTOR
		m_pCubes.back()->AttachRigidActor(pCubeActor);
	}

	// RAMPS
	const XMFLOAT3 rampDimensions{ 5.f,1.f,1.f };
	for (size_t i{ 0 }; i < 3; ++i)
	{
		m_pRamps.push_back(new CubePosColorNorm(rampDimensions.x, rampDimensions.y, rampDimensions.z));
		AddGameObject(m_pRamps.back());
		// RAMPACTOR
		PxRigidStatic* pRampActor = pPhysX->createRigidStatic(PxTransform{ PxIdentity });
		PxBoxGeometry boxGeometry{ rampDimensions.x * 0.5f, rampDimensions.y * 0.5f, rampDimensions.y * 0.5f };

		PxRigidActorExt::createExclusiveShape(*pRampActor, boxGeometry, *pRampMaterial);

		//Link RAMP with RAMP_ACTOR
		m_pRamps.back()->AttachRigidActor(pRampActor);
	}

	// POSITIONS
	for (size_t i{ 0 }; i < 3; ++i)
	{
		m_pRamps[i]->RotateDegrees(0, 0, -30);
		m_pRamps[i]->Translate(6 * i, 3, 0);
		m_pCubes[i]->RotateDegrees(0, 0, -30);
		m_pCubes[i]->Translate(6 * i, 5, 0);
	}
}

void FrictionTestScene::Update()
{
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, 'R'))
	{
		for (size_t i{ 0 }; i < 3; ++i)
		{
			m_pCubes[i]->RotateDegrees(0, 0, -30);
			m_pCubes[i]->Translate(6 * i, 5, 0);
		}
	}
}

void FrictionTestScene::Draw() const
{
}

void FrictionTestScene::OnSceneActivated()
{
}

void FrictionTestScene::OnSceneDeactivated()
{
}
