#pragma once
#include "KamataEngine.h"
#include "Player.h"
using namespace KamataEngine;
class Player;
class CameraController {
public:
	enum class Mode {
		kFollow,
		kForcedScroll,
	};
	Mode mode_ = Mode::kFollow;
	void setMode(Mode mode) { mode_ = mode; }
	Mode getMode() const { return mode_; }
	void Initialize(Camera* camera, Player* player);
	void Update();
	void SetTarget(Player* target) { target_ = target; }
	void Reset();
	struct Rect {
		float left = 0.0f;
		float top = 1.0f;
		float right = 0.0f;
		float bottom = 1.0f;
	};
	static inline const float kInterpolationRate = 0.1f;
	static inline const float kVelocityBias = 0.15f;
	static inline const Rect marginArea_ = {-5.0f, 4.0f, 5.0f, 5.0f};
	void SetMovableArea(const Rect& area) { movableArea_ = area; }
	~CameraController();

private:
	Camera* camera_ = nullptr;
	Player* target_ = nullptr;
	float cameraVelocityX_ = 0.0f;

	Vector3 initialTargetPosition_ = {0.0f, 0.0f, 0.0f};
	Rect movableArea_ = {0, 100, 0, 100};

	Vector3 targetOffSet_ = {0.0f, 0.0f, -15.0f};
};