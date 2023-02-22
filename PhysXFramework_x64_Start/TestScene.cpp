#include "stdafx.h"
#include "TestScene.h"

#include "Logger.h"

#include "CubePosColorNorm.h"

void TestScene::Initialize()
{
	m_pBox = new CubePosColorNorm(5.f, 2.f, 1.f);
	AddGameObject(m_pBox);

	m_pBox->Translate(0,1.f,0);
}

void TestScene::Update()
{
	const float totalTime{ GetSceneContext().GetGameTime()->GetTotal() };
	m_pBox->Rotate(0, totalTime, 0);
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
