#define NOMINMAX
#include "CameraController.h"
#include "Player.h"
#include <algorithm>
#include <cmath>

void CameraController::Initialize(Camera* camera, Player* player) {

	camera_ = camera;

	target_ = player;

	if (target_ != nullptr) {

		initialTargetPosition_ = target_->GetWorldTransform().translation_;
	}

	Reset();
}

void CameraController::Update() {

	if (target_ == nullptr) {
		return;
	}

	if (camera_ == nullptr) {
		return;
	}

	// player transform
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();
	// target camera position
	Vector3 destination;

	// camera inertia (NOT player velocity)
	cameraVelocityX_ += (targetVelocity.x - cameraVelocityX_) * 0.08f;

	// keep camera moving even if player stops
	destination.x = targetWorldTransform.translation_.x + targetOffSet_.x + cameraVelocityX_ * 20.0f;

	destination.y = targetWorldTransform.translation_.y + targetOffSet_.y + targetVelocity.y * kVelocityBias;

	destination.z = targetWorldTransform.translation_.z + targetOffSet_.z + targetVelocity.z * kVelocityBias;

	// smooth interpolation every frame
	camera_->translation_.x += (destination.x - camera_->translation_.x) * kInterpolationRate;

	camera_->translation_.y += (destination.y - camera_->translation_.y) * kInterpolationRate;

	camera_->translation_.z += (destination.z - camera_->translation_.z) * kInterpolationRate;

	// movable area clamp
	// camera_->translation_.x = std::clamp(camera_->translation_.x, movableArea_.left, movableArea_.right);

	// camera_->translation_.y = std::clamp(camera_->translation_.y, movableArea_.bottom, movableArea_.top);
	//   keep player inside screen area

	camera_->translation_.x = std::max(camera_->translation_.x, targetWorldTransform.translation_.x + marginArea_.left);
	camera_->translation_.x = std::min(camera_->translation_.x, targetWorldTransform.translation_.x + marginArea_.right);
	camera_->translation_.y = std::max(camera_->translation_.y, targetWorldTransform.translation_.y + marginArea_.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, targetWorldTransform.translation_.y + marginArea_.top);

	//// look at player
	// Vector3 direction;

	// direction.x = targetWorldTransform.translation_.x - camera_->translation_.x;

	// direction.y = targetWorldTransform.translation_.y - camera_->translation_.y;

	// direction.z = targetWorldTransform.translation_.z - camera_->translation_.z;

	//// Y rotation
	// camera_->rotation_.y = std::atan2(direction.x, direction.z);

	//// X rotation
	// float distanceXZ = std::sqrt(direction.x * direction.x + direction.z * direction.z);

	// camera_->rotation_.x = std::atan2(-direction.y, distanceXZ);

	camera_->UpdateMatrix();
}

void CameraController::Reset() {

	if (target_ == nullptr) {
		return;
	}

	if (camera_ == nullptr) {
		return;
	}

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffSet_.x;

	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffSet_.y;

	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffSet_.z;

	camera_->UpdateMatrix();
}

CameraController::~CameraController() {}