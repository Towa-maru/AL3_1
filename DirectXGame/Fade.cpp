#define NOMINMAX
#include "Fade.h"
#include <algorithm>

using namespace KamataEngine;

void Fade::Initialize() {
	uint32_t textureHandle = TextureManager::Load("./Resources/deathParticle/white1x1.png");
	sprite_ = Sprite::Create(textureHandle, {0.0f, 0.0f});
	sprite_->SetSize(Vector2(1280.0f, 720.0f));
	sprite_->SetColor(Vector4(0, 0, 0, 1));
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Update() {
	switch (status_) {
	case Status::None:
		/// 何もしない
		break;

	case Status::FadeOut:
		/// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		/// フェード継続時間に達したら打ち止め
		counter_ = std::min(counter_, duration_);
		/// 経過時間がduration_に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		/// 完了したらNoneに
		if (counter_ >= duration_) {
			status_ = Status::None;
		}
		break;

	case Status::FadeIn:
		/// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		/// フェード継続時間に達したら打ち止め
		counter_ = std::min(counter_, duration_);
		/// 経過時間がduration_に近づくほどアルファ値を小さくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(1.0f - counter_ / duration_, 0.0f, 1.0f)));
		/// 完了したらNoneに
		if (counter_ >= duration_) {
			status_ = Status::None;
		}
		break;
	}
}

void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFinished() const {
	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		return counter_ >= duration_;
	}
	return true;
}

void Fade::Draw() {
	if (status_ == Status::None) {
		return;
	}
	Sprite::PreDraw();
	sprite_->Draw();
	Sprite::PostDraw();
}