#pragma once
#include "KamataEngine.h"
#include <array>
#include <numbers>

using namespace KamataEngine;

class DeathParticles {
public:
	void Initialize(Model* model, uint32_t textureHandle, Camera* camera, const Vector3& position);
	void Update();
	void Draw();

private:
	static inline const uint32_t kNumParticles = 8;

	std::array<WorldTransform, kNumParticles> worldTransforms_;

	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
	Camera* camera_ = nullptr;

	// 存続時間（消滅までの時間）秒
	static inline const float kDuration = 1.0f;
	// 移動の速さ
	static inline const float kSpeed = 0.1f;
	// 分割した1個分の角度
	static inline const float kAngleUnit = 2.0f * std::numbers::pi_v<float> / kNumParticles;

	// 終了フラグ
	bool isFinished_ = false;
	// 経過時間カウント
	float counter_ = 0.0f;

	// 色変更オブジェクト
	ObjectColor objectColor_;
	// 色の数値
	Vector4 color_;
};