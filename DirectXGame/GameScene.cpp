#include "GameScene.h"
#include "MathUtility.h"
#include "Player.h"
#include "WorldTransformUpdate.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	// カメラの初期化
	camera_.farZ = 5000.0f;
	camera_.Initialize();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("player/player.png");

	// 3Dモデルの生成
	model_ = Model::CreateFromOBJ("player", true);

	modelBlock_ = Model::Create();

	modelEnemy_ = Model::CreateFromOBJ("enemy", true);

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	GenerateBlocks();

	// 自キャラの生成
	player_ = new Player();

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);

	// 自キャラの初期化
	player_->Initialize(model_, &camera_, playerPosition, textureHandle_);

	player_->SetMapChipField(mapChipField_);

	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();

	cameraController_->SetMovableArea({0, 100, 0, 100});

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	enemy_ = new Enemy();
	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(9, 18);
	enemy_->Initialize(modelEnemy_, &camera_, enemyPosition);

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_);
}

void GameScene::GenerateBlocks() {
	for (std::vector<WorldTransform*>& row : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : row) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	uint32_t numBlockVertical = mapChipField_->GetNumBlockVertical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVertical);
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				worldTransformBlocks_[i][j] = worldTransform;
			}
		}
	}
}

GameScene::~GameScene() {
	delete player_;
	delete model_;
	delete modelBlock_;
	delete debugCamera_;
	delete skydome_;
	delete modelSkydome_;
	delete mapChipField_;
	delete cameraController_;
	delete enemy_;
	delete modelEnemy_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Update() {
	// 自キャラの更新
	player_->Update();

	// デバッグカメラの更新
	debugCamera_->Update();

	enemy_->Update();

	skydome_->Update();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			UpdateWorldTransform(*worldTransformBlock);
		}
	}

	cameraController_->Update();

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	// カメラの処理
	if (isDebugCameraActive_) {
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		// カメラコントローラのカメラを使う
		camera_.matView = cameraController_->GetCamera().matView;
		camera_.matProjection = cameraController_->GetCamera().matProjection;
		camera_.TransferMatrix();
	}
}

void GameScene::Draw() {
	Model::PreDraw();

	player_->Draw();

	enemy_->Draw();

	skydome_->Draw(camera_);

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	Model::PostDraw();
}