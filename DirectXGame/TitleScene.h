#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {
public:
	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }

private:
	bool finished_ = false;

	Model* modelPlayer_ = nullptr;
	uint32_t textureHandlePlayer_ = 0;

	WorldTransform worldTransformPlayer_;

	Camera camera_;

	float timer_ = 0.0f;
};