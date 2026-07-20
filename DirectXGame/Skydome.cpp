#include "skydome.h"

using namespace KamataEngine;

void skydome::initialize() {

	textureHandleSky_ = TextureManager::Load("./Resources/SkyDome/sky_sphere.png");

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	worldTransform_.Initialize();

	worldTransform_.scale_ = {-100.0f, 100.0f, 100.0f};
}

void skydome::update() { worldTransform_.TransferMatrix(); }

void skydome::Draw(Camera& camera) { modelSkydome_->Draw(worldTransform_, camera, textureHandleSky_); }

skydome::~skydome() { delete modelSkydome_; }