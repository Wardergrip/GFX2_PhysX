#include "stdafx.h"
#include "W2_AssignmentScene.h"

#include "PhysxManager.h"
#include "MeshObject.h"
#include "ContentManager.h"
#include "SpherePosColorNorm.h"
#include "CubePosColorNorm.h"
#include "SoundManager.h"

void W2_AssignmentScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	const auto pPhysX = PhysxManager::GetInstance()->GetPhysics();
	const auto pDefaultMaterial = pPhysX->createMaterial(.5f, .5f, .2f);

	// Ground Plane
	{
		const auto pGroundActor = pPhysX->createRigidStatic(PxTransform{ {PxPiDivTwo,{0,0,1}} });
		PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, *pDefaultMaterial);
		m_pPhysxScene->addActor(*pGroundActor);
	}

	// Triangle Level
	{
		m_pLevel = new MeshObject(L"Resources/Meshes/Level.ovm");
		AddGameObject(m_pLevel);

		const auto pLevelMesh = ContentManager::GetInstance()->Load<PxTriangleMesh>(L"Resources/Meshes/Level.ovpt");
		const auto pLevelActor = pPhysX->createRigidStatic(PxTransform{ PxIdentity });

		PxRigidActorExt::createExclusiveShape(*pLevelActor, PxTriangleMeshGeometry{ pLevelMesh }, *pDefaultMaterial);

		m_pLevel->AttachRigidActor(pLevelActor);
	}

	// PlayerSphere
	{
		m_pPlayerSphere = new SpherePosColorNorm(1.f, 10, 10, XMFLOAT4{ Colors::Gray });
		AddGameObject(m_pPlayerSphere);

		const auto pSpherePlayerActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
		PxRigidActorExt::createExclusiveShape(*pSpherePlayerActor, PxSphereGeometry{ 1.f }, *pDefaultMaterial);
		m_pPlayerSphere->AttachRigidActor(pSpherePlayerActor);
	}

	// BOXES
	{
		const XMFLOAT3 boxDimensions{ 1.5f,1.5f,1.5f };
		const float boxMass{ .1f };
		// BlueBox
		{
			m_pBlueBox = new CubePosColorNorm(boxDimensions.x, boxDimensions.y, boxDimensions.z, XMFLOAT4{ DirectX::Colors::Blue });
			AddGameObject(m_pBlueBox);

			const auto pBlueBoxActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
			PxRigidActorExt::createExclusiveShape(*pBlueBoxActor, PxBoxGeometry{ boxDimensions.x * 0.5f,boxDimensions.y * 0.5f,boxDimensions.z * 0.5f }, *pDefaultMaterial);

			PxRigidBodyExt::setMassAndUpdateInertia(*pBlueBoxActor, boxMass);
			m_pBlueBox->AttachRigidActor(pBlueBoxActor);
		}
		// RedBox
		{
			m_pRedBox = new CubePosColorNorm(boxDimensions.x, boxDimensions.y, boxDimensions.z, XMFLOAT4{ DirectX::Colors::Red });
			AddGameObject(m_pRedBox);

			const auto pRedBoxActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
			PxRigidActorExt::createExclusiveShape(*pRedBoxActor, PxBoxGeometry{ boxDimensions.x * 0.5f,boxDimensions.y * 0.5f,boxDimensions.z * 0.5f }, *pDefaultMaterial);

			PxRigidBodyExt::setMassAndUpdateInertia(*pRedBoxActor, boxMass);
			m_pRedBox->AttachRigidActor(pRedBoxActor);
		}
	}

	// TRIGGERS
	{
		const XMFLOAT3 triggerDimensions{ 2.f,0.5f,5.f };
		// BlueTrigger
		{
			m_pBlueTrigger = new CubePosColorNorm(triggerDimensions.x, triggerDimensions.y, triggerDimensions.z, XMFLOAT4{ DirectX::Colors::Blue });
			auto pRigidStatic = pPhysX->createRigidStatic(PxTransform{ PxIdentity });
			const auto pBlueShape = PxRigidActorExt::createExclusiveShape(*pRigidStatic, PxBoxGeometry{ triggerDimensions.x * 0.5f, triggerDimensions.y * 0.5f, triggerDimensions.z * 0.5f }, *pDefaultMaterial);

			pBlueShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			pBlueShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

			m_pBlueTrigger->AttachRigidActor(pRigidStatic);
			AddGameObject(m_pBlueTrigger);
			m_pBlueTrigger->Translate(-7.25f, 2.25f, 0.f);
			m_BlueTriggered = false;
		}
		// RedTrigger
		{
			m_pRedTrigger = new CubePosColorNorm(triggerDimensions.x, triggerDimensions.y, triggerDimensions.z, XMFLOAT4{ DirectX::Colors::Red });
			auto pRigidStatic = pPhysX->createRigidStatic(PxTransform{ PxIdentity });
			const auto pRedShape = PxRigidActorExt::createExclusiveShape(*pRigidStatic, PxBoxGeometry{ triggerDimensions.x * 0.5f, triggerDimensions.y * 0.5f, triggerDimensions.z * 0.5f }, *pDefaultMaterial);

			pRedShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			pRedShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

			m_pRedTrigger->AttachRigidActor(pRigidStatic);
			AddGameObject(m_pRedTrigger);
			m_pRedTrigger->Translate(6.75f, 2.25f, 0.f);
			m_RedTriggered = false;
		}
	}

	// HATCHES
	{
		const XMFLOAT3 hatchesDimensions{ 2.f,0.5f,5.f };
		// BlueHatch
		{
			m_pBlueHatch = new CubePosColorNorm(hatchesDimensions.x, hatchesDimensions.y, hatchesDimensions.z, XMFLOAT4{ DirectX::Colors::Blue });
			AddGameObject(m_pBlueHatch);

			const auto pBlueHatchActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
			PxRigidActorExt::createExclusiveShape(*pBlueHatchActor, PxBoxGeometry{ hatchesDimensions.x * 0.5f,hatchesDimensions.y * 0.5f,hatchesDimensions.z * 0.5f }, *pDefaultMaterial);
			pBlueHatchActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
			m_pBlueHatch->AttachRigidActor(pBlueHatchActor);
		}
		// RedHatch
		{
			m_pRedHatch = new CubePosColorNorm(hatchesDimensions.x, hatchesDimensions.y, hatchesDimensions.z, XMFLOAT4{ DirectX::Colors::Red });
			AddGameObject(m_pRedHatch);

			const auto pRedHatchActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
			PxRigidActorExt::createExclusiveShape(*pRedHatchActor, PxBoxGeometry{ hatchesDimensions.x * 0.5f,hatchesDimensions.y * 0.5f,hatchesDimensions.z * 0.5f }, *pDefaultMaterial);
			pRedHatchActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
			m_pRedHatch->AttachRigidActor(pRedHatchActor);
		}
	}

	// SPHERES
	{
		constexpr const float radius{ 1.f };
		constexpr const int slices{ 10 };
		constexpr const int stacks{ 10 };
		// BlueSphere
		{
			m_pBlueSphere = new SpherePosColorNorm(radius, slices, stacks, XMFLOAT4{ Colors::Gray });
			AddGameObject(m_pBlueSphere);

			const auto pBlueSphereActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
			PxRigidActorExt::createExclusiveShape(*pBlueSphereActor, PxSphereGeometry{ radius }, * pDefaultMaterial);
			m_pBlueSphere->AttachRigidActor(pBlueSphereActor);
		}
		// RedSphere
		{
			m_pRedSphere = new SpherePosColorNorm(radius, slices, stacks, XMFLOAT4{ Colors::Gray });
			AddGameObject(m_pRedSphere);

			const auto pRedSphereActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
			PxRigidActorExt::createExclusiveShape(*pRedSphereActor, PxSphereGeometry{ radius }, * pDefaultMaterial);
			m_pRedSphere->AttachRigidActor(pRedSphereActor);
		}
	}

	
	// SOUND
	{
		const auto pFmod = SoundManager::GetInstance()->GetSystem();
		//SOUND 2D
		FMOD_RESULT result = pFmod->createSound("Resources/Sounds/bell.mp3", FMOD_2D | FMOD_LOOP_OFF, nullptr, &m_pSound2D);
		SoundManager::GetInstance()->ErrorCheck(result);

		result = pFmod->playSound(m_pSound2D, nullptr, true, &m_pChannel2D);
		SoundManager::GetInstance()->ErrorCheck(result);
	}

	ResetScene();
}

void W2_AssignmentScene::Update()
{
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, 'R'))
	{
		ResetScene();
	}

	// PlayerSphere input
	auto pPlayerSphereRigidBody = static_cast<PxRigidDynamic*>(m_pPlayerSphere->GetRigidActor());
	const constexpr float forceStrength{ 7.f };
	const constexpr float jumpStrength{ 5.f };
	const constexpr PxForceMode::Enum forceMode{ PxForceMode::eFORCE };
	const constexpr InputTriggerState inputMode{ InputTriggerState::down };
	const auto pInputManager = m_SceneContext.GetInput();
	if (pInputManager->IsKeyboardKey(inputMode, VK_UP))
	{
		pPlayerSphereRigidBody->addTorque(physx::PxVec3{ forceStrength,0.f,0.f }, forceMode);
	}
	if (pInputManager->IsKeyboardKey(inputMode, VK_DOWN))
	{
		pPlayerSphereRigidBody->addTorque(physx::PxVec3{ -forceStrength,0.f,0.f }, forceMode);
	}
	if (pInputManager->IsKeyboardKey(inputMode, VK_RIGHT))
	{
		pPlayerSphereRigidBody->addTorque(physx::PxVec3{ 0.f,0.f,-forceStrength }, forceMode);
	}
	if (pInputManager->IsKeyboardKey(inputMode, VK_LEFT))
	{
		pPlayerSphereRigidBody->addTorque(physx::PxVec3{ 0.f,0.f,forceStrength }, forceMode);
	}
	if (pInputManager->IsKeyboardKey(InputTriggerState::pressed, VK_SPACE))
	{
		pPlayerSphereRigidBody->addForce(physx::PxVec3{ 0.f,jumpStrength ,0.f }, PxForceMode::eIMPULSE);
	}

	// Trigger logic
	if (m_BlueTriggered)
	{
		m_pBlueHatch->RotateDegrees(0, 0, 90);
		m_pBlueHatch->Translate(-7.5f, 16.f, 0.f);
		m_BlueTriggered = false;

		SoundManager::GetInstance()->GetSystem()->playSound(m_pSound2D, nullptr, false, &m_pChannel2D);
	}
	if (m_RedTriggered)
	{
		m_pRedHatch->RotateDegrees(0, 0, 90);
		m_pRedHatch->Translate(7.25f, 16.f, 0.f);
		m_RedTriggered = false;

		//m_pChannel2D->setPaused(false);

		SoundManager::GetInstance()->GetSystem()->playSound(m_pSound2D, nullptr, false, &m_pChannel2D);
	}
}

void W2_AssignmentScene::Draw() const
{
}

void W2_AssignmentScene::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (auto i{ 0 }; i < count; ++i)
	{
		// Ignore deleted shapes
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		const PxTriggerPair& pair = pairs[i];

		if (pair.triggerActor == m_pBlueTrigger->GetRigidActor())
		{
			if (pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND) // ENTERED
			{
				//Logger::GetInstance()->LogInfo(L"Sphere blue entered");
				m_BlueTriggered = true;
			}
			else if (pair.status == PxPairFlag::eNOTIFY_TOUCH_LOST) // LEFT
			{
				//Logger::GetInstance()->LogInfo(L"Sphere blue left");
			}
		}
		else if (pair.triggerActor == m_pRedTrigger->GetRigidActor())
		{
			if (pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND) // ENTERED
			{
				//Logger::GetInstance()->LogInfo(L"Sphere red entered");
				m_RedTriggered = true;
			}
			else if (pair.status == PxPairFlag::eNOTIFY_TOUCH_LOST) // LEFT
			{
				//Logger::GetInstance()->LogInfo(L"Sphere red left");
			}
		}
	}
}

void W2_AssignmentScene::ResetScene()
{
	m_pPlayerSphere->Translate(0.f, 4.f, 0.f);
	m_pBlueBox->Translate(-4.f, 4.5f, 0.f);
	m_pRedBox->Translate(4.f, 4.5f, 0.f);

	m_pBlueHatch->Translate(-9.f, 17.f, 0.f);
	m_pBlueHatch->Rotate(0, 0, 0);

	m_pRedHatch->Translate(9.f, 17.f, 0.f);
	m_pRedHatch->Rotate(0, 0, 0);

	m_pBlueSphere->Translate(-9.f, 19.f, 0.f);
	m_pRedSphere->Translate(9.f, 19.f, 0.f);
}
