#include "TitleScene.h"
#include "mathUti.h"
#include <numbers>

using namespace KamataEngine;

void TitleScene::Initialize() {
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -15.0f};
	camera_.UpdateMatrix();

	modelPlayer_ = Model::CreateFromOBJ("player", true);
	textureHandlePlayer_ = TextureManager::Load("./Resources/player/player.png");

	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransformPlayer_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransformPlayer_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void TitleScene::Update() {
	timer_ += 1.0f / 60.0f;

	worldTransformPlayer_.translation_.y = std::sin(timer_ * 2.0f) * 0.3f;

	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_, worldTransformPlayer_.translation_);
	worldTransformPlayer_.TransferMatrix();

	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}
}

void TitleScene::Draw() {
	Model::PreDraw();
	modelPlayer_->Draw(worldTransformPlayer_, camera_, textureHandlePlayer_);
	Model::PostDraw();
}