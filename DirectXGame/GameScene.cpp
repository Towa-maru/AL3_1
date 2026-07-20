#include "GameScene.h"
#include "CameraController.h"
#include "Enemy.h"
#include "MapChipField.h"
#include "Player.h"
#include "mathUti.h"
#include "skydome.h"
#include <cmath>

using namespace KamataEngine;

void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("./Resources/block/block.png");
	blockModel_ = Model::CreateFromOBJ("block", true);
	camera_.Initialize();
	debugCamera_ = new DebugCamera(1280, 720);
	isDebugCameraActive_ = false;

	mapchipField_ = new MapChipField();
	mapchipField_->LoadMapChipCsv("Resources/blocks.csv");

	player_ = new Player();
	textureHandlePlayer_ = TextureManager::Load("./Resources/player/player.png");
	model_ = Model::CreateFromOBJ("player", true);
	Vector3 playerPosition = mapchipField_->GetmapChipPositionByIndex(1, 17);
	player_->Initialize(model_, textureHandlePlayer_, &camera_, playerPosition);
	player_->setMapChipField(mapchipField_);

	textureHandleParticle_ = TextureManager::Load("./Resources/deathParticle/white1x1.png");
	deathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);

	textureHandleEnemy_ = TextureManager::Load("./Resources/enemy/enemy.png");
	enemyModel_ = Model::CreateFromOBJ("enemy", true);
	for (int32_t i = 0; i < 3; i++) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemysPosition = mapchipField_->GetmapChipPositionByIndex(10 + i * 5, 17);
		newEnemy->Initialize(enemyModel_, textureHandleEnemy_, &camera_, enemysPosition);
		newEnemy->setMapChipField(mapchipField_);
		enemies_.push_back(newEnemy);
	}

	cameraController_ = new CameraController();
	cameraController_->Initialize(&camera_, player_);
	CameraController::Rect movableArea = {0.0f, 100.0f, 100.0f, 0.0f};
	cameraController_->SetMovableArea(movableArea);

	skydome_ = new skydome();
	skydome_->initialize();

	camera_.farZ = 1000.0f;
	camera_.UpdateMatrix();

	GenerateBlocks();

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	phase_ = Phase::kFadeIn;
}

void GameScene::UpdateGamePlay() {
	player_->Update();
	for (Enemy* enemy : enemies_) {
		enemy->update();
	}
	skydome_->update();
	cameraController_->Update();
	CheckAllCollisions();
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->scale_ = {1.0f, 1.0f, 1.0f};
			worldTransformBlock->rotation_.y = 0.00f;
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			worldTransformBlock->TransferMatrix();
		}
	}

	/// 自キャラがデス状態なら
	if (player_->IsDead()) {
		/// 死亡演出フェーズに切り替え
		phase_ = Phase::kDeath;
		/// 自キャラの座標を取得
		const Vector3& deathParticlesPosition = player_->GetWorldPosition();
		/// 自キャラの座標にデスパーティクルを発生、初期化
		deathParticles_ = new DeathParticles();
		deathParticles_->Initialize(deathParticleModel_, textureHandleParticle_, &camera_, deathParticlesPosition);
	}
}

void GameScene::UpdateDeath() {
	if (deathParticles_) {
		deathParticles_->Update();
	}
	for (Enemy* enemy : enemies_) {
		enemy->update();
	}
	skydome_->update();
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->scale_ = {1.0f, 1.0f, 1.0f};
			worldTransformBlock->rotation_.y = 0.00f;
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			worldTransformBlock->TransferMatrix();
		}
	}
	if (deathParticles_ && deathParticles_->IsFinished()) {
		finished_ = true;
	}
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		/// ゲームプレイフェーズの処理
		break;
	case Phase::kDeath:
		/// デス演出フェーズの処理
		break;
	}
}

void GameScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kPlay:
		UpdateGamePlay();
		break;
	case Phase::kDeath:
		UpdateDeath();
		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

#ifdef DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif
	debugCamera_->Update();
	if (isDebugCameraActive_) {
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
	}
}

void GameScene::Draw() {
	Model::PreDraw();
	skydome_->Draw(camera_);
	if (phase_ == Phase::kPlay) {
		player_->Draw();
	}
	if (deathParticles_) {
		deathParticles_->Draw();
	}
	for (Enemy* enemy : enemies_) {
		enemy->draw();
	}
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			if (!blockModel_)
				continue;
			blockModel_->Draw(*worldTransformBlock, camera_, textureHandle_);
		}
	}
	Model::PostDraw();

	/// フェードは必ず末尾に描画
	fade_->Draw();
}

void GameScene::GenerateBlocks() {

	uint32_t numBlockVirtical = mapchipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapchipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; i++) {

		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
	for (uint32_t i = 0; i < numBlockVirtical; i++) {

		for (uint32_t j = 0; j < numBlockHorizontal; j++) {

			if (mapchipField_->GetmapChipTypeByIndex(j, i) != MapChipType::kBlock) {
				continue;
			}

			worldTransformBlocks_[i][j] = new WorldTransform();

			worldTransformBlocks_[i][j]->Initialize();

			Vector3 blockPosition = mapchipField_->GetmapChipPositionByIndex(j, i);

			worldTransformBlocks_[i][j]->translation_ = blockPosition;
		}
	}
}

void GameScene::CheckAllCollisions() {

	Player::AABB aabb1;
	Enemy::AABB aabb2;

	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemies_) {
		aabb2 = enemy->GetAABB();
		if (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x && aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) {
			// hit!
			player_->onCollision(enemy);
			enemy->onCollision(player_);
		}
	}
}

// =========================
// Destructor
// =========================
GameScene::~GameScene() {

	delete blockModel_;
	delete model_;
	delete debugCamera_;
	delete player_;
	delete deathParticles_;
	delete deathParticleModel_;
	delete skydome_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	delete mapchipField_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			delete worldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();
}