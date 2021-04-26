// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Actor.h"
class Enemy : public Actor
{
public:
	Enemy(class Game* game);
	~Enemy();
	void UpdateActor(float deltaTime) override;
	float initialYPos;
	float mSpeed;
	float verticalDirection;
	float qtdMovimentacao;
private:
};