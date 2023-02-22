#include "stdafx.h"
#include "TestScene.h"

#include "Logger.h"

#include "CubePosColorNorm.h"
#include "TorusPosColorNorm.h"
#include "CylinderPosColorNorm.h"

enum InputIds : int
{
	Up,
	Down,
	Left,
	Right
};

void TestScene::Initialize()
{
	// CUBE
	m_pBox = new CubePosColorNorm(5.f, 2.f, 1.f);
	AddGameObject(m_pBox);

	m_pBox->Translate(0,1.f,0);

	//CAMERA
	m_SceneContext.GetCamera()->SetPosition(XMFLOAT3{0,4,-25});
	// after clicking in the window, rotation gets reset
	//m_SceneContext.GetCamera()->SetForward(XMFLOAT3{0,-.3f,1});

	//TORUS
	auto pTorus = new TorusPosColorNorm(8.f,50,1.f,10,XMFLOAT4{Colors::Cornsilk});
	AddGameObject(pTorus);

	//CYLINDER
	auto pCyl = new CylinderPosColorNorm(1.5f, 10, 4);
	pTorus->AddChild(pCyl);
	pCyl->Translate(0, 8.f, 0);

	//INPUT
	m_SceneContext.GetInput()->AddInputAction(InputAction
		{ 
			InputIds::Down, InputTriggerState::down, VK_DOWN, -1, XINPUT_GAMEPAD_DPAD_DOWN
		});
	m_SceneContext.GetInput()->AddInputAction(InputAction
		{
			InputIds::Up, InputTriggerState::down, VK_UP, -1, XINPUT_GAMEPAD_DPAD_UP
		});
	m_SceneContext.GetInput()->AddInputAction(InputAction
		{
			InputIds::Right, InputTriggerState::down, VK_RIGHT, -1, XINPUT_GAMEPAD_DPAD_RIGHT
		});
	m_SceneContext.GetInput()->AddInputAction(InputAction
		{
			InputIds::Left, InputTriggerState::down, VK_LEFT, -1, XINPUT_GAMEPAD_DPAD_LEFT
		});
}

void TestScene::Update()
{
	const float totalTime{ m_SceneContext.GetGameTime()->GetTotal() };
	m_pBox->Rotate(0, totalTime, 0);

	DirectX::XMFLOAT3 translation{}; 
	const float  movementSpeed{ 10.f * m_SceneContext.GetGameTime()->GetElapsed() };

	if (m_SceneContext.GetInput()->IsActionTriggered(InputIds::Left))
	{
		translation.x -= movementSpeed;
	}
	if (m_SceneContext.GetInput()->IsActionTriggered(InputIds::Right))
	{
		translation.x += movementSpeed;
	}
	if (m_SceneContext.GetInput()->IsActionTriggered(InputIds::Up))
	{
		translation.y += movementSpeed;
	}
	if (m_SceneContext.GetInput()->IsActionTriggered(InputIds::Down))
	{
		translation.y -= movementSpeed;
	}

	// Get 2 float3s, add them into a vector, and convert to float3
	// Add can be +
	XMStoreFloat3(&translation, XMVectorAdd(XMLoadFloat3(&translation), XMLoadFloat3(&m_pBox->GetPosition())));
	m_pBox->Translate(translation.x, translation.y, translation.z);
}

void TestScene::Draw() const
{

}

void TestScene::OnSceneActivated()
{
	Logger::GetInstance()->LogFormat(LogLevel::Info, L"Scene Activated > \"%ls\"", GetName().c_str());
}

void TestScene::OnSceneDeactivated()
{
}
