#include "Enemy.h"
#include "WorldTransformUpdate.h"
#include <cassert>
#include <numbers>

using namespace KamataEngine;

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	// 速度を設定する（左方向に移動）
	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;
}

void Enemy::Update() {
	// 移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	walkTimer_ += 1.0f / 60.0f;

	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);
	// -1〜+1 を 0〜+1 に変換してから Lerp で角度範囲に変換
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	// 度をラジアンに変換してX軸回転に適用
	worldTransform_.rotation_.x = degree * std::numbers::pi_v<float> / 180.0f;

	// ワールド行列の更新
	UpdateWorldTransform(worldTransform_);
}

void Enemy::Draw() {
	// ワールドトランスフォーム、カメラを渡して3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}