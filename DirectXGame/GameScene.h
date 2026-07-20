#pragma once
#include "CameraController.h"
#include "Enemy.h"
#include "MapChipField.h"
#include "Player.h"
#include "skydome.h"
#include <KamataEngine.h>
#include <vector>
class GameScene {
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	std::list<Enemy*> enemies_;
	bool isDebugCameraActive_ = false;

public:
	void Initialize();

	void Update();

	void Draw();

	void GenerateBlocks();

	void CheckAllCollisions();
	~GameScene();

private:
	uint32_t textureHandle_ = 0;
	uint32_t textureHandlePlayer_ = 0;
	uint32_t textureHandleEnemy_ = 0;
	MapChipField* mapchipField_;
	KamataEngine::Model* blockModel_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	Model* enemyModel_ = nullptr;
	CameraController* cameraController_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	skydome* skydome_ = nullptr;
	Player* player_ = nullptr;
};