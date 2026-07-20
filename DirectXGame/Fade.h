#pragma once
#define NOMINMAX
#include "KamataEngine.h"
#include <algorithm>

using namespace KamataEngine;

/// <summary>
/// フェード
/// </summary>
class Fade {
public:
	/// フェードの状態
	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

	void Initialize();
	void Update();
	void Draw();

	/// フェード開始
	void Start(Status status, float duration);
	/// フェード停止
	void Stop();
	/// フェード終了判定
	bool IsFinished() const;

private:
	Sprite* sprite_ = nullptr;

	Status status_ = Status::None;
	float duration_ = 0.0f;
	float counter_ = 0.0f;
};