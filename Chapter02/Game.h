// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include <unordered_map>
#include <string>
#include <vector>

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	void AddEnemy(class Enemy* enemy);
	void RemoveEnemy(class Enemy* enemy);
	std::vector<class Enemy*>& GetEnemies() { return mEnemies; }

	void AddHeart(class Heart* heart);
	void RemoveHeart(class Heart* heart);
	std::vector<class Heart*>& GetHearts() { return mHearts; }
	
	SDL_Texture* GetTexture(const std::string& fileName);

	bool mShooting;

	void RemoveLive();
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();
	void Shoot();
	void AddEnemies(int numEnemies);
	void DrawScore(int score, int pos_x);
	void DrawGameOver();
	void AddLives();
	void FreezeUnfreeze();
	void FreezeAllActors();
	
	// Map of textures loaded
	std::unordered_map<std::string, SDL_Texture*> mTextures;

	// All the actors in the game
	std::vector<class Actor*> mActors;
	// Any pending actors
	std::vector<class Actor*> mPendingActors;

	// All the sprite components drawn
	std::vector<class SpriteComponent*> mSprites;

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	Uint32 mTicksCount;
	bool mIsRunning;
	// Track if we're updating actors right now
	bool mUpdatingActors;

	// Game-specific
	class Ship* mShip; // Player's ship

	class Enemy* mEnemy;

	class Laser* mLaser;

	std::vector<class Enemy*> mEnemies;
	std::vector<class Heart*> mHearts;

	int score;
	int lives;

	bool canFreeze;
	bool Freezed;
	int contKillsFreezed;

	bool GameOver;
};
