#include "stdafx.h"
#include "W1_AssignmentScene.h"

#include "PhysxManager.h"
#include "CubePosColorNorm.h"
#include "SpherePosColorNorm.h"

#define MUL3(vector,multiplier) vector.x *= multiplier; vector.y *= multiplier; vector.z *= multiplier;

void W1_AssignmentScene::Initialize()
{
	EnablePhysxDebugRendering(true);
	m_pPhysxScene->setVisualizationParameter(physx::PxVisualizationParameter::Enum::eBODY_LIN_VELOCITY, 1.f);

	const auto pPhysX = PhysxManager::GetInstance()->GetPhysics();
	const PxMaterial* pDefaultMaterial = pPhysX->createMaterial(.5f, .5f, .05f);

	// GROUNDPLANE
	const XMFLOAT3 groundplaneDimensions{ 100.f,.25f,100.f };
	auto pGroundPlane = new CubePosColorNorm(groundplaneDimensions.x, groundplaneDimensions.y, groundplaneDimensions.z, DirectX::XMFLOAT4{ DirectX::Colors::Gray });
	AddGameObject(pGroundPlane);

	// GROUNDPLANEACTOR
	PxRigidStatic* pGroundPlaneActor = pPhysX->createRigidStatic(PxTransform{ PxIdentity });
	PxBoxGeometry boxGeometry{ groundplaneDimensions.x * 0.5f, groundplaneDimensions.y * 0.5f, groundplaneDimensions.z * 0.5f };

	PxRigidActorExt::createExclusiveShape(*pGroundPlaneActor, boxGeometry, *pDefaultMaterial);

	//Link GROUNDPLANE with GROUNDPLANE_ACTOR
	pGroundPlane->AttachRigidActor(pGroundPlaneActor);

	constexpr int amountOfCubesPerSide{ 10 };
	for (size_t i{ 0 }; i < (amountOfCubesPerSide * amountOfCubesPerSide); ++i)
	{
		m_pCubes.push_back(new CubePosColorNorm(m_CubeDimensions.x, m_CubeDimensions.y, m_CubeDimensions.z));
		AddGameObject(m_pCubes.back());

		PxRigidBody* pCubeActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
		PxBoxGeometry cubeGeometry{ m_CubeDimensions.x * .5f,m_CubeDimensions.y * .5f, m_CubeDimensions.z * .5f };

		PxRigidActorExt::createExclusiveShape(*pCubeActor, cubeGeometry, *pDefaultMaterial);

		m_pCubes.back()->AttachRigidActor(pCubeActor);
	}

	// BALL
	m_pBall = new SpherePosColorNorm(m_Radius, 10, 10, DirectX::XMFLOAT4{ DirectX::Colors::Yellow });
	AddGameObject(m_pBall);
	PxRigidBody* pSphereActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
	PxSphereGeometry sphereGeometry{ m_Radius };
	PxRigidActorExt::createExclusiveShape(*pSphereActor, sphereGeometry, *pDefaultMaterial);
	PxRigidBodyExt::setMassAndUpdateInertia(*pSphereActor, 5.f);
	m_pBall->AttachRigidActor(pSphereActor);
	
	Reset();

	m_IsInitialised = true;
}

void W1_AssignmentScene::Update()
{
	auto pSphereRB = static_cast<PxRigidBody*>(m_pBall->GetRigidActor());
	const constexpr float forceStrength{ 50.f };
	const constexpr float jumpStrength{ 2500.f };
	const constexpr physx::PxForceMode::Enum forceMode{ physx::PxForceMode::eFORCE };
	const constexpr InputTriggerState triggerState{ InputTriggerState::down };
	if (m_SceneContext.GetInput()->IsKeyboardKey(triggerState, VK_UP))
	{
		auto direction = m_SceneContext.GetCamera()->GetRight();
		MUL3(direction, forceStrength);
		pSphereRB->addTorque(physx::PxVec3{ direction.x,direction.y,direction.z }, forceMode);
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(triggerState, VK_DOWN))
	{
		auto direction = m_SceneContext.GetCamera()->GetRight();
		MUL3(direction, -forceStrength);
		pSphereRB->addTorque(physx::PxVec3{ direction.x,direction.y,direction.z }, forceMode);
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(triggerState, VK_RIGHT))
	{
		auto direction = m_SceneContext.GetCamera()->GetForward();
		MUL3(direction, -forceStrength);
		pSphereRB->addTorque(physx::PxVec3{ direction.x,direction.y,direction.z }, forceMode);
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(triggerState, VK_LEFT))
	{
		auto direction = m_SceneContext.GetCamera()->GetForward();
		MUL3(direction, forceStrength);
		pSphereRB->addTorque(physx::PxVec3{ direction.x,direction.y,direction.z }, forceMode);
	}

	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, VK_SPACE))
	{
		pSphereRB->addForce(physx::PxVec3{ 0,jumpStrength,0 }, forceMode);
	}

	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, 'R'))
	{
		Reset();
	}
}

void W1_AssignmentScene::Draw() const
{
}

void W1_AssignmentScene::OnSceneActivated()
{
}

void W1_AssignmentScene::OnSceneDeactivated()
{
}

void W1_AssignmentScene::Reset()
{
	const size_t amountOfCubesPerSide{static_cast<size_t>(std::sqrt(m_pCubes.size()))};
	for (size_t i{ 0 }; i < m_pCubes.size(); ++i)
	{
		m_pCubes[i]->Translate(i % amountOfCubesPerSide * m_CubeDimensions.x + i % amountOfCubesPerSide * .25f, i / amountOfCubesPerSide * m_CubeDimensions.y + i / amountOfCubesPerSide * .5f + .5f, 10.f);
		m_pCubes[i]->RotateDegrees(0, (rand() % 11) - 5, 0);
	}
	m_pBall->Translate(0, m_Radius, 0);
	if (m_IsInitialised)
	{
		auto pSphereRB = static_cast<PxRigidBody*>(m_pBall->GetRigidActor());
		pSphereRB->setForceAndTorque({ 0,0,0 }, { 0,0,0 });
	}
}
