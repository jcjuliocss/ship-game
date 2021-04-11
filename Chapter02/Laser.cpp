// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Laser.h"
#include "AnimSpriteComponent.h"
#include "Game.h"

Laser::Laser(Game* game)
	:Actor(game)
	, mSpeed(20.0f)
{
	AnimSpriteComponent* asc = new AnimSpriteComponent(this);
	std::vector<SDL_Texture*> anims = {
		game->GetTexture("Assets/Laser.png"),
	};
	//set the textures to the Ship vector of animated sprites
	asc->SetAnimTextures(anims);
}

void Laser::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	Vector2 pos = GetPosition();
	pos.x += mSpeed;
	if (pos.x >= 1024.0f)
	{
		Kill();
		GetGame()->mShooting = false;
	}

	SetPosition(pos);
}

void Laser::Kill() {
	SetState(EDead);
}
