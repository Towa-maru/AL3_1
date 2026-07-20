#pragma once
#include "CameraController.h"
#include "Enemy.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include <vector>

// ゲームシーン
class GameScene {
public:
	// 初期化
	void Initialize();

	void GenerateBlocks();

	// 更新
	void Update();

	// 描画
	void Draw();

	~GameScene();

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデルデータ
	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Model* modelBlock_ = nullptr;

	KamataEngine::Model* modelSkydome_ = nullptr;

	MapChipField* mapChipField_;

	// カメラ
	KamataEngine::Camera camera_;

	// 自キャラ
	Player* player_ = nullptr;

	Skydome* skydome_ = nullptr;

	Model* modelEnemy_ = nullptr;
	Enemy* enemy_ = nullptr;

	CameraController* cameraController_ = nullptr;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
};