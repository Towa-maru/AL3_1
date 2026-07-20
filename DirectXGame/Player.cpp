#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "WorldTransformUpdate.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <numbers>
#include <stdio.h>
#include <windows.h>

using namespace KamataEngine;

// Debug helper - writes to Visual Studio Output window
static void DebugLog(const char* format, ...) {
	char buf[512];
	va_list args;
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);
	OutputDebugStringA(buf);
}

void Player::Initialize(Model* model, Camera* camera, const Vector3& position, uint32_t textureHandle) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::InputMove() {
	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x += kAcceleration;
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x -= kAcceleration;
			}
			velocity_.x += acceleration.x;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y += kJumpAcceleration;
		}
	} else {
		velocity_.y -= kGravityAcceleration;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::Update() {
	// ①移動入力
	InputMove();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;

	// ②マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	// ③判定結果を反映して移動させる
	ApplyCollisionResult(collisionMapInfo);

	// ④天井に接触している場合の処理
	CeilingCollisionResponse(collisionMapInfo);

	WallCollisionResponse(collisionMapInfo);

	// ⑥接地状態の切り替え処理
	SwitchGroundState(collisionMapInfo);

	// ⑦旋回制御
	float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
	float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
	worldTransform_.rotation_.y = destinationRotationY;

	// DEBUG: log every frame to Output window
	DebugLog("pos.y=%.3f vel.y=%.3f onGround=%d\n", worldTransform_.translation_.y, velocity_.y, (int)onGround_);

	// ⑧行列計算
	UpdateWorldTransform(worldTransform_);
}

void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	if (info.move.y <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] =
		    CornerPosition({worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = rect.bottom - positionsNew[kLeftTop].y - kBlank;
		info.isCeiling = true;
		DebugLog("UP HIT: rect.bottom=%.3f top.y=%.3f move.y=%.3f\n", rect.bottom, positionsNew[kLeftTop].y, info.move.y);
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	if (info.move.y >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] =
		    CornerPosition({worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = rect.top - (worldTransform_.translation_.y - kHeight / 2.0f) - kBlank;
		info.isLanding = true;
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	if (info.move.x <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] =
		    CornerPosition({worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = rect.left - positionsNew[kRightTop].x - kBlank;
		info.isWall = true;
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] =
		    CornerPosition({worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = rect.left - positionsNew[kRightBottom].x - kBlank;
		info.isWall = true;
	}
}

void Player::ApplyCollisionResult(const CollisionMapInfo& info) {
	worldTransform_.translation_.x += info.move.x;
	worldTransform_.translation_.y += info.move.y;
	worldTransform_.translation_.z += info.move.z;
}

void Player::CeilingCollisionResponse(const CollisionMapInfo& info) {
	if (info.isCeiling) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0.0f;
	}
}

void Player::WallCollisionResponse(const CollisionMapInfo& info) {
	if (info.isWall) {
		velocity_.x = 0.0f;
	}
}

void Player::SwitchGroundState(const CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			MapChipType mapChipType;
			bool hit = false;

			MapChipField::IndexSet indexSet;

			Vector3 leftCheck = CornerPosition(worldTransform_.translation_, kLeftBottom);
			leftCheck.y -= kGroundStickyOffset;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(leftCheck);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			Vector3 rightCheck = CornerPosition(worldTransform_.translation_, kRightBottom);
			rightCheck.y -= kGroundStickyOffset;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(rightCheck);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			DebugLog("STICKY: leftCheck.y=%.3f hit=%d\n", leftCheck.y, (int)hit);

			if (!hit) {
				onGround_ = false;
			}
		}
	} else {
		if (info.isLanding) {
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
			DebugLog("LANDED: pos.y=%.3f\n", worldTransform_.translation_.y);
		}
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};

	return {center.x + offsetTable[static_cast<uint32_t>(corner)].x, center.y + offsetTable[static_cast<uint32_t>(corner)].y, center.z + offsetTable[static_cast<uint32_t>(corner)].z};
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_, textureHandle_); }