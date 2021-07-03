#include<iostream>
#include<string>
#include<algorithm>
using namespace std;

#include "olcConsoleGameEngine.h"

class SpaceShip : public olcConsoleGameEngine {
public:
	SpaceShip() {
		m_sAppName = L"SpaceShip";
	}
private:
	struct sStarsObject
	{
		int x;
		int y;
		int dx; //velocity
		int dy;
		int nSize; // size of the objects
		int angle; //player's ship can rotate
	};

	struct sSpaceObject
	{
		float x;
		float y;
		float dx; //velocity
		float dy;
		int nSize; // size of the objects
		float angle; //player's ship can rotate
	};

	
	vector<sSpaceObject> vecSpaceShip;
	vector<sSpaceObject> vecBullets;


	vector<sStarsObject> vecStars;
	
	sSpaceObject player;
	bool bDead = false ;
	int nScore = 0;


	vector<pair<float, float>> vecModelShip;
	vector<pair<float, float>> vecModelObjects;
	
protected:
	// Overwrite olsConsoleGamingEngine, call when the application launches
	virtual bool OnUserCreate()
	{
		vecSpaceShip.push_back({ 20.0f, 20.0f, 8.0f, -6.0f, (int)16, 0.0f });

		//Initialize Player Position
		player.x = ScreenWidth() / 2.0f;
		player.y = ScreenHeight() / 2.0f;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;

		vecModelShip = {
			{ 0.0f, -5.0f },
			{-2.5f, +2.5f },
			{+2.5f, +2.5f }

		}; //triangle for player


		// shape of the object
		int verts = 20;
		for (int i = 0; i < verts; i++) {
			//float radius = 5.0f; // circle
			float radius  = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
			float a = ((float)i / (float)verts) * 6.3f ;
			vecModelObjects.push_back(make_pair(radius * sinf(a), radius * cosf(a)));
		}

		//Stars
		
		for (int i = 0; i < 100; i++) {
			//srand(time(NULL));
			int x = rand() % (160 - 3 + 1) + 3;
			int y = rand() % (100 - 3 + 1) + 3;
			vecStars.push_back({ x, y, 0, 0, 1, 0 });

		}

		

		resetGame();
		return true;
	}

	void resetGame() {

		//Initialize Player Position
		player.x = ScreenWidth() / 2.0f;
		player.y = ScreenHeight() / 2.0f;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;


		vecSpaceShip.clear();
		vecBullets.clear();

		vecSpaceShip.push_back({ 20.0f, 20.0f, 8.0f, -6.0f, (int)16, 0.0f });
		vecSpaceShip.push_back({ 90.0f, 20.0f, -5.0f, 3.0f, (int)16, 0.0f });

	
		bDead = false;
		nScore = 0;

	}
	// Overwrite olsConsoleGamingEngine, update the screen
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (bDead) {
			resetGame();
		}

		

		//Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);

		//Steer
		if (m_keys[VK_LEFT].bHeld) {
			player.angle -= 5.0f * fElapsedTime;
		}
		if (m_keys[VK_RIGHT].bHeld) {
			player.angle += 5.0f * fElapsedTime;
		}

		//THRUST
		if (m_keys[VK_UP].bHeld) {
			//Acceleration  changes VELOCITY respect to time
			player.dx += sin(player.angle) * 20.0f * fElapsedTime;
			player.dy += -cos(player.angle) * 20.0f * fElapsedTime;
		}

		//Velocity changes the position(of course)
		player.x += player.dx * fElapsedTime;
		player.y += player.dy * fElapsedTime;

		//Keep player in the gamespace 
		WrapCoordinates(player.x, player.y, player.x, player.y);

		//If ship hit one of the objects
		for (auto& a : vecSpaceShip) {
			if (IsPointInsideCircle(a.x, a.y, a.nSize, player.x, player.y)) {
				bDead = true;
			}
		}

		//Shooting(Firing)
		if (m_keys[VK_SPACE].bReleased) {
			vecBullets.push_back({  player.x, player.y, 50.0f * sinf(player.angle) , -50.0f * cosf(player.angle), 0, 0 });
		}

		// update and draw objects
		for (auto& a : vecSpaceShip) {
			a.x += a.dx * fElapsedTime;
			a.y += a.dy * fElapsedTime;
			a.angle += 0, 5 * fElapsedTime;
			WrapCoordinates(a.x, a.y, a.x, a.y);
			DrawWireFrameModel(vecModelObjects, a.x, a.y, a.angle, a.nSize, FG_RED );
		
		}

		vector<sSpaceObject> newSpaceShips ;

		// update and draw bullets
		for (auto& b : vecBullets) {
			b.x += b.dx * fElapsedTime;
			b.y += b.dy * fElapsedTime;
			WrapCoordinates(b.x, b.y, b.x, b.y);
			Draw(b.x, b.y);

			// check if bullets were collapsed with the objects
			for (auto& a : vecSpaceShip) {
				if (IsPointInsideCircle(a.x, a.y, a.nSize, b.x, b.y)) {
					//Object was hitted
					//If the Objects is big enough, it will split in to 2 objects with half size of origin
					if (a.nSize > 4) {
						//Create 2 child objects
						float angle1 = ((float)rand() / (float)RAND_MAX) * 6.3f; 
						float angle2 = ((float)rand() / (float)RAND_MAX) * 6.3f;
						newSpaceShips.push_back({ a.x, a.y,10.0f * sinf(angle1), 10.0f * cosf(angle1),(int)a.nSize >> 1, 0.0f});
						newSpaceShips.push_back({ a.x, a.y,10.0f * sinf(angle2), 10.0f * cosf(angle2),(int)a.nSize >> 1, 0.0f });
					}
					a.x = -100; // if the size < 4 , the object will be killed
					nScore += 10;
				}
			}
		}

		

		//Remove the off zone bullets
		if (vecBullets.size() > 0)
		{
			auto i = remove_if(vecBullets.begin(), vecBullets.end(), [&](sSpaceObject o) { return (o.x < 1 || o.y < 1 || o.x >= ScreenWidth() - 1 || o.y >= ScreenHeight() - 1); });
			if (i != vecBullets.end())
				vecBullets.erase(i);
		}


		// Append new asteroids to existing vector
		for (auto a : newSpaceShips)
			vecSpaceShip.push_back(a);

		//Remove the original objects when it was hitted
		if (vecSpaceShip.size() > 0)
		{
			auto i = remove_if(vecSpaceShip.begin(), vecSpaceShip.end(), [&](sSpaceObject o) { return (o.x < 0 ); });
			if (i != vecSpaceShip.end())
				vecSpaceShip.erase(i);
		}

		//Check if there is any objects left, if no , next level
		if (vecSpaceShip.empty()) {
			// Level Clear
			nScore += 1000; // Large score for level progression
			vecSpaceShip.clear();
			vecBullets.clear();

			// Add two new asteroids, but in a place where the player is not, we'll simply
			// add them 90 degrees left and right to the player, their coordinates will
			// be wrapped by the  next asteroid update
			vecSpaceShip.push_back({  30.0f * sinf(player.angle - 3.15f / 2.0f) + player.x,
											  30.0f * cosf(player.angle - 3.15f / 2.0f) + player.y,
											  10.0f * sinf(player.angle), 10.0f * cosf(player.angle), (int)16, 0.0f });

			vecSpaceShip.push_back({  30.0f * sinf(player.angle + 3.15f / 2.0f) + player.x,
											  30.0f * cosf(player.angle + 3.15f / 2.0f) + player.y,
											  10.0f * sinf(-player.angle), 10.0f * cosf(-player.angle),(int)16 , 0.0f });
		}

		//Draw ship
		DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle , 1);
		//Draw score
		DrawString(4, 4, L"SCORE: " + to_wstring(nScore));
		//Draw stars
		for (auto& a : vecStars) {
			
			Draw2(a.x,a.y);

		}

		return true;
	}



	void WrapCoordinates(float ix, float iy, float& ox, float& oy) {
		ox = ix;
		oy = iy;
		// mapping the console screen so the objects cannot get lost
		if (ix < 0.0f) {
			ox = ix + (float)ScreenWidth();
		}
		if (ix >= (float)ScreenWidth()) {
			ox = ix - (float)ScreenWidth();
		}
		if (iy < 0.0f) {
			oy = iy + (float)ScreenHeight();
		}
		if (iy >= (float)ScreenHeight()) {
			oy = iy - (float)ScreenHeight();
		}
	}
	// overwrite the draw function of the olcConsole to draw it properly.
	virtual void Draw(int x, int y, short c = 0x2588, short col = FG_GREEN)
	{
		float fx, fy;
		WrapCoordinates(x, y, fx, fy);
		olcConsoleGameEngine::Draw(fx, fy, c, col);
	}

	 void Draw2(int x, int y, short c = 0x2588, short col = FG_YELLOW)
	{
		float fx, fy;
		WrapCoordinates(x, y, fx, fy);
		olcConsoleGameEngine::Draw(fx, fy, c, col);
	}
	 


	void DrawWireFrameModel(const vector<pair<float, float>>& vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE)
	{
		// pair.first = x coordinate
		// pair.second = y coordinate

		// Create translated model vector of coordinate pairs
		vector<pair<float, float>> vecTransformedCoordinates;
		int verts = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(verts);

		// Rotate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i < verts + 1; i++)
		{
			int j = (i + 1);
			DrawLine(vecTransformedCoordinates[i % verts].first, vecTransformedCoordinates[i % verts].second,
				vecTransformedCoordinates[j % verts].first, vecTransformedCoordinates[j % verts].second, PIXEL_SOLID, col);
		}
	}

	bool IsPointInsideCircle(float cx, float cy, float radius, float x, float y)
	{
		return sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy)) < radius;
	}
};
int main() {
	SpaceShip game;
	game.ConstructConsole(160, 100, 8, 8); // size of the gamezone

	game.Start();
	return 0;
	
}
