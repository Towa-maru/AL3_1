#pragma once
#include "KamataEngine.h"
#include <numbers>

using namespace KamataEngine;

class Enemy {
public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.05f;

	// 速度
	Vector3 velocity_ = {};

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = -30.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 30.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 1.0f;

	// 経過時間
	float walkTimer_ = 0.0f;
};