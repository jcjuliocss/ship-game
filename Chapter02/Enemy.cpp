// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Enemy.h"
#include "AnimSpriteComponent.h"
#include "Game.h"

Enemy::Enemy(Game* game)
	:Actor(game)
	, mSpeed(4.0f)
{
	SetPosition(Vector2((float)(rand() % 300 + 1024), (float)(rand() % 718 + 30)));

	AnimSpriteComponent* asc = new AnimSpriteComponent(this);
	std::vector<SDL_Texture*> anims = {
		game->GetTexture("Assets/Enemy01.png"),
		game->GetTexture("Assets/Enemy02.png"),
		game->GetTexture("Assets/Enemy03.png"),
		game->GetTexture("Assets/Enemy04.png"),
		game->GetTexture("Assets/Enemy05.png"),
		game->GetTexture("Assets/Enemy06.png"),
	};

	asc->SetAnimTextures(anims);

	game->AddEnemy(this);
}

Enemy::~Enemy() {
	GetGame()->RemoveEnemy(this);
}


void Enemy::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	Vector2 pos = GetPosition();
	if (pos.x <= 0.0f)
	{
		GetGame()->RemoveLive();
		this->~Enemy();
		new Enemy(GetGame());
	}
	else {
		pos.x -= mSpeed;
	}

	SetPosition(pos);
}
