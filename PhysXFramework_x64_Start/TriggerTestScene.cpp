#include "stdafx.h"
#include "TriggerTestScene.h"

#include "PhysxManager.h"
#include "SpherePosColorNorm.h"
#include "Logger.h"

void TriggerTestScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	const auto pPhysX = PhysxManager::GetInstance()->GetPhysics();
	const auto pDefaultMaterial = pPhysX->createMaterial(.5f, .5f, .2f);


	// Ground Plane
	const auto pGroundActor = pPhysX->createRigidStatic(PxTransform{ {PxPiDivTwo,{0,0,1}} });
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, *pDefaultMaterial);
	m_pPhysxScene->addActor(*pGroundActor);

	// SPhere
	m_pSphere = new SpherePosColorNorm(1.f, 10, 10, XMFLOAT4{ Colors::Gray });
	AddGameObject(m_pSphere);

	const auto pSphereActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
	PxRigidActorExt::createExclusiveShape(*pSphereActor, PxSphereGeometry{ 1.f }, *pDefaultMaterial);
	m_pSphere->AttachRigidActor(pSphereActor);

	m_pSphere->Translate(0.f, 2.f, 0.f);

	// TRIGGER LEFT
	m_pTriggerLeft = pPhysX->createRigidStatic(PxTransform{ {-8.f,.5f,0.f} });
	const auto pLShape = PxRigidActorExt::createExclusiveShape(*m_pTriggerLeft, PxBoxGeometry{ .5f,.5f,.5f }, *pDefaultMaterial);

	pLShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pLShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	m_pPhysxScene->addActor(*m_pTriggerLeft);

	// TRIGGER RIGHT
	m_pTriggerRight = pPhysX->createRigidStatic(PxTransform{ {8.f,.5f,0.f} });
	const auto pRShape = PxRigidActorExt::createExclusiveShape(*m_pTriggerRight, PxBoxGeometry{ .5f,.5f,.5f }, *pDefaultMaterial);

	pRShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pRShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	m_pPhysxScene->addActor(*m_pTriggerRight);

}

void TriggerTestScene::Update()
{
	auto pSphereRigidBody = static_cast<PxRigidDynamic*>(m_pSphere->GetRigidActor());
	const constexpr float forceStrength{ 500.f };
	const constexpr physx::PxForceMode::Enum forceMode{ physx::PxForceMode::eFORCE };
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, VK_UP))
	{
		pSphereRigidBody->addTorque(physx::PxVec3{ forceStrength,0.f,0.f }, forceMode);
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, VK_DOWN))
	{
		pSphereRigidBody->addTorque(physx::PxVec3{ -forceStrength,0.f,0.f }, forceMode);
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, VK_RIGHT))
	{
		pSphereRigidBody->addTorque(physx::PxVec3{ 0.f,0.f,-forceStrength }, forceMode);
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, VK_LEFT))
	{
		pSphereRigidBody->addTorque(physx::PxVec3{ 0.f,0.f,forceStrength }, forceMode);
	}

	if (m_IsTriggerLeft)
	{
		pSphereRigidBody->addForce(PxVec3{ 8,10,0 }, PxForceMode::eIMPULSE);
		m_IsTriggerLeft = false;
	}
	else if (m_IsTriggerRight)
	{
		pSphereRigidBody->addForce(PxVec3{ -8,10,0 }, PxForceMode::eIMPULSE);
		m_IsTriggerRight = false;
	}
}

void TriggerTestScene::Draw() const
{
}

void TriggerTestScene::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (auto i{ 0 }; i < count; ++i)
	{
		// Ignore deleted shapes
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		const PxTriggerPair& pair = pairs[i];

		if (pair.triggerActor == m_pTriggerLeft)
		{
			if (pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND) // ENTERED
			{
				Logger::GetInstance()->LogInfo(L"Sphere entered");
				m_IsTriggerLeft = true;
			}
			else if (pair.status == PxPairFlag::eNOTIFY_TOUCH_LOST) // LEFT
			{
				Logger::GetInstance()->LogInfo(L"Sphere left");
			}
		}
		else if (pair.triggerActor == m_pTriggerRight)
		{
			if (pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND) // ENTERED
			{
				Logger::GetInstance()->LogInfo(L"Sphere entered");
				m_IsTriggerRight = true;
			}
			else if (pair.status == PxPairFlag::eNOTIFY_TOUCH_LOST) // LEFT
			{
				Logger::GetInstance()->LogInfo(L"Sphere left");
			}
		}
	}
}
