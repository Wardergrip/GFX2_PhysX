#include "stdafx.h"
#include "BoxTorqueScene.h"

#include "PhysxManager.h"
#include "CubePosColorNorm.h"

void BoxTorqueScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	const auto pPhysX = PhysxManager::GetInstance()->GetPhysics();
	const PxMaterial* pDefaultMaterial = pPhysX->createMaterial(.5f, .5f, .2f);

	//GROUNDPLANE
	auto pGroundActor = pPhysX->createRigidStatic(PxTransform{ PxQuat{PxPiDivTwo,PxVec3{0,0,1}} });
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, *pDefaultMaterial);
	m_pPhysxScene->addActor(*pGroundActor);

	// CUBE
	const XMFLOAT3 actorDimensions{ 1.f,1.f,1.f };
	m_pCube = new CubePosColorNorm(actorDimensions.x, actorDimensions.y, actorDimensions.z);
	AddGameObject(m_pCube);

	// CUBEACTOR
	PxRigidDynamic* pCubeActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
	PxBoxGeometry boxGeometry{ actorDimensions.x * 0.5f, actorDimensions.y * 0.5f, actorDimensions.y * 0.5f };

	PxRigidActorExt::createExclusiveShape(*pCubeActor, boxGeometry, *pDefaultMaterial);

	//Link CUBE with CUBE_ACTOR
	m_pCube->AttachRigidActor(pCubeActor);


	m_pCube->Translate(0, 3, 0);
}

void BoxTorqueScene::Update()
{
	auto pCubeRigidBody = static_cast<PxRigidBody*>(m_pCube->GetRigidActor());
	const constexpr float forceStrength{ 500.f };
	const constexpr physx::PxForceMode::Enum forceMode{ physx::PxForceMode::eFORCE };
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, VK_UP))
	{
		pCubeRigidBody->addTorque(physx::PxVec3{ 0.f,0.f,forceStrength }, forceMode);
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, VK_DOWN))
	{
		pCubeRigidBody->addTorque(physx::PxVec3{ 0.f,0.f,-forceStrength }, forceMode);
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, VK_RIGHT))
	{
		pCubeRigidBody->addTorque(physx::PxVec3{ forceStrength,0.f,0.f }, forceMode);
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, VK_LEFT))
	{
		pCubeRigidBody->addTorque(physx::PxVec3{ -forceStrength,0.f,0.f }, forceMode);
	}
}

void BoxTorqueScene::Draw() const
{
}

void BoxTorqueScene::OnSceneActivated()
{
}

void BoxTorqueScene::OnSceneDeactivated()
{
}
