#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

enum class LRDirection {
	kRight,
	kLeft,
};

enum Corner {
	kRightBottom,
	kLeftBottom,
	kRightTop,
	kLeftTop,

	kNumCorner
};

class MapChipField;

struct CollisionMapInfo {
	bool isCeiling = false;
	bool isLanding = false;
	bool isWall = false;
	KamataEngine::Vector3 move = {};
};

class Player {
public:
	void Initialize(Model* model, Camera* camera, const Vector3& position, uint32_t textureHandle);

	void Update();

	void Draw();

	void InputMove();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void CheckMapCollision(CollisionMapInfo& info);

	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	void ApplyCollisionResult(const CollisionMapInfo& info);

	void CeilingCollisionResponse(const CollisionMapInfo& info);

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	static inline const float kBlank = 0.01f;

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	KamataEngine::Camera* camera_ = nullptr;

	Vector3 velocity_ = {};

	static inline const float kAcceleration = 0.01f;

	static inline const float kAttenuation = 0.1f;

	static inline const float kLimitRunSpeed = 0.5f;

	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;

	float turnTimer_ = 0.0f;

	static inline const float kTimeTurn = 0.3f;

	bool onGround_ = true;

	static inline const float kGravityAcceleration = 0.05f;

	static inline const float kLimitFallSpeed = 0.5f;

	static inline const float kJumpAcceleration = 1.0f;

	float groundY_ = 0.0f;

	MapChipField* mapChipField_ = nullptr;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
};