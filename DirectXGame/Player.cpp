#include "Player.h"
#include "WorldTransformUpdate.h"
#include <cassert>

using namespace KamataEngine;

void Player::Initialize(Model* model, uint32_t textureHandle, Camera* camera) {
	assert(model);
	assert(camera);

	model_ = model;
	textureHandle_ = textureHandle;
	camera_ = camera;

	worldTransform_.Initialize();
}

void Player::Update() { UpdateWorldTransform(worldTransform_); }

void Player::Draw() { model_->Draw(worldTransform_, *camera_, textureHandle_); }