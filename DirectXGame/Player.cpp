#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "mathUti.h"
#include <Windows.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <numbers>

using namespace KamataEngine;

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandlePlayer, KamataEngine::Camera* camera, Vector3& position) {

	assert(model);

	model_ = model;
	textureHandlePlayer_ = textureHandlePlayer;
	camera_ = camera;

	worldTransform_.Initialize();

	// player start position
	worldTransform_.translation_ = position;

	// player size
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// player rotation
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {

	// horizontal movement - runs using onground_ from PREVIOUS frame
	if (onground_) {
		if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {

			Vector3 acceleration = {};

			if (Input::GetInstance()->PushKey(DIK_D)) {
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
				}
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = 1.0f / 60.0f;
				acceleration.x += kAcceleration;
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
			} else if (Input::GetInstance()->PushKey(DIK_A)) {
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
				}
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = 1.0f / 60.0f;
				acceleration.x -= kAcceleration;
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
			}

			velocity_.x += acceleration.x;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

			if (turnTimer_ < KTimeTurn) {
				turnTimer_ += 1.0f / 60.0f;
				float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
				float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
				float t = turnTimer_ / KTimeTurn;
				t = std::clamp(t, 0.0f, 1.0f);
				t = t * t * (3.0f - 2.0f * t);
				worldTransform_.rotation_.y = (1.0f - t) * turnFirstRotationY_ + t * destinationRotationY;
			}

		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}
	}

	// jump before resetting onground_
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) && onground_) {
		velocity_.y = kJumpAcceleration;
	}

	// reset onground AFTER movement and jump check
	onground_ = false;

	// gravity always applies
	velocity_.y -= kGravityAcceleration;
	velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

	// setup collision info
	CollisionMapInfo collisionmapInfo;
	collisionmapInfo.velocityAfterCollision.x = velocity_.x;
	collisionmapInfo.velocityAfterCollision.y = velocity_.y;
	collisionmapInfo.velocityAfterCollision.z = velocity_.z;

	// run collision
	CollisionMap(collisionmapInfo);

	// apply position
	CollisionDeceted(collisionmapInfo);

	// apply velocity changes from collision
	TopCollision(collisionmapInfo);

	// affine matrix
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// send to GPU
	worldTransform_.TransferMatrix();
}
Player::AABB Player::GetAABB() {

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f + kBlank, worldPos.y - kHeight / 2.0f + kBlank, worldPos.z};
	aabb.max = {worldPos.x + kWidth / 2.0f - kBlank, worldPos.y + kHeight / 2.0f - kBlank, worldPos.z};
	return aabb;
}
Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;
	return worldPos;
}
const WorldTransform& Player::GetWorldTransform() const { return worldTransform_; }

void Player::Draw() { model_->Draw(worldTransform_, *camera_, textureHandlePlayer_); }

void Player::CollisionMap(CollisionMapInfo& info) {
	CollisionMapTop(info);    // 上
	CollisionMapBottom(info); // 下
	CollisionMapRight(info);  // 右
	CollisionMapleft(info);   // 左
}

void Player::onCollision(const Enemy* enemy) {

	(void)enemy;

	velocity_.y += kJumpAcceleration;

	isDead_ = true;
}

void Player::CollisionMapTop(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionNew;
	Vector3 movedTranslation = {worldTransform_.translation_.x, worldTransform_.translation_.y + info.velocityAfterCollision.y, worldTransform_.translation_.z};

	for (uint32_t i = 0; i < positionNew.size(); i++) {
		positionNew[i] = CornerPosition(movedTranslation, static_cast<Corner>(i));
	}

	// only check when moving up
	if (info.velocityAfterCollision.y <= 0) {
		return;
	}

	MapChipType mapChipType;
	bool hit = false;
	IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetmapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetmapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);

		// セル境界の判定：移動前のセルと衝突検出したセルのyIndexを比較
		IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);

		if (indexSetNow.yIndex != indexSet.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			worldTransform_.translation_.y = rect.bottom - kHeight / 2.0f;
			info.velocityAfterCollision.y = 0.0f;
			info.isHitUp = true;
		}
	}
}

void Player::CollisionMapBottom(CollisionMapInfo& info) {

	if (info.velocityAfterCollision.y >= 0.0f) {
		return;
	}

	Vector3 moved = {worldTransform_.translation_.x, worldTransform_.translation_.y + info.velocityAfterCollision.y, worldTransform_.translation_.z};

	Vector3 leftBottom = CornerPosition(moved, kLeftBottom);

	Vector3 rightBottom = CornerPosition(moved, kRightBottom);

	bool hit = false;
	IndexSet hitIndex{};

	IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(leftBottom);

	if (mapChipField_->GetmapChipTypeByIndex(index.xIndex, index.yIndex) == MapChipType::kBlock) {

		hit = true;
		hitIndex = index;
	}

	index = mapChipField_->GetMapChipIndexSetByPosition(rightBottom);

	if (mapChipField_->GetmapChipTypeByIndex(index.xIndex, index.yIndex) == MapChipType::kBlock) {

		hit = true;
		hitIndex = index;
	}

	if (hit) {

		// セル境界の判定：移動前のセルと着地検出したセルのyIndexを比較
		IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);

		if (indexSetNow.yIndex != hitIndex.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(hitIndex.xIndex, hitIndex.yIndex);

			float correctY = rect.top + kHeight / 2.0f;

			info.velocityAfterCollision.y = correctY - worldTransform_.translation_.y;

			info.isHitDown = true;
		}
	}
}
void Player::CollisionMapRight(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionNew;
	Vector3 movedTranslation = {worldTransform_.translation_.x + info.velocityAfterCollision.x, worldTransform_.translation_.y, worldTransform_.translation_.z};

	for (uint32_t i = 0; i < positionNew.size(); i++) {
		positionNew[i] = CornerPosition(movedTranslation, static_cast<Corner>(i));
	}

	// only check when moving right
	if (info.velocityAfterCollision.x <= 0) {
		return;
	}

	MapChipType mapChipType;
	bool hit = false;
	IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetmapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetmapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);

		// セル境界の判定：移動前のセルと衝突検出したセルのxIndexを比較
		IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);

		if (indexSetNow.xIndex != indexSet.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			// push player to LEFT of the tile
			worldTransform_.translation_.x = rect.left - kWidth / 2.0f;
			info.velocityAfterCollision.x = 0.0f;
			info.isHitRight = true;
		}
	}
}

void Player::CollisionMapleft(CollisionMapInfo& info) {

	std::array<Vector3, kNumCorner> positionNew;

	Vector3 movedTranslation = {worldTransform_.translation_.x + info.velocityAfterCollision.x, worldTransform_.translation_.y, worldTransform_.translation_.z};

	for (uint32_t i = 0; i < positionNew.size(); i++) {
		positionNew[i] = CornerPosition(movedTranslation, static_cast<Corner>(i));
	}

	// moving left only
	if (info.velocityAfterCollision.x >= 0) {
		return;
	}

	MapChipType mapChipType;
	bool hit = false;

	IndexSet indexSetTop;
	IndexSet indexSetBottom;

	indexSetTop = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);

	mapChipType = mapChipField_->GetmapChipTypeByIndex(indexSetTop.xIndex, indexSetTop.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSetBottom = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);

	mapChipType = mapChipField_->GetmapChipTypeByIndex(indexSetBottom.xIndex, indexSetBottom.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {

		// use the tile actually hit
		IndexSet indexSet = indexSetTop;

		if (mapChipField_->GetmapChipTypeByIndex(indexSetBottom.xIndex, indexSetBottom.yIndex) == MapChipType::kBlock) {

			indexSet = indexSetBottom;
		}

		// セル境界の判定：移動前のセルと衝突検出したセルのxIndexを比較
		IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);

		if (indexSetNow.xIndex != indexSet.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			// push player to right side of block
			worldTransform_.translation_.x = rect.right + kWidth / 2.0f - 0.001f;

			info.velocityAfterCollision.x = 0.0f;
			info.isHitLeft = true;
		}
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	const float kAdjustX = 0.03f;
	const float kAdjustY = 0.03f;

	Vector3 offsetTable[kNumCorner] = {

	    {+kWidth / 2.0f - kAdjustX, -kHeight / 2.0f + kAdjustY, 0},
	    {-kWidth / 2.0f + kAdjustX, -kHeight / 2.0f + kAdjustY, 0},
	    {+kWidth / 2.0f - kAdjustX, +kHeight / 2.0f - kAdjustY, 0},
	    {-kWidth / 2.0f + kAdjustX, +kHeight / 2.0f - kAdjustY, 0},
	};

	Vector3 offset = offsetTable[static_cast<uint32_t>(corner)];

	return {center.x + offset.x, center.y + offset.y, center.z + offset.z};
}
void Player::CollisionDeceted(const CollisionMapInfo& info) {
	// only apply if not already snapped by collision
	if (!info.isHitDown && !info.isHitUp) {
		worldTransform_.translation_.y += info.velocityAfterCollision.y;
	}
	if (!info.isHitLeft && !info.isHitRight) {
		worldTransform_.translation_.x += info.velocityAfterCollision.x;
	}
	worldTransform_.translation_.z += info.velocityAfterCollision.z;
}

void Player::TopCollision(const CollisionMapInfo& info) {
	if (info.isHitUp) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0.0f;
	}
	if (info.isHitDown) {
		onground_ = true;
		velocity_.y = 0.0f; // stop gravity accumulating
	}
	if (info.isHitLeft || info.isHitRight) {
		velocity_.x = 0.0f;
	}
}
Player::~Player() {}