#pragma once
#include "KamataEngine.h"
class skydome {
public:
	void initialize();
	void update();
	void Draw(KamataEngine::Camera& camera);
	~skydome();

private:
	int32_t textureHandleSky_ = 0;

	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* modelSkydome_ = nullptr;
};