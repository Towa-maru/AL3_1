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

	fade_ = new Fade();
	fade_->Initialize();

	/// タイトル開始時にフェードイン開始
	static const float kFadeDuration = 1.0f;
	fade_->Start(Fade::Status::FadeIn, kFadeDuration);
}

static const float kFadeDuration = 1.0f;

void TitleScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;

	case Phase::kMain:
		timer_ += 1.0f / 60.0f;
		worldTransformPlayer_.translation_.y = std::sin(timer_ * 2.0f) * 0.3f;
		worldTransformPlayer_.matWorld_ = MakeAffineMatrix(worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_, worldTransformPlayer_.translation_);
		worldTransformPlayer_.TransferMatrix();

		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, kFadeDuration);
		}
		break;

	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}

void TitleScene::Draw() {
	Model::PreDraw();
	modelPlayer_->Draw(worldTransformPlayer_, camera_, textureHandlePlayer_);
	Model::PostDraw();

	/// フェードは必ず末尾に描画
	fade_->Draw();
}

TitleScene::~TitleScene() { delete fade_; }