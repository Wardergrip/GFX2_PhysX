#pragma once
#include "GameScene.h"
#include <vector>

class W1_AssignmentScene final : public GameScene
{
public:
	W1_AssignmentScene() :GameScene(L"W1_AssignmentScene") {}
	~W1_AssignmentScene() override = default;
	W1_AssignmentScene(const W1_AssignmentScene& other) = delete;
	W1_AssignmentScene(W1_AssignmentScene&& other) noexcept = delete;
	W1_AssignmentScene& operator=(const W1_AssignmentScene& other) = delete;
	W1_AssignmentScene& operator=(W1_AssignmentScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

private:
	void Reset();

	std::vector<GameObject*> m_pCubes;
	GameObject* m_pBall;
	const XMFLOAT3 m_CubeDimensions{ 1.f,1.f,1.f };
	const float m_Radius{ 1.f };

	bool m_IsInitialised{ false };
};