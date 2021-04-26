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
	, verticalDirection(1)
	, qtdMovimentacao(0.0f)
{
	float initialY = (float)(rand() % 718 + 30);
	SetPosition(Vector2((float)(rand() % 300 + 1024), initialY));
	initialYPos = initialY;

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

		if (qtdMovimentacao < 100.0f) {
			verticalDirection = 1.0f;
		}
		else if(qtdMovimentacao > 100.0f && qtdMovimentacao < 200.0f) {
			verticalDirection = -1.0f;
		}
		else if (qtdMovimentacao >= 200.0f) {
			qtdMovimentacao = 0.0f;
		}

		pos.y += mSpeed * verticalDirection;

		if (pos.y < 25.0f)
		{
			pos.y = 25.0f;
		}
		else if (pos.y > 743.0f)
		{
			pos.y = 743.0f;
		}

		qtdMovimentacao += mSpeed/2.0f;
	}

	SetPosition(pos);
}
