// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include "SDL/SDL_image.h"
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "Enemy.h"
#include "Laser.h"
#include "Heart.h"
#include "BGSpriteComponent.h"
#include <Windows.h>

Game::Game()
	:mWindow(nullptr)
	, mRenderer(nullptr)
	, mIsRunning(true)
	, mUpdatingActors(false)
	, mTicksCount(0)
	, mShooting(false)
	, lives(5)
	, canFreeze(false)
	, Freezed(false)
	, contKillsFreezed(0)
	, score(8)
	, GameOver(false)
{
	
}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 2)", 100, 100, 1024, 768, 0);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	
	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
		return false;
	}

	LoadData();

	mTicksCount = SDL_GetTicks();
	
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	// Process ship input
	mShip->ProcessKeyboard(state);

	if (state[SDL_SCANCODE_SPACE]) {
		if (!mShooting) {
			Shoot();
		}
	}

	if (state[SDL_SCANCODE_LSHIFT]) {
		Game::mShip->mNitro = 2.0f;
	}
	else {
		Game::mShip->mNitro = 1.0f;
	}
}

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	// Update all actors
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update(deltaTime);
	}
	mUpdatingActors = false;

	// Move any pending actors to mActors
	for (auto pending : mPendingActors)
	{
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	// Add any dead actors to a temp vector
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}

	// Delete dead actors (which removes them from mActors)
	for (auto actor : deadActors)
	{
		delete actor;
	}

	//colisao da nave com o inimigo
	Vector2 shipPos = mShip->GetPosition();
	for (auto enemy : mEnemies) {
		Vector2 enemyPos = enemy->GetPosition();
		float diffy = abs(shipPos.y - enemyPos.y);
		float diffx = abs(shipPos.x - enemyPos.x);
		if (diffy <= 50.0f && diffx <= 81.0f) {
			RemoveLive();
			AddEnemies(1);
			enemy->~Enemy();
		}
	}

	//colisao do tiro com o inimigo
	if (mShooting) {
		Vector2 shootPos = mLaser->GetPosition();
		for (auto enemy : mEnemies) {
			Vector2 enemyPos = enemy->GetPosition();
			bool xCollision = shootPos.x >= enemyPos.x;
			bool yCollision = (shootPos.y >= enemyPos.y - 40.0f && shootPos.y <= enemyPos.y + 40.0f);
			if (xCollision && yCollision) {
				score++;
				mShooting = false;
				mLaser->Kill();
				if (!Freezed) {
					AddEnemies(1);
				}
				else {
					contKillsFreezed++;
					if (contKillsFreezed == 2) {
						Freezed = false;
						canFreeze = false;
						AddEnemies(2);
						FreezeUnfreeze();
						contKillsFreezed = 0;
					}
				}
				enemy->~Enemy();
			}
		}
	}

	if (score > 0 && score % 10 == 0) {
		FreezeUnfreeze();
	}

	if (lives <= 0) {
		GameOver = true;
	}

	if (score > 0 && score % 10 == 0) {
		canFreeze = true;
	}
}

void Game::RemoveLive() {
	lives--;
	Heart* heart = mHearts.back();
	heart->~Heart();
}

void Game::Shoot() {
	mLaser = new Laser(this);
	Vector2 pos = mShip->GetPosition();
	mLaser->SetPosition(Vector2(pos.x + 25.0f, pos.y));
	mLaser->SetScale(1.5f);
	mShooting = true;
}

void Game::DrawScore(int score, int pos_x) {
	int ttfResult = TTF_Init();

	TTF_Font* font = TTF_OpenFont("arial.ttf", 18); // define fonte do score
	SDL_Color color = { 255, 255, 255 }; // define a cor do score

	// cria a superficie e renderiza o texto
	SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), color);

	// log de possiveis erros
	if (scoreSurface == NULL) {
		SDL_Log("Failed to create surface: %s", SDL_GetError());
	}
	else {
		int width = 100;
		int heigth = 100;
		// cria a textura com o score a partir dos pixels da superficie
		SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(mRenderer, scoreSurface);
		if (scoreTexture == NULL) {
			SDL_Log("Failed to create texture: %s", SDL_GetError());
		}
		else {
			//passa fonte, dimensoes(WxH) e o score para servir de texto e sua posicao na tela
			TTF_SizeText(font, std::to_string(score).c_str(), &width, &heigth);
		}
		// depois de definida a textura, pode descartar a superficie
		SDL_FreeSurface(scoreSurface);
		// cria o retangulo para exibir o score
		SDL_Rect scoreRect{
			static_cast<int>(pos_x), // exibe score no meio da tela horizontalmente
			50, // coloca o score no topo da tela
			width, // largura do tamanho do score
			heigth // altura do tamanho do score
		};
		// renderiza a textura no retangulo
		SDL_RenderCopy(mRenderer, scoreTexture, NULL, &scoreRect);
		// agora que já renderizou a textura, destruimos o texto ate a proxima atualizacao
		SDL_DestroyTexture(scoreTexture);
	}
}

void Game::DrawGameOver() {
	int ttfResult = TTF_Init();

	TTF_Font* font = TTF_OpenFont("arial.ttf", 25);
	SDL_Color color = { 255, 255, 255 };

	char texto[512] = { 0 };
	SDL_snprintf(texto, 512, "Pontuacao: %d...Pressione ESC para sair.", score);

	SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, texto, color);

	// log de possiveis erros
	if (scoreSurface == NULL) {
		SDL_Log("Failed to create surface: %s", SDL_GetError());
	}
	else {
		int width = 400;
		int heigth = 75;

		SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(mRenderer, scoreSurface);
		if (scoreTexture == NULL) {
			SDL_Log("Failed to create texture: %s", SDL_GetError());
		}
		else {
			TTF_SizeText(font, std::to_string(score).c_str(), &width, &heigth);
		}

		SDL_FreeSurface(scoreSurface);

		SDL_Rect scoreRect{
			1024.0f / 3,
			768.0f / 2,
			400,
			75
		};

		SDL_RenderCopy(mRenderer, scoreTexture, NULL, &scoreRect);
		SDL_DestroyTexture(scoreTexture);
	}
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mRenderer);
	
	// Draw all sprite components
	for (auto sprite : mSprites)
	{
		sprite->Draw(mRenderer);
	}

	DrawScore(score, 1024 / 2);

	if (GameOver) {
		DrawGameOver();
		FreezeAllActors();
	}

	SDL_RenderPresent(mRenderer);
}

void Game::AddEnemies(int numEnemies) {
	const int kNumEnemies = numEnemies;
	for (int i = 0; i < kNumEnemies; i++)
	{
		new Enemy(this);
	}
}

void Game::LoadData()
{
	// Create player's ship
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(100.0f, 384.0f));
	mShip->SetScale(1.5f);

	AddEnemies(4);

	AddLives();

	//--------------------------------Criação do background----------------------------
	// Create actor for the background (this doesn't need a subclass)
	Actor* temp = new Actor(this);
	temp->SetPosition(Vector2(512.0f, 384.0f));
	
	// Create the "far back" background
	BGSpriteComponent* bg = new BGSpriteComponent(temp);
	bg->SetScreenSize(Vector2(1024.0f, 768.0f));//set the screen size for the background to fit in
	//create a vector
	std::vector<SDL_Texture*> bgtexs = {
		GetTexture("Assets/Farback01.png"),
		GetTexture("Assets/Farback02.png")
	};
	//set this vector to the background component
	bg->SetBGTextures(bgtexs);
	bg->SetScrollSpeed(-100.0f);
	
	// Create the closer background
	bg = new BGSpriteComponent(temp, 50);
	bg->SetScreenSize(Vector2(1024.0f, 768.0f));//set the screen size for the background to fit in
	//create a vector
	bgtexs = {
		GetTexture("Assets/Stars.png"),
		GetTexture("Assets/Stars.png")
	};
	//set this vector to the background component
	bg->SetBGTextures(bgtexs);
	bg->SetScrollSpeed(-200.0f);
}

void Game::AddLives() {
	for (int i = 0; i < lives; i++)
	{
		new Heart(this, (float)(i * 30.0f + 50.0f));
	}
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy textures
	for (auto i : mTextures)
	{
		SDL_DestroyTexture(i.second);
	}
	mTextures.clear();
}

void Game::FreezeUnfreeze() {
	if (canFreeze) {
		Freezed = true;
		canFreeze = false;
		FreezeAllActors();
		mShip->SetState(Actor::EActive);
		mLaser->SetState(Actor::EActive);
	}
	else {
		for (auto actor : mActors)
		{
			actor->SetState(Actor::EActive);
		}
		mLaser->Kill();
	}	
}

void Game::FreezeAllActors() {
	for (auto actor : mActors)
	{
		actor->SetState(Actor::EPaused);
	}
}

void Game::AddEnemy(Enemy* enemy) {
	mEnemies.emplace_back(enemy);
}

void Game::RemoveEnemy(Enemy* enemy) {
	auto iter = std::find(mEnemies.begin(), mEnemies.end(), enemy);
	if (iter != mEnemies.end())
	{
		mEnemies.erase(iter);
	}
}

void Game::AddHeart(Heart* heart) {
	mHearts.emplace_back(heart);
}

void Game::RemoveHeart(Heart* heart) {
	auto iter = std::find(mHearts.begin(), mHearts.end(), heart);
	if (iter != mHearts.end())
	{
		mHearts.erase(iter);
	}
}

// =load the textures
SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	SDL_Texture* tex = nullptr;
	// Is the texture already in the map?
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		// Load from file
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
			return nullptr;
		}

		// Create texture from surface
		tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
			return nullptr;
		}

		mTextures.emplace(fileName.c_str(), tex);
	}
	return tex;
}

void Game::Shutdown()
{
	UnloadData();
	IMG_Quit();
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
	TTF_Quit();
}

//add actors
void Game::AddActor(Actor* actor)
{
	// If we're updating actors, need to add to pending
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

//remove any actors
void Game::RemoveActor(Actor* actor)
{
	// Is it in pending actors?
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// Is it in actors?
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

//add sprites
void Game::AddSprite(SpriteComponent* sprite)
{
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for ( ;
		iter != mSprites.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);
}

//remove sprite
void Game::RemoveSprite(SpriteComponent* sprite)
{
	// (We can't swap because it ruins ordering)
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}
