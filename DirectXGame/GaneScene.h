#pragma once
#include"KamataEngine.h"

class GameScene {
private:
	uint32_t textureHandle_ = 0;
	KamataEngine::Model* model_ = nullptr;

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera camera_;

	KamataEngine::DebugCamera* debugCamera_ = nullptr;

public:
	void Initialize();

	void Update();

	void Draw();
};