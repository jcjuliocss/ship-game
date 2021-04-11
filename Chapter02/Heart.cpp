// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Heart.h"
#include "AnimSpriteComponent.h"
#include "Game.h"

Heart::Heart(Game* game, float posX)
	:Actor(game)
{
	SetPosition(Vector2(posX, 25.0f));
	SetScale(1.5f);

	AnimSpriteComponent* asc = new AnimSpriteComponent(this);
	std::vector<SDL_Texture*> anims = {
		game->GetTexture("Assets/Heart.png"),
	};
	//set the textures to the Ship vector of animated sprites
	asc->SetAnimTextures(anims);

	game->AddHeart(this);
}

Heart::~Heart() {
	GetGame()->RemoveHeart(this);
}
