#include "iGraphics.h"
#include <stdlib.h>
#include <time.h>
#include<stdio.h>
#include<windows.h>
#include<mmsystem.h>
#pragma comment ( lib , "winmm.lib")

void drawHomePage();
void drawStartPage();
void easyButtonClickHandler();
void drawEasyPage();
void startButtonClickHandler();
void drawAboutPage();
void aboutButtonClickHandler();
void drawScorePage();
void scoreButtonClickHandler();
void backButtonClickHandler();
void moveCars();
void checkCollision();
void initEnemies();
void playMusic();
void stopMusic();
void drawMediumPage();
void mediumButtonClickHandler();
void drawHardPage();
void hardButtonClickHandler();
void checkSurpriseCollision();
void drawSurpriseBox();

int hardPage = 0;
int mediumPage = 0;
int startButtonClick = 0;
int startPage = 0;
int homePage = 1;
int easyPage = 0;
int aboutButtonClick = 0;
int aboutPage = 0;
int scorePage = 0;
int drawButtonClick = 0;
int mainCar, enemyCars[2];
//sheild
bool shieldActive = false;
int shieldDuration = 10;      // shield lasts for 10 seconds
int shieldStartTime = 0;      // when shield was activated
int shieldImage;              // shield image
int lastShieldScore = 0;      // to track re-activation every 100 points
//outfit change
int bikeBlue, bikePink, bikeGold;
//bonous heart
int bonusX, bonusY;
bool bonusVisible = false;
float heartPulse = 0.0;
int lastBonusTime = 0;
int bgx = 250;
int bgy = 0;
int image;
// Surprise box
int surpriseX, surpriseY;
bool surpriseVisible = false;
int lastSurpriseScore = 0;   // track the score for next box
int surpriseWidth = 50, surpriseHeight = 50;
bool firstBoxCollected = false;
bool secondBoxCollected = false;
// ::::::::::::::::: MISSION SYSTEM ::::::::::::::::: //
bool missionActive = false;
int missionType = -1;      // 0 = Collect Hearts, 1 = Avoid Cars, 2 = Survive
int missionGoal = 0;       // target (e.g., 12 hearts, 7 cars, 35 sec)
int missionProgress = 0;   // progress tracker
int missionStartTime = 0;
int missionDuration = 0;   // countdown time
int lastMissionTime = 0;   // last mission trigger
int missionStreak = 0;     // for bonus streaks
bool missionSuccessMessage = false;
bool missionFailMessage = false;
int messageTimer = 0;      // for floating message duration
char missionMsg[50];

void moveUpward(){

	bgy += 5;
	if (bgy > 800){
		bgy = 0;
	}
}



int bgY1 = 0;       // first background’s Y
int bgY2 = -600;     // second background’s Y (screen height)
int bgSpeed = 3;  // speed of scrolling
int roadImage;

void moveBackground()
{

	bgY1 += bgSpeed;
	bgY2 += bgSpeed;

	// If first image goes fully down, reset it to top
	if (bgY1 >= 600)
		bgY1 = bgY2 - 600;

	// If second image goes fully down, reset it to top
	if (bgY2 >= 600)
		bgY2 = bgY1 - 600;
}
void renderBackground()
{
	iShowImage(0, bgY1, 1000, 600, roadImage);
	iShowImage(0, bgY2, 1000, 600, roadImage);
}




extern int x;
extern int y;
// Coin variables for 4 coins
const int totalCoins = 4;
int coinX[totalCoins], coinY[totalCoins];
bool coinVisible[totalCoins];
int coinImage;

int laneY[totalCoins] = { 120, 240, 360, 480 }; // fixed lanes
int score = 0;                                // score
int coinSpeed = 4;                            // coin movement speed

// ::::::::::::::::: COIN MOVEMENT ::::::::::::::::: //

void moveCoins()
{


	for (int i = 0; i < totalCoins; i++)
	{
		coinX[i] -= coinSpeed; // move left

		// If coin goes off screen, respawn at right edge
		if (coinX[i] < -40)
		{
			coinX[i] = 1000 + rand() % 200; // respawn beyond right edge
			coinY[i] = laneY[i];
			coinVisible[i] = true;
		}
	}
}

// ::::::::::::::::: COIN COLLISION ::::::::::::::::: //
void checkCoinCollision()
{
	for (int i = 0; i < 4; i++) // check for all 4 coins
	{
		if (coinVisible[i])
		{
			// bounding box collision between bike and coin
			if (x < coinX[i] + 40 && x + 100 > coinX[i] &&
				y < coinY[i] + 40 && y + 100 > coinY[i])
			{
				score += 10;              // add score
				if (missionActive && missionType == 0) {
					missionProgress++;
				}
				
				coinVisible[i] = false;  // hide collected coin

				// respawn coin at new random lane
				int laneY[4] = { 150, 250, 350, 450 };  // 4 fixed lanes
				coinX[i] = rand() % (1000 - 40);
				coinY[i] = laneY[rand() % 4];
				coinVisible[i] = true;
			}
		}
	}
}

int elapsedTime = 0;   // in seconds
int lastScoreTime = 0;
// Speed Heart
const int totalSpeedHearts = 1;      // number of hearts on screen
int heartX[totalSpeedHearts], heartY[totalSpeedHearts];
bool heartVisible[totalSpeedHearts];
int lastHeartScore = 0;               // track when last heart spawned
bool speedActive = false;             // is speed boost active
int speedStartTime = 0;               // when speed started

int speedDuration = 3;                // duration in seconds
//villain car
int x = 470;
int y = 0;
int speed = 6;          // normal player movement speed
int normalSpeed = 6;    // store normal speed for boost
int speedIncrease = 5;   // amount to  boost
const int numLanes = 4;
const int numEnemies = 4;
int enemyX[numEnemies], enemyY[numEnemies];
int enemyType[numEnemies];
int enemySpeed[numEnemies];
int roadLeft = 100;
int roadRight = 900;
int gameOver = 0;
int lanes[4] = { 200, 350, 550, 750 };
int lastJumpTime = 0;   // to track cooldown
int jumpCooldown = 2;   // seconds before another jump allowed
// find nearest lane index based on current x
// Glow for the rectangle
bool glowToggle = false;
int glowCounter = 0;
void moveHearts() {
	for (int i = 0; i < totalSpeedHearts; i++) {
		if (heartVisible[i]) {
			heartX[i] -= coinSpeed;

			if (heartX[i] < -40) { // respawn off-screen
				heartX[i] = 1000 + rand() % 200;
				heartY[i] = laneY[rand() % 4];
				heartVisible[i] = true;
			}
		}
	}
}

void checkHeartCollision() {
	if (!(mediumPage == 1 || hardPage == 1)) return;

	for (int i = 0; i < totalSpeedHearts; i++) {
		if (heartVisible[i]) {
			if (x < heartX[i] + 40 && x + 100 > heartX[i] &&
				y < heartY[i] + 40 && y + 100 > heartY[i])
			{
				speedActive = true;
				speedStartTime = elapsedTime;
				speed = normalSpeed + speedIncrease;

				heartVisible[i] = false; // collected
			}
		}
	}

	// deactivate speed after duration
	if (speedActive && (elapsedTime - speedStartTime >= speedDuration)) {
		speedActive = false;
		speed = normalSpeed;
	}
}
int findNearestLaneIndex() {
	int best = 0;
	int bestDist = abs(x - lanes[0]);
	for (int i = 1; i < numLanes; i++) {
		int d = abs(x - lanes[i]);
		if (d < bestDist) {
			bestDist = d;
			best = i;
		}
	}
	return best;
}

// jump exactly two lanes to the RIGHT (clamped)
void jumpRightTwoLanes() {
	int idx = findNearestLaneIndex();
	int target = idx + 2;
	if (target >= numLanes) target = numLanes - 1; // clamp
	x = lanes[target];
}

// jump exactly two lanes to the LEFT (clamped)
void jumpLeftTwoLanes() {
	int idx = findNearestLaneIndex();
	int target = idx - 2;
	if (target < 0) target = 0; // clamp
	x = lanes[target];
}
// Initialize enemy cars at the bottom
void initEnemies() {
	for (int i = 0; i < numEnemies; i++) {
		enemyX[i] = lanes[i % numLanes];   // assign to lane
		enemyY[i] = 0 - (rand() % 300);   // start slightly below screen
		enemyType[i] = i % 2; // car type
		if (hardPage == 1)
			enemySpeed[i] = 8 + rand() % 7;
		else
			enemySpeed[i] = 6 + rand() % 5;   // positive speed (upwards)
	}
}
void moveCars() {
	if ((easyPage == 0 && mediumPage == 0 && hardPage == 0) || gameOver) return;

	// Move enemy cars upward
	for (int i = 0; i < numEnemies; i++) {
		enemyY[i] += enemySpeed[i];  // move UP the screen

		// If enemy reaches top, reset to bottom and increase speed
		if (enemyY[i] > 600) {
			enemyY[i] = -100;                       // reset below screen
			enemySpeed[i] += 1 + rand() % 2;  
			//mission
			if (missionActive && missionType == 1) {
				missionProgress++;
			} // increase speed randomly
			if (enemySpeed[i] > 15) enemySpeed[i] = 15; // max speed cap
		}
	}

	// Move player upward as well (loop)
	y += speed;
	if (y > 600) {
		y = -100;  // reset player position
	}

	checkCollision();  // detect collision with player
}


void checkCollision() {
	if (shieldActive) return;  // Ignore collisions when shield is active

	int pw = 60, ph = 120;
	int ew = 50, eh = 100;

	for (int i = 0; i < numEnemies; i++) {

		if ((x + pw >= enemyX[i] && x <= enemyX[i] + ew) &&
			(y + ph >= enemyY[i] && y <= enemyY[i] + eh))

		{
			gameOver = 1;
		}
	}
}
void moveSurpriseBox() {
	if (surpriseVisible) {
		surpriseX -= 3;  // speed of the box
		if (surpriseX < -50) {
			surpriseVisible = false;  // missed → disappear
		}
	}
}
void checkSurpriseCollision() {
	if (surpriseVisible) {
		if (x < surpriseX + surpriseWidth && x + 100 > surpriseX &&
			y < surpriseY + surpriseHeight && y + 100 > surpriseY)
		{
			if (!firstBoxCollected) {
				score *= 2;              // double score first box
				firstBoxCollected = true;
			}
			else if (firstBoxCollected && !secondBoxCollected) {
				// slow down all enemy cars
				for (int i = 0; i < numEnemies; i++) {
					enemySpeed[i] = enemySpeed[i] / 2;
				}
				secondBoxCollected = true;
			}

			surpriseVisible = false;
		}
	}
}
void drawSurpriseBox() {
	if (surpriseVisible) {
		iSetColor(255, 215, 0); // golden
		iFilledRectangle(surpriseX, surpriseY, surpriseWidth, surpriseHeight);
		
	}

	
}
//sheild
//mission 

void updateTime() {
	if (!gameOver) {
		elapsedTime++;

		// Score increment every 5 seconds
		if (elapsedTime - lastScoreTime >= 5) {
			score += 2;
			lastScoreTime = elapsedTime;
		}

		// ------------------ Shield logic (Easy & Medium) ------------------
		if ((easyPage == 1 || mediumPage == 1 || hardPage == 1) && shieldActive && (elapsedTime - shieldStartTime >= shieldDuration))
			shieldActive = false;

		// Activate shield every 100 points
		if ((easyPage == 1 || mediumPage == 1 || hardPage == 1) && score >= lastShieldScore + 100) {
			shieldActive = true;
			shieldStartTime = elapsedTime;
			lastShieldScore = score - (score % 100); // reset counter
		}

		// ---------------- Speed Heart logic (Medium only) ----------------
		// Spawn speed heart at score 50 or every 50 points (if you want repeated spawn)
		if ((mediumPage == 1 || hardPage == 1) && score >= lastHeartScore + 50) {
			for (int i = 0; i < totalSpeedHearts; i++) {
				if (!heartVisible[i]) {
					heartX[i] = 1000;  // spawn at right edge
					heartY[i] = laneY[rand() % 4];  // random lane
					heartVisible[i] = true;
					lastHeartScore = score; // update last spawn
				}
			}
		}

			// Deactivate speed boost after duration
			if (speedActive && (elapsedTime - speedStartTime >= speedDuration)) {
				speedActive = false;
				speed = normalSpeed;
			}
		}


	// Glow toggle (runs always)
	glowCounter++;
	if (glowCounter >= 2) {
		glowToggle = !glowToggle;
		glowCounter = 0;
	}
	//bonous heart
	// Spawn bonus heart every 60 seconds
	if (elapsedTime - lastBonusTime >= 20) {
		bonusVisible = true;
		bonusX = 1000;  // spawn at right edge
		bonusY = laneY[rand() % 4];  // random lane
		lastBonusTime = elapsedTime;
	}
	if (!gameOver && hardPage == 1) {
		if (score >= lastSurpriseScore + 300 && !secondBoxCollected) {
			surpriseVisible = true;
			surpriseX = 1000;               // spawn at right edge
			surpriseY = laneY[rand() % 4];  // random lane
			lastSurpriseScore = score - (score % 100);  // next trigger after 100 pts
		}
	}
	//mission

	// Randomly start a new mission every ~60 sec if none active
	if (!missionActive && elapsedTime - lastMissionTime >= 60) {
		missionActive = true;
		missionProgress = 0;
		missionStartTime = elapsedTime;
		lastMissionTime = elapsedTime;

		// Weighted mission selection
		int r = rand() % 10; // 0–9
		if (r < 4) {
			missionType = 0; // Collect Hearts → 40%
		}
		else if (r < 8) {
			missionType = 1; // Avoid Cars → 40%
		}
		else {
			missionType = 2; // Survive → 20%
		}

		if (missionType == 0) { // Collect Hearts
			missionGoal = 5 + rand() % 5;   // 5–10
			missionDuration = 15 + rand() % 10; // 15–25 sec
			sprintf_s(missionMsg, sizeof(missionMsg), "Mission: Collect %d Hearts!", missionGoal);
		}
		else if (missionType == 1) { // Avoid Cars
			missionGoal = 5 + rand() % 5;  // 5–10
			missionDuration = 15 + rand() % 10;
			sprintf_s(missionMsg, sizeof(missionMsg), "Mission: Avoid %d Cars!", missionGoal);
		}
		else { // Survive
			missionGoal = 20 + rand() % 20; // 20–40 sec
			missionDuration = missionGoal;  // survival = time
			sprintf_s(missionMsg, sizeof(missionMsg), "Mission: Survive %d Seconds!", missionGoal);
		}
	}

	//mission 
	if (missionActive && missionType == 2) {
		missionProgress++;
	}
	// mission 
	if (missionActive) {
		int timePassed = elapsedTime - missionStartTime;
		if (missionProgress >= missionGoal) {
			// SUCCESS
			missionActive = false;
			score += 50;
			missionStreak++;
			missionSuccessMessage = true;
			messageTimer = elapsedTime;

			if (missionStreak % 3 == 0) {
				score += 100; // streak bonus
			}
		}
		else if (timePassed >= missionDuration) {
			// FAIL
			missionActive = false;
			missionStreak = 0;
			score -= 10;
			if (shieldActive) shieldDuration -= 2;
			missionFailMessage = true;
			messageTimer = elapsedTime;
		}
	}

}
// ::::::::::::::::: FLOATING TEXT VARIABLES ::::::::::::::::: //
bool floatTextActive = false;
char floatText[20];
int floatTextX, floatTextY;
int floatTimer = 0;
int floatAlpha = 255; // opacity (simulate fade-out)

// ::::::::::::::::: MOVE BONUS HEART ::::::::::::::::: //
int bonusSpeed = 1; // new slower speed for bonus heart

void moveBonusHeart() {
	if (bonusVisible) {
		bonusX -= bonusSpeed; // slow, smooth movement
		if (bonusX < -40) {
			bonusVisible = false; // missed → disappear
		}
	}
	heartPulse += 0.05; // smoother pulse

	// Animate floating text
	if (floatTextActive) {
		floatTextY += 1;    // slower upward movement
		floatTimer++;
		floatAlpha -= 4;    // slower fade-out

		if (floatAlpha <= 0) {
			floatTextActive = false;
			floatTimer = 0;
			floatAlpha = 255;
		}
	}

}


// ::::::::::::::::: CHECK BONUS HEART COLLISION ::::::::::::::::: //
void checkBonusCollision() {
	if (bonusVisible) {
		if (x < bonusX + 40 && x + 100 > bonusX &&
			y < bonusY + 40 && y + 100 > bonusY)
		{
			if (shieldActive) {
				// extend shield instead of score
				shieldDuration += 3;
				sprintf_s(floatText, sizeof(floatText), "+3s");
			}
			else {
				// reward score if no shield active
				score += 20;
				sprintf_s(floatText, sizeof(floatText), "+20");
			}

			// activate floating text
			floatTextActive = true;
			floatTextX = x + 40;   // center above bike
			floatTextY = y + 100;
			floatAlpha = 255;      // reset opacity

			bonusVisible = false; // collected → hide
		}
	}
}

// ::::::::::::::::: DRAW BONUS HEART ::::::::::::::::: //

void drawBonusHeart() {
	if (bonusVisible) {
		// Pulse effect (size varies between 8 and 14)
		int baseSize = 10;
		int pulseSize = baseSize + (int)(4 * sin(heartPulse));

		iSetColor(255, 0, 0);
		iFilledCircle(bonusX + 10, bonusY + 20, pulseSize);
		iFilledCircle(bonusX + 30, bonusY + 20, pulseSize);

		double triX[3] = { (double)bonusX, (double)(bonusX + 40), (double)(bonusX + 20) };
		double triY[3] = { (double)(bonusY + 20), (double)(bonusY + 20), (double)bonusY };
		iFilledPolygon(triX, triY, 3);
	}

	// Floating text
	if (floatTextActive) {
		// simulate fade: reduce RGB by alpha
		int r = (255 * floatAlpha) / 255;
		int g = (215 * floatAlpha) / 255;
		int b = (0 * floatAlpha) / 255;
		iSetColor(r, g, b);
		sprintf_s(floatText, sizeof(floatText), "%s", floatText); // safe version
		iText(floatTextX, floatTextY, floatText, GLUT_BITMAP_HELVETICA_18);
	}
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::Idraw Here::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://

void iDraw()
{
	iClear();
	if (homePage == 1)
	{
		drawHomePage();
	}
	else if (startPage == 1)
	{
		drawStartPage();
	}
	else if (easyPage == 1)
	{
		drawEasyPage();
	}
	else if (mediumPage == 1)
	{
		drawMediumPage();
	}
	else if (hardPage == 1)
	{
		drawHardPage();
	}
	else if (aboutPage == 1)
	{
		drawAboutPage();
	}
	else if (scorePage == 1)
	{
		drawScorePage();
	}


}







/*function iMouseMove() is called when the user presses and drags the mouse.
(mx, my) is the position where the mouse pointer is.
*/


void iMouseMove(int mx, int my)
{

}
//ipassiveMouse//
void iPassiveMouseMove(int mx, int my)
{

}

void iMouse(int button, int state, int mx, int my)
{

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{

		printf("x=%d  y=%d", mx, my);
		if (homePage == 1 && (mx >= 333 && mx <= 531) && (my >= 455 && my <= 509))
		{
			startButtonClickHandler();
		}
		else if (startPage == 1 && (mx >= 333 && mx <= 531) && (my >= 455 && my <= 509))
		{
			easyButtonClickHandler();
		}
		else if (startPage == 1 && (mx >= 322 && mx <= 520) && (my >= 373 && my <= 412))
		{
			mediumButtonClickHandler();
		}
		else if (startPage == 1 && (mx >= 306 && mx <= 509) && (my >= 250 && my <= 315))
		{
			hardButtonClickHandler();
		}
		else if (homePage == 1 && (mx >= 493 && mx <= 746) && (my >= 347 && my <= 408))
		{
			aboutButtonClickHandler();
		}

		else if (homePage == 1 && (mx >= 228 && mx <= 438) && (my >= 259 && my <= 301))
		{
			scoreButtonClickHandler();
		}
		else if ((startPage == 1 || aboutPage == 1 || scorePage == 1 || easyPage == 1 || mediumPage == 1 || hardPage == 1)
			&& (mx >= 82 && mx <= 242) && (my >= 515 && my <= 577))
		{
			backButtonClickHandler();
		}


	}


	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{

	}
}

/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/


void iKeyboard(unsigned char key)
{

	if (gameOver && (easyPage || mediumPage || hardPage) && (key == 'r' || key == 'R'))
	{
		gameOver = 0; x = 470; y = 0;
		homePage = 0;
		startPage = 1;
		aboutPage = 0;
		scorePage = 0;
		easyPage = 0;
		mediumPage = 0;
		hardPage = 0;
		elapsedTime = 0;      // reset time
		lastScoreTime = 0;    // reset scoring
		speed = normalSpeed;  // reset speed
		shieldActive = false; // reset shield
		speedActive = false;  // reset boost
		return;               // done
	}

	// Normal movement keys
	if ((easyPage == 0 && mediumPage == 0 && hardPage == 0) || gameOver) return;

	if (key == 'd') { x += speed; if (x > roadRight - 60) x = roadRight - 60; }
	if (key == 'a') { x -= speed; if (x < roadLeft) x = roadLeft; }
	// Forward
	if (key == 'w' || key == 'W') {
		y += 30;   // move car upward
		if (y > 700) y = 700; // keep inside window
	}

	// Backward
	if (key == 's' || key == 'S') {
		y -= 30;   // move car downward
		if (y < 0) y = 0; // keep inside window
	}



	if ((mediumPage == 1 || hardPage == 1) && key == ' ') {
		if (elapsedTime - lastJumpTime >= jumpCooldown) {
			int mod = glutGetModifiers();
			if (mod & GLUT_ACTIVE_SHIFT) jumpLeftTwoLanes();
			else jumpRightTwoLanes();
			lastJumpTime = elapsedTime;
		}
	}
}




/*
function iSpecialKeyboard() is called whenver user hits special keys like-
function keys, home, end, pg up, pg down, arraows etc. you have to use
appropriate constants to detect them. A list is:
GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12,
GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
*/
void iSpecialKeyboard(unsigned char key)
{



	if (!gameOver) {
		if (key == GLUT_KEY_LEFT) { x -= speed; if (x < roadLeft) x = roadLeft; }
		if (key == GLUT_KEY_RIGHT){ x += speed; if (x > roadRight - 60) x = roadRight - 60; }
		if (key == GLUT_KEY_UP) {
			y += 30;
			if (y > 700) y = 700;
		}

		if (key == GLUT_KEY_DOWN) {
			y -= 30;
			if (y < 0) y = 0;
		}



	}

}

void drawHomePage()
{
	iSetColor(128, 128, 128);
	iFilledRectangle(0, 0, 1000, 600);
	iShowBMP2(0, 0, "image\\main.bmp", 0);
	iShowBMP2(250, 300, "image\\buttons1.bmp", 0);
	iShowBMP2(400, 200, "image\\buttons2.bmp", 0);

	iShowBMP2(150, 100, "image\\buttons1.bmp", 0);



	iSetColor(255, 275, 255);
	iText(400, 475, "START", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(500, 375, "ABOUT US", GLUT_BITMAP_TIMES_ROMAN_24);

	iText(300, 275, "SCORE", GLUT_BITMAP_TIMES_ROMAN_24);

}

void drawStartPage()
{

	iSetColor(0, 250, 0);
	iFilledRectangle(0, 0, 1000, 600);
	iShowBMP2(0, 0, "image\\main2.bmp", 0);
	iShowBMP2(250, 300, "image\\buttons1.bmp", 0);
	iSetColor(255, 255, 255);
	iText(400, 475, "EASY", GLUT_BITMAP_TIMES_ROMAN_24);
	iShowBMP2(250, 200, "image\\buttons1.bmp", 0);
	iSetColor(255, 255, 255);
	iText(375, 375, "MEDIUM", GLUT_BITMAP_TIMES_ROMAN_24);
	iShowBMP2(10, 450, "image\\back.bmp", 0);
	iShowBMP2(250, 100, "image\\buttons1.bmp", 0);
	iSetColor(255, 255, 255);
	iText(400, 275, "HARD", GLUT_BITMAP_TIMES_ROMAN_24);


}

void startButtonClickHandler()
{

	homePage = 0;
	startPage = 1;
}

void easyButtonClickHandler() {
	startPage = 0;
	easyPage = 1;
	homePage = 0;

	// Reset player position
	x = 470;
	y = 0;

	// Reset score
	score = 0;

	// Reset enemies
	initEnemies();

	// ----- Reset Shield -----
	shieldActive = false;
	shieldStartTime = 0;
	lastShieldScore = 0;

	// Reset speed (normal speed for Easy)
	speed = normalSpeed;

	// Reset elapsed time
	elapsedTime = 0;
	lastScoreTime = 0;
}


void mediumButtonClickHandler() {
	startPage = 0;
	mediumPage = 1;
	homePage = 0;

	// Reset player position
	x = 470;
	y = 0;

	// Reset score
	score = 0;

	// Reset enemies
	initEnemies();

	// ----- Reset Shield -----
	shieldActive = false;
	shieldStartTime = 0;
	lastShieldScore = 0;

	// ----- Reset Speed Heart -----
	for (int i = 0; i < totalSpeedHearts; i++) {
		heartX[i] = 1000;              // off-screen
		heartY[i] = laneY[rand() % 4]; // random lane
		heartVisible[i] = false;       // not visible initially
	}

	// Reset speed boost
	speedActive = false;
	speed = normalSpeed;

	// Reset elapsed time if you want fresh timing for Medium
	elapsedTime = 0;
	lastScoreTime = 0;
	lastHeartScore = 0;
}
void hardButtonClickHandler() {
	startPage = 0;
	hardPage = 1;
	homePage = 0;

	// Reset player position
	x = 470;
	y = 0;

	// Reset score
	score = 0;

	// Reset enemies
	initEnemies();

	// ----- Reset Shield -----
	shieldActive = false;
	shieldStartTime = 0;
	lastShieldScore = 0;

	// ----- Reset Speed Heart -----
	for (int i = 0; i < totalSpeedHearts; i++) {
		heartX[i] = 1000;              // off-screen
		heartY[i] = laneY[rand() % 4]; // random lane
		heartVisible[i] = false;       // not visible initially
	}

	// Reset speed boost
	speedActive = false;
	speed = normalSpeed;

	// Reset elapsed time if you want fresh timing for Medium
	elapsedTime = 0;
	lastScoreTime = 0;
	lastHeartScore = 0;
}
void drawEasyPage()
{
	renderBackground();

	iShowBMP2(10, 450, "image\\back.bmp", 0);
	iShowImage(x, y, 100, 100, image);

	// Draw Coins
	for (int i = 0; i < totalCoins; i++)
	{
		if (coinVisible[i])
		{
			iShowImage(coinX[i], coinY[i], 40, 40, coinImage);
		}
	}

	// Show Score
	char scoreStr[20];
	sprintf_s(scoreStr, sizeof(scoreStr), "Score: %d", score);
	iSetColor(0, 0, 0);
	iText(850, 550, scoreStr, GLUT_BITMAP_HELVETICA_18);

	// Show Time
	char timeStr[30];
	sprintf_s(timeStr, sizeof(timeStr), "Time: %d s", elapsedTime);
	iText(850, 520, timeStr, GLUT_BITMAP_HELVETICA_18);

	checkCoinCollision();      // coins collision
	checkHeartCollision();     // nothing will happen since hearts are Medium only

	// Draw enemy cars
	for (int i = 0; i < numEnemies; i++) {
		int img = (enemyType[i] == 0) ? enemyCars[0] : enemyCars[1];
		iShowImage(enemyX[i], enemyY[i], 50, 100, img);
	}

	checkCollision();
	if (gameOver) {
		iSetColor(255, 0, 0);
		iFilledRectangle(0, 250, 1000, 100);
		iSetColor(255, 255, 255);
		iText(400, 280, "GAME OVER!Press R to Restart", GLUT_BITMAP_HELVETICA_18);
	}

	// ------------------- Shield visual (Easy & Medium) -------------------
	if (shieldActive) {
		for (int r = 70; r <= 80; r += 3) {
			iSetColor(0, 200, 255);
			iCircle(x + 50, y + 50, r);
		}
		iSetColor(0, 200, 255);
		iCircle(x + 50, y + 50, 70);

		// Show shield countdown
		int remaining = shieldDuration - (elapsedTime - shieldStartTime);
		char shieldStr[30];
		sprintf_s(shieldStr, sizeof(shieldStr), "Shield: %d s", remaining);
		iSetColor(0, 0, 255);
		iText(850, 490, shieldStr, GLUT_BITMAP_HELVETICA_18);
	}
}

void drawMediumPage()
{
	renderBackground();

	iShowBMP2(10, 450, "image\\back.bmp", 0);
	iShowImage(x, y, 100, 100, image);

	// Draw Coins
	for (int i = 0; i < totalCoins; i++) {
		if (coinVisible[i])
			iShowImage(coinX[i], coinY[i], 40, 40, coinImage);
	}

	// Draw Speed Hearts
	for (int i = 0; i < totalSpeedHearts; i++) {
		if (heartVisible[i]) {
			if (glowToggle) iSetColor(0, 255, 255); // glow cyan
			else iSetColor(255, 0, 0);              // normal red

			iRectangle(heartX[i] - 5, heartY[i] - 5, 50, 50);
			iShowImage(heartX[i], heartY[i], 40, 40, coinImage);
		}
	}


	// Draw Enemy Cars
	for (int i = 0; i < numEnemies; i++) {
		int img = (enemyType[i] == 0) ? enemyCars[0] : enemyCars[1];
		iShowImage(enemyX[i], enemyY[i], 50, 100, img);
	}

	checkCoinCollision();
	checkHeartCollision();
	checkCollision();

	// Draw Score & Time
	char scoreStr[20], timeStr[30];
	sprintf_s(scoreStr, sizeof(scoreStr), "Score: %d", score);
	sprintf_s(timeStr, sizeof(timeStr), "Time: %d s", elapsedTime);
	iSetColor(0, 0, 0);
	iText(850, 550, scoreStr, GLUT_BITMAP_HELVETICA_18);
	iText(850, 520, timeStr, GLUT_BITMAP_HELVETICA_18);

	// Game Over
	if (gameOver) {
		iSetColor(255, 0, 0);
		iFilledRectangle(0, 250, 1000, 100);
		iSetColor(255, 255, 255);
		iText(400, 280, "GAME OVER! Press R to Restart", GLUT_BITMAP_HELVETICA_18);
	}

	// Shield visual
	if (shieldActive) {
		for (int r = 70; r <= 80; r += 3) {
			iSetColor(0, 200, 255);
			iCircle(x + 50, y + 50, r);
		}
		iSetColor(0, 200, 255);
		iCircle(x + 50, y + 50, 70);

		int remaining = shieldDuration - (elapsedTime - shieldStartTime);
		char shieldStr[30];
		sprintf_s(shieldStr, sizeof(shieldStr), "Shield: %d s", remaining);
		iSetColor(0, 0, 255);
		iText(850, 490, shieldStr, GLUT_BITMAP_HELVETICA_18);
	}
	int currentBike;
	static int lastOutfit = -1;          // track last unlocked outfit
	static int outfitChangeTimer = -100; // timer for particle effect
	static char unlockMsg[50];           // message to display

	// Outfit selection by score
	int outfitNow;
	if (score <= 20) {
		currentBike = image;
		outfitNow = 0;
	}
	else if (score <= 50) {
		currentBike = bikeBlue;
		outfitNow = 1;
	}
	else if (score <= 100) {
		currentBike = bikePink;
		outfitNow = 2;
	}
	else {
		currentBike = bikeGold;
		outfitNow = 3;
	}

	// Detect outfit change (only trigger effect for new unlocks, not red)
	if (outfitNow != lastOutfit && outfitNow > 0) {
		outfitChangeTimer = elapsedTime;  // start effect
		lastOutfit = outfitNow;

		// Set custom unlock message
		if (outfitNow == 1) strcpy_s(unlockMsg, sizeof(unlockMsg), "Blue Bike Unlocked!");
		else if (outfitNow == 2) strcpy_s(unlockMsg, sizeof(unlockMsg), "Pink Bike Unlocked!");
		else if (outfitNow == 3) strcpy_s(unlockMsg, sizeof(unlockMsg), "Gold Bike Unlocked!");


	}

	// Draw current bike
	iShowImage(x, y, 100, 100, currentBike);

	// Show particles & unlock message for 3 seconds after unlock
	if (elapsedTime - outfitChangeTimer < 3) {
		for (int i = 0; i < 25; i++) {
			int px = x + rand() % 100; // random around bike
			int py = y + rand() % 100;
			iSetColor(rand() % 255, rand() % 255, rand() % 255);
			iFilledCircle(px, py, 3);
		}

		iSetColor(255, 255, 0);
		iText(400, 550, unlockMsg, GLUT_BITMAP_HELVETICA_18);
	}
	moveBonusHeart();
	checkBonusCollision();
	drawBonusHeart();

}
void drawHardPage()
{
	renderBackground();

	iShowBMP2(10, 450, "image\\back.bmp", 0);
	iShowImage(x, y, 100, 100, image);

	// Draw Coins
	for (int i = 0; i < totalCoins; i++) {
		if (coinVisible[i])
			iShowImage(coinX[i], coinY[i], 40, 40, coinImage);
	}

	// Draw Speed Hearts
	for (int i = 0; i < totalSpeedHearts; i++) {
		if (heartVisible[i]) {
			if (glowToggle) iSetColor(0, 255, 255); // glow cyan
			else iSetColor(255, 0, 0);              // normal red

			iRectangle(heartX[i] - 5, heartY[i] - 5, 50, 50);
			iShowImage(heartX[i], heartY[i], 40, 40, coinImage);
		}
	}


	// Draw Enemy Cars
	for (int i = 0; i < numEnemies; i++) {
		int img = (enemyType[i] == 0) ? enemyCars[0] : enemyCars[1];
		iShowImage(enemyX[i], enemyY[i], 50, 100, img);
	}

	checkCoinCollision();
	checkHeartCollision();
	checkCollision();

	// Draw Score & Time
	char scoreStr[20], timeStr[30];
	sprintf_s(scoreStr, sizeof(scoreStr), "Score: %d", score);
	sprintf_s(timeStr, sizeof(timeStr), "Time: %d s", elapsedTime);
	iSetColor(0, 0, 0);
	iText(850, 550, scoreStr, GLUT_BITMAP_HELVETICA_18);
	iText(850, 520, timeStr, GLUT_BITMAP_HELVETICA_18);

	// Game Over
	if (gameOver) {
		iSetColor(255, 0, 0);
		iFilledRectangle(0, 250, 1000, 100);
		iSetColor(255, 255, 255);
		iText(400, 280, "GAME OVER! Press R to Restart", GLUT_BITMAP_HELVETICA_18);
		char finalScore[50];
		sprintf_s(finalScore, sizeof(finalScore), "Your Score is: %d", score);
		iText(450, 320, finalScore, GLUT_BITMAP_HELVETICA_18);
	}

	// Shield visual
	if (shieldActive) {
		for (int r = 70; r <= 80; r += 3) {
			iSetColor(0, 200, 255);
			iCircle(x + 50, y + 50, r);
		}
		iSetColor(0, 200, 255);
		iCircle(x + 50, y + 50, 70);

		int remaining = shieldDuration - (elapsedTime - shieldStartTime);
		char shieldStr[30];
		sprintf_s(shieldStr, sizeof(shieldStr), "Shield: %d s", remaining);
		iSetColor(0, 0, 255);
		iText(850, 490, shieldStr, GLUT_BITMAP_HELVETICA_18);
	}
	int currentBike;
	static int lastOutfit = -1;          // track last unlocked outfit
	static int outfitChangeTimer = -100; // timer for particle effect
	static char unlockMsg[50];           // message to display

	// Outfit selection by score
	int outfitNow;
	if (score <= 20) {
		currentBike = image;
		outfitNow = 0;
	}
	else if (score <= 50) {
		currentBike = bikeBlue;
		outfitNow = 1;
	}
	else if (score <= 100) {
		currentBike = bikePink;
		outfitNow = 2;
	}
	else {
		currentBike = bikeGold;
		outfitNow = 3;
	}

	// Detect outfit change (only trigger effect for new unlocks, not red)
	if (outfitNow != lastOutfit && outfitNow > 0) {
		outfitChangeTimer = elapsedTime;  // start effect
		lastOutfit = outfitNow;

		// Set custom unlock message
		if (outfitNow == 1) strcpy_s(unlockMsg, sizeof(unlockMsg), "Blue Bike Unlocked!");
		else if (outfitNow == 2) strcpy_s(unlockMsg, sizeof(unlockMsg), "Pink Bike Unlocked!");
		else if (outfitNow == 3) strcpy_s(unlockMsg, sizeof(unlockMsg), "Gold Bike Unlocked!");


	}

	// Draw current bike
	iShowImage(x, y, 100, 100, currentBike);

	// Show particles & unlock message for 3 seconds after unlock
	if (elapsedTime - outfitChangeTimer < 3) {
		for (int i = 0; i < 25; i++) {
			int px = x + rand() % 100; // random around bike
			int py = y + rand() % 100;
			iSetColor(rand() % 255, rand() % 255, rand() % 255);
			iFilledCircle(px, py, 3);
		}

		iSetColor(255, 255, 0);
		iText(400, 550, unlockMsg, GLUT_BITMAP_HELVETICA_18);
	}
	//mission
	// ==================== MISSION DISPLAY ====================
	if (missionActive) {
		int rightX = 700;       // right side position
		int baseY = 460;        // start below score/time

		// Mission description
		iSetColor(255, 255, 0); // Yellow
		iText(rightX, baseY, missionMsg, GLUT_BITMAP_HELVETICA_18);

		// Mission progress
		char prog[50];
		sprintf_s(prog, sizeof(prog), "Progress: %d / %d", missionProgress, missionGoal);
		iSetColor(0, 255, 0);   // Green
		iText(rightX, baseY - 25, prog, GLUT_BITMAP_HELVETICA_18);
		
		//int a;

		// Mission time left
		int timeLeft = missionDuration - (elapsedTime - missionStartTime);
		if (timeLeft < 0) timeLeft = 0;
		char timerStr[30];
		sprintf_s(timerStr, sizeof(timerStr), "Time Left: %d s", timeLeft);
		iSetColor(255, 0, 0);   // Red
		iText(rightX, baseY - 50, timerStr, GLUT_BITMAP_HELVETICA_18);
	}

	// Floating messages for mission success/failure
	if (missionSuccessMessage && elapsedTime - messageTimer < 3) {
		iSetColor(0, 255, 0);
		iText(400, 300, "Mission Complete! +50", GLUT_BITMAP_HELVETICA_18);
	}
	else if (missionFailMessage && elapsedTime - messageTimer < 3) {
		iSetColor(255, 0, 0);
		iText(400, 300, "Mission Failed! -10", GLUT_BITMAP_HELVETICA_18);
	}
	
	moveBonusHeart();
	checkBonusCollision();
	drawBonusHeart();
	checkCollision();
	checkSurpriseCollision();
	drawSurpriseBox();
}

void drawAboutPage()
{
	iSetColor(0, 0, 0);
	iFilledRectangle(0, 0, 1000, 600);
	iShowBMP2(100, 150, "image\\susmita.bmp", 0);
	iSetColor(255, 275, 255);
	iText(100, 130, "NAME:SUSMITA RANI MOHONTHO ", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(100, 100, "Roll:00724105101035 ", GLUT_BITMAP_TIMES_ROMAN_24);
	iShowBMP2(600, 150, "image\\roza.bmp", 0);
	iText(600, 130, "NAME:OAKKEYA AMINA ROZA", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(600, 100, "Roll:00724105101046 ", GLUT_BITMAP_TIMES_ROMAN_24);
	iShowBMP2(10, 450, "image\\back.bmp", 0);
}

void aboutButtonClickHandler()
{
	homePage = 0;
	startPage = 0;
	aboutPage = 1;
}

void scoreButtonClickHandler()
{

	homePage = 0;
	startPage = 0;
	aboutPage = 0;
	scorePage = 1;
}

void drawScorePage()
{
	iSetColor(0, 250, 0);
	iFilledRectangle(0, 0, 1000, 600);
	iShowBMP2(0, 0, "image\\main.bmp", 0);
	iShowBMP2(10, 450, "image\\back.bmp", 0);
}
void backButtonClickHandler()
{
	homePage = 1;
	startPage = 0;
	aboutPage = 0;
	scorePage = 0;
	easyPage = 0;
	mediumPage = 0;
	hardPage = 0;
}
void playMusic() {
	mciSendString("open \"music.mp3\" type mpegvideo alias bgm", NULL, 0, NULL);
	mciSendString("play bgm repeat", NULL, 0, NULL);
}

void stopMusic() {
	mciSendString("stop bgm", NULL, 0, NULL);
	mciSendString("close bgm", NULL, 0, NULL);
}


int main()

{
	srand((unsigned)time(NULL));

	playMusic();
	iInitialize(1000, 600, "Project Title");

	// load player & background
	image = iLoadImage("b.png");
	roadImage = iLoadImage("image\\road2.bmp");
	coinImage = iLoadImage("image\\heart_coin.png");
	enemyCars[0] = iLoadImage("carvi.png");
	enemyCars[1] = iLoadImage("carvi1.png");


	for (int i = 0; i < totalCoins; i++)
	{
		coinX[i] = 1000 + (i * 200); // space them out
		coinY[i] = laneY[i];
		coinVisible[i] = true;
	}
	normalSpeed = speed;  // store player's normal speed

	// Initialize speed hearts
	for (int i = 0; i < totalSpeedHearts; i++) {
		heartX[i] = 1000;              // start off-screen
		heartY[i] = laneY[rand() % 4];
		heartVisible[i] = false;       // start invisible
	}

	// Initialize shield
	shieldActive = false;
	lastShieldScore = 0;
	// new
	bikeBlue = iLoadImage("image\\bike_blue.png");
	bikePink = iLoadImage("image\\bike_pink.png");
	bikeGold = iLoadImage("image\\bike_gold.png");

	// timers

	iSetTimer(20, moveBackground);  // for smooth background
	iSetTimer(50, moveUpward);
	iSetTimer(30, moveCoins);
	iSetTimer(1000, updateTime);  // increase elapsedTime every 1 second
	initEnemies();
	iSetTimer(50, moveCars);
	iSetTimer(30, moveHearts);
	iSetTimer(30, moveSurpriseBox);


	///updated see the documentations
	iStart();
	stopMusic();
	return 0;



}