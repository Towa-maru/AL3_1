#include"GaneScene.h"

using namespace KamataEngine;

void GameScene::Initialize() { 
	textureHandle_ = TextureManager::Load("uvChecker.png");
	model_ = Model::Create();

	worldTransform_.Initialize();
	camera_.Initialize();

	debugCamera_ = new DebugCamera(1280, 720);
}

void GameScene::Update() { 
	debugCamera_->Update(); 
}

void GameScene::Draw() { 
	Model::PreDraw();

	model_->Draw(worldTransform_, camera_, textureHandle_);
	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);

	Model::PostDraw();
}
