#pragma once
#include "GameScene.h"
#include <vector>
class BouncingBallsScene final : public GameScene
{
public:
	BouncingBallsScene() :GameScene(L"Bouncingballs") {}
	~BouncingBallsScene() override = default;
	BouncingBallsScene(const BouncingBallsScene& other) = delete;
	BouncingBallsScene(BouncingBallsScene&& other) noexcept = delete;
	BouncingBallsScene& operator=(const BouncingBallsScene& other) = delete;
	BouncingBallsScene& operator=(BouncingBallsScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

private:
	std::vector<GameObject*> m_pBalls;
	const float m_DropHeight = 10.f;
};

