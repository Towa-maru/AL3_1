#pragma once

#include "KamataEngine.h"
#include "MapChipField.h"
using namespace KamataEngine;

class MapChipField;
class Enemy;
class Player {
public:
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

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	bool onground_ = true;
	LRDirection lrDirection_ = LRDirection::kRight;
	Vector3 velocity_ = {0, 0, 0};
	Vector3 GetWorldPosition();

	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.1f;
	static inline const float kLimitRunSpeed = 1.0f;
	static inline const float KTimeTurn = 0.3f;
	static inline const float kGravityAcceleration = 0.01f;
	static inline const float kLimitFallSpeed = 1.0f;
	static inline const float kJumpAcceleration = 0.3f;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.01f;

	struct AABB {

		Vector3 min;
		Vector3 max;
	};

	AABB GetAABB();

	struct CollisionMapInfo {
		bool isHitLeft = false;
		bool isHitRight = false;
		bool isHitUp = false;
		bool isHitDown = false;
		Vector3 velocityAfterCollision = {0, 0, 0};
	};

	const WorldTransform& GetWorldTransform() const;
	const Vector3& GetVelocity() const { return velocity_; }
	void CollisionDeceted(const CollisionMapInfo& info);
	void TopCollision(const CollisionMapInfo& info);
	void Initialize(KamataEngine::Model* model, uint32_t textureHandlePlayer, KamataEngine::Camera* camera, Vector3& position);

	void Update();

	void Draw();

	void setMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void CollisionMap(CollisionMapInfo& info);

	void onCollision(const Enemy* enemy);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	bool isDead_ = false;

	bool IsDead() const { return isDead_; }

	~Player();

private:
	void CollisionMapTop(CollisionMapInfo& info);
	void CollisionMapBottom(CollisionMapInfo& info);
	void CollisionMapRight(CollisionMapInfo& info);
	void CollisionMapleft(CollisionMapInfo& info);
	MapChipField* mapChipField_ = nullptr;

	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	uint32_t textureHandlePlayer_ = 0;

	Camera* camera_ = nullptr;
};