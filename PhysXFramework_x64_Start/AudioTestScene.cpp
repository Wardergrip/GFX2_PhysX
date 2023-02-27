#include "stdafx.h"
#include "AudioTestScene.h"

#include "PhysxManager.h"
#include "SpherePosColorNorm.h"
#include "SoundManager.h"

void AudioTestScene::Initialize()
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

	// INPUT
	const auto pInput = m_SceneContext.GetInput();
	pInput->AddInputAction({ Play2D,InputTriggerState::pressed, VK_SPACE });
	pInput->AddInputAction({ IncreaseVolume,InputTriggerState::pressed, VK_UP });
	pInput->AddInputAction({ DecreaseVolume,InputTriggerState::pressed, VK_DOWN });

	const auto pFmod = SoundManager::GetInstance()->GetSystem();
	//SOUND 2D
	FMOD::Sound* pSound2D{ nullptr };
	FMOD_RESULT result = pFmod->createStream("Resources/Sounds/wave.mp3", FMOD_2D | FMOD_LOOP_NORMAL, nullptr, &pSound2D);
	SoundManager::GetInstance()->ErrorCheck(result);

	result = pFmod->playSound(pSound2D, nullptr, true, &m_pChannel2D);
	SoundManager::GetInstance()->ErrorCheck(result);

	//SOUND 3D
	FMOD::Sound* pSound3D{ nullptr };
	result = pFmod->createStream("Resources/Sounds/Thrones.mp3", FMOD_3D | FMOD_LOOP_NORMAL | FMOD_3D_LINEARROLLOFF, nullptr, &pSound3D);
	SoundManager::GetInstance()->ErrorCheck(result);

	result = pFmod->playSound(pSound3D, nullptr, true, &m_pChannel3D);
	SoundManager::GetInstance()->ErrorCheck(result);

	m_pChannel3D->set3DMinMaxDistance(0.f, 100.f);
	//m_pChannel3D->set3DAttributes();
}

inline FMOD_VECTOR ToFmod(const XMFLOAT3& v) // DIRECTX
{
	return FMOD_VECTOR(v.x, v.y, v.z);
}

inline FMOD_VECTOR ToFmod(const PxVec3& v) // PHYSX
{
	return FMOD_VECTOR(v.x, v.y, v.z);
}

void AudioTestScene::Update()
{
	auto pSphereRigidBody = static_cast<PxRigidDynamic*>(m_pSphere->GetRigidActor());
	const constexpr float forceStrength{ 7.f };
	const constexpr physx::PxForceMode::Enum forceMode{ physx::PxForceMode::eFORCE };
	const constexpr InputTriggerState inputMode{ InputTriggerState::down };
	if (m_SceneContext.GetInput()->IsKeyboardKey(inputMode, VK_UP))
	{
		pSphereRigidBody->addTorque(physx::PxVec3{ forceStrength,0.f,0.f }, forceMode);
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(inputMode, VK_DOWN))
	{
		pSphereRigidBody->addTorque(physx::PxVec3{ -forceStrength,0.f,0.f }, forceMode);
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(inputMode, VK_RIGHT))
	{
		pSphereRigidBody->addTorque(physx::PxVec3{ 0.f,0.f,-forceStrength }, forceMode);
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(inputMode, VK_LEFT))
	{
		pSphereRigidBody->addTorque(physx::PxVec3{ 0.f,0.f,forceStrength }, forceMode);
	}

	// Sound Input
	if (m_SceneContext.GetInput()->IsActionTriggered(Play2D))
	{
		bool isPaused;
		m_pChannel2D->getPaused(&isPaused);
		m_pChannel2D->setPaused(!isPaused);
	}
	if (m_SceneContext.GetInput()->IsActionTriggered(Play3D))
	{
		bool isPaused;
		m_pChannel3D->getPaused(&isPaused);
		m_pChannel3D->setPaused(!isPaused);
	}

	if (m_SceneContext.GetInput()->IsActionTriggered(IncreaseVolume))
	{
		float volume{};
		m_pChannel2D->getVolume(&volume);

		volume += 0.1f;
		Clamp(volume, 10.f, 0.f);

		m_pChannel2D->setVolume(volume);
		m_pChannel3D->setVolume(volume);
	}
	if (m_SceneContext.GetInput()->IsActionTriggered(DecreaseVolume))
	{
		float volume{};
		m_pChannel2D->getVolume(&volume);

		volume -= 0.1f;
		Clamp(volume, 10.f, 0.f);

		m_pChannel2D->setVolume(volume);
		m_pChannel3D->setVolume(volume);
	}

	//3D sound attributes
	auto pCam = m_SceneContext.GetCamera();
	auto pos = ToFmod(pCam->GetPosition());
	auto forward = ToFmod(pCam->GetForward());
	auto up = ToFmod(pCam->GetUp());

	FMOD_VECTOR vel{};
	const float deltaT = m_SceneContext.GetGameTime()->GetElapsed();
	vel.x = (pos.x - m_PreviousCamPos.x) / deltaT;
	vel.y = (pos.y - m_PreviousCamPos.y) / deltaT;
	vel.z = (pos.z - m_PreviousCamPos.z) / deltaT;
	SoundManager::GetInstance()->GetSystem()->set3DListenerAttributes(0,&pos,&vel,&forward,&up);
	auto spherePos = ToFmod(m_pSphere->GetPosition());
	auto sphereVel = ToFmod(m_pSphere->GetRigidActor()->is<PxRigidDynamic>()->getLinearVelocity());
	m_pChannel3D->set3DAttributes(&spherePos,&sphereVel);

	m_PreviousCamPos = pos;
}

void AudioTestScene::Draw() const
{
}
