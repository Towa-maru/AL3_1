#pragma once
#include "Fade.h"
#include "KamataEngine.h"

using namespace KamataEngine;

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {
public:
	enum class Phase {
		kFadeIn,  /// フェードイン
		kMain,    /// メイン部
		kFadeOut, /// フェードアウト
	};

	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }

	Phase phase_ = Phase::kFadeIn;

	~TitleScene();

private:
	bool finished_ = false;

	Model* modelPlayer_ = nullptr;
	uint32_t textureHandlePlayer_ = 0;

	WorldTransform worldTransformPlayer_;

	Camera camera_;

	float timer_ = 0.0f;

	Fade* fade_ = nullptr;
};