#define NOMINMAX
#include "CameraController.h"
#include "MathUtility.h"
#include "Player.h"
#include <algorithm>

void CameraController::Initialize() { camera_.Initialize(); }

void CameraController::Update() {
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const KamataEngine::Vector3& targetVelocity = target_->GetVelocity();

	targetPosition_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	targetPosition_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity.y * kVelocityBias;
	targetPosition_.z = targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity.z * kVelocityBias;

	camera_.translation_.x = Lerp(camera_.translation_.x, targetPosition_.x, kInterpolationRate);
	camera_.translation_.y = Lerp(camera_.translation_.y, targetPosition_.y, kInterpolationRate);
	camera_.translation_.z = Lerp(camera_.translation_.z, targetPosition_.z, kInterpolationRate);

	camera_.translation_.x = std::max(camera_.translation_.x, targetWorldTransform.translation_.x + kMargin.left);
	camera_.translation_.x = std::min(camera_.translation_.x, targetWorldTransform.translation_.x + kMargin.right);
	camera_.translation_.y = std::max(camera_.translation_.y, targetWorldTransform.translation_.y + kMargin.bottom);
	camera_.translation_.y = std::min(camera_.translation_.y, targetWorldTransform.translation_.y + kMargin.top);

	camera_.UpdateMatrix();
}

void CameraController::Reset() {
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	camera_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}