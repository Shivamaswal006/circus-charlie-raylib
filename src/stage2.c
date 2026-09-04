// --- START OF FILE stage2.c ---
#include "raylib.h"

extern int highScore; // Pulling the global high score from main.c
extern int currentScore; // Pulling currentScore global variable from main.c

int RunStage2(int mode) {
    // --- LOAD TEXTURES ---
    Texture2D bg = LoadTexture("../images/stage2_bg.png");
    Texture2D cWalk1 = LoadTexture("../images/clown_walk1.png");
    Texture2D cWalk2 = LoadTexture("../images/clown_walk2.png");
    Texture2D cWalk3 = LoadTexture("../images/clown_walk3.png");
    Texture2D cJump = LoadTexture("../images/clown_jump.png");
    Texture2D cHurt = LoadTexture("../images/clown_hurt.png");
    Texture2D cWin1 = LoadTexture("../images/clown_win1.png");
    Texture2D cWin2 = LoadTexture("../images/clown_win2.png");
    
    Texture2D br1 = LoadTexture("../images/monkey_br1.png");
    Texture2D br2 = LoadTexture("../images/monkey_br2.png");
    Texture2D br3 = LoadTexture("../images/monkey_br3.png");
    Texture2D bl1 = LoadTexture("../images/monkey_bl1.png");
    Texture2D blJump = LoadTexture("../images/monkey_bl_jump.png");
    Texture2D podium2 = LoadTexture("../images/podium2.png");

    // --- PLAYER VARIABLES ---
    float playerX = 40.0f;
    float playerY = 135.0f; // Clown sits higher up to perfectly align his feet with the tightrope
    float speedY = 0.0f;
    bool isJumping = false;
    
    // --- GAME STATE VARIABLES ---
    float bgScrollX = 0.0f;
    int state = 0; // 0 = Playing, 1 = Hurt, 2 = Win
    int endTimer = 0;
    int animTimer = 0, frame = 0;
    
    float distance = 100.0f; // Distance to the podium
    // --- BONUS VARIABLES ---
    int bonus = 5000;      // Starts at 5000
    int bonusTimer = 0;    // A clock to measure 1 second (60 frames)

    // --- MONKEY SETUP (Object Pool) ---
    // We use arrays of size [5] to keep up to 5 monkeys on screen at once for denser gameplay.
    float mX[5] = { 426.0f, 550.0f, 700.0f, 850.0f, 1000.0f }; // Starting X positions
    float mY[5] = { 142.0f, 142.0f, 142.0f, 142.0f, 142.0f }; // Monkeys sit slightly lower on the rope (142)
    int mType[5] = { 0, 0, 0, 0, 0 }; // 0 = Brown Monkey, 1 = Blue Monkey
    float mSpeedY[5] = { 0 }; // Blue monkeys need their own gravity/jump speed!
    bool monkeyPassed[5] = { false, false, false, false, false }; // To ensure we only score once per monkey
    
    // --- PODIUM SETUP ---
    float podX = 500.0f;
    bool podSpawned = false;

    PlayMusicStream(audio.stage2);

    while (!WindowShouldClose()) {
        UpdateMusicStream(audio.stage2);

        if (state == 0) { // IF PLAYING
            
            // 1. Update High Score dynamically
            if (currentScore > highScore) highScore = currentScore;
            
            // --- BONUS COUNTDOWN LOGIC ---
            if (mode == 0) { // Only count down in Normal Mode
                bonusTimer++;
                if (bonusTimer >= 60) { // 60 frames = 1 exact second
                    bonus -= 10;        // Lose 10 bonus points
                    if (bonus < 0) bonus = 0; // Never let it drop below 0
                    bonusTimer = 0;     // Reset the clock for the next second
                }
            }

            // 2. Handle Player X Movement
            float moveX = 0.0f;
            if (IsKeyDown(KEY_RIGHT)) moveX = isJumping ? 2.0f : 1.5f;
            else if (IsKeyDown(KEY_LEFT)) moveX = isJumping ? -2.0f : -1.5f;
            playerX += moveX;

            // 3. Camera/Scrolling Logic
            float camShift = 0.0f;
            if (playerX > 180.0f) {
                camShift = playerX - 180.0f;
                playerX = 180.0f;
            }
            if (playerX < 10.0f) playerX = 10.0f;

            // 4. Handle Player Jump & Gravity
            if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)) && !isJumping) {
                speedY = -3.8f; // Clown's jump velocity
                isJumping = true;
                PlaySound(audio.jump);
            }
            
            if (isJumping) {
                playerY += speedY;
                speedY += 0.18f; // Gravity
                
                // Land back on the tightrope
                if (playerY >= 135.0f) { 
                    playerY = 135.0f; 
                    speedY = 0.0f; 
                    isJumping = false; 
                }
            }

            // 5. Scroll Background
            bgScrollX -= (camShift * 0.5f);
            if (bgScrollX <= -256.0f) bgScrollX += 256.0f;

            // 6. Decrease Distance
            if (mode == 0 && distance > 0) distance -= (camShift * 0.02f);

            // 7. Process Monkeys (THE AI)
            for (int i = 0; i < 5; i++) {
                // Brown monkeys (0.7f) are slow. Blue monkeys (2.2f) are fast.
                float mVel = (mType[i] == 1) ? 2.2f : 0.7f; 
                mX[i] -= (mVel + camShift);

                // Respawn monkey if it goes off the left side of the screen
                if (mX[i] < -20.0f) {
                    if (!podSpawned) {
                        float maxX = 426.0f;
                        for (int j = 0; j < 5; j++) { if (mX[j] > maxX) maxX = mX[j]; }
                        
                        mX[i] = maxX + GetRandomValue(150, 220); // Maintain a safe gap between monkeys
                        
                        // 1 in 6 chance for this recycled monkey to become a Blue Monkey
                        mType[i] = (GetRandomValue(1, 6) == 1) ? 1 : 0; 
                        monkeyPassed[i] = false;
                    } else {
                        mX[i] = -100.0f; // Hide offscreen if podium is spawning
                    }
                }

                // --- BLUE MONKEY JUMP AI ---
                if (mType[i] == 1) { // Only Blue Monkeys can jump
                    // Check every OTHER monkey in the array
                    for (int j = 0; j < 5; j++) {
                        if (i != j && mType[j] == 0) { // If it's a different monkey, AND it's brown
                            // If the blue monkey gets close behind a brown monkey (less than 40px), JUMP!
                            if (mX[i] > mX[j] && (mX[i] - mX[j]) < 40.0f && mY[i] == 142.0f) {
                                mSpeedY[i] = -3.5f; // Blue monkey jump velocity
                            }
                        }
                    }
                    
                    // Apply gravity to the blue monkey if it is in the air
                    if (mY[i] < 142.0f || mSpeedY[i] < 0) {
                        mY[i] += mSpeedY[i];
                        mSpeedY[i] += 0.2f; // Gravity pulls blue monkey down
                        
                        if (mY[i] >= 142.0f) { // Blue monkey lands back on rope
                            mY[i] = 142.0f; 
                            mSpeedY[i] = 0.0f; 
                        } 
                    }
                }

                // --- MONKEY SCORING ---
                // You get points if you jump over ANY monkey (Brown = 100, Blue = 500)
                if (!monkeyPassed[i] && playerX > mX[i] + 10 && isJumping) {
                    monkeyPassed[i] = true;
                    currentScore += (mType[i] == 1) ? 500 : 100;
                    if(mType[i] == 1)
                        PlaySound(audio.score);
                }

                // --- HITBOX LOGIC ---
                Rectangle pRect = { playerX + 5, playerY + 5, 15, 25 }; // Clown's body
                Rectangle mRect = { mX[i] + 5, mY[i] + 5, 15, 15 };     // Monkey's body
                
                if (CheckCollisionRecs(pRect, mRect)) {
                    state = 1; // Touched a monkey! Dead!
                    PlaySound(audio.hurt); 
                    StopMusicStream(audio.stage2);
                }
            }

            // 8. Podium Safe Spawn & Snap Logic
            if (distance <= 0 && mode == 0) {
                if (!podSpawned) { 
                    // Find the absolute furthest monkey so we don't spawn the podium on top of him
                    float furthestX = 426.0f;
                    for (int j = 0; j < 5; j++) { if (mX[j] > furthestX) furthestX = mX[j]; }
                    
                    // Spawn podium 250 pixels behind the last monkey
                    podX = furthestX + 250.0f; 
                    podSpawned = true; 
                }
                
                podX -= camShift;
                
                // Instantly snap the clown to the podium when aligned
                if (playerX + 10 > podX) {
                    state = 2; // WON
                    playerX = podX + 5;
                    playerY = 105.0f; // Sits perfectly on top of podium
                    isJumping = false;
                    PlaySound(audio.win);
                    StopMusicStream(audio.stage2);
                }
            }

            // 9. Animation Timer
            if (moveX != 0.0f) {
                animTimer++;
                if (animTimer > 6) { // Monkeys animate slightly faster than lions
                    frame++; 
                    if (frame > 2) frame = 0; // Cycles through frame 0, 1, and 2
                    animTimer = 0; 
                }
            }
            
        } else {
            // IF DEAD OR WON
            // --- BONUS TALLY ANIMATION ---
            // If we WON (state == 2), are in Normal mode, and still have bonus points left:
            if (state == 2 && mode == 0 && bonus > 0) {
                // Drain 100 points from the bonus per frame (super fast animation)
                int drain = (bonus >= 100) ? 100 : bonus; 
                bonus -= drain;
                currentScore += drain;
                
                // Play a ticking sound every few frames to sound like an arcade counter
                if (bonus % 300 == 0) PlaySound(audio.score);
             }
             else {
                // Only start the 3-second exit timer AFTER the bonus hits 0 (or if player died)
            endTimer++;
            if (endTimer > 180) { // Wait 3 seconds
                if (currentScore > highScore) highScore = currentScore;
                if (state == 1) return -1;
                if (state == 2) return currentScore;
                }
            }
        }

        // --- DRAWING THE SCREEN ---
        BeginNESDrawing();
        
        // Draw 16:9 Background
        DrawTexture(bg, (int)bgScrollX, 30, WHITE);
        DrawTexture(bg, (int)bgScrollX + 256, 30, WHITE);
        DrawTexture(bg, (int)bgScrollX + 512, 30, WHITE);

        // Draw Monkeys
        for (int i = 0; i < 5; i++) {
            if (mType[i] == 0) {
                // Draw Brown Monkey (uses 3 walking frames)
                Texture2D mTex = (frame == 0) ? br1 : (frame == 1) ? br2 : br3;
                DrawTexture(mTex, (int)mX[i], (int)mY[i], WHITE);
            } else {
                // Draw Blue Monkey
                if (mY[i] < 142.0f) {
                    // If Y is less than 142 (ground), he is in the air! Draw jumping sprite.
                    DrawTexture(blJump, (int)mX[i], (int)mY[i], WHITE);
                } else {
                    // Otherwise, draw the standing/walking blue monkey
                    DrawTexture(bl1, (int)mX[i], (int)mY[i], WHITE);
                }
            }
        }

        // Draw Podium
        if (podSpawned) DrawTexture(podium2, (int)podX, 120, WHITE);

        // Draw Player Based on State
        if (state == 1) {
            // Dead / Fell
            DrawTexture(cHurt, (int)playerX, (int)playerY, WHITE);
        } else if (state == 2) {
            // Victory Pose
            Texture2D wTex = (frame % 2 == 0) ? cWin1 : cWin2;
            DrawTexture(wTex, (int)playerX, (int)playerY, WHITE);
        } else {
            // Playing
            if (isJumping) {
                DrawTexture(cJump, (int)playerX, (int)playerY, WHITE);
            } else {
                // Clown uses 3 walking frames
                Texture2D wTex = (frame == 0) ? cWalk1 : (frame == 1) ? cWalk2 : cWalk3;
                DrawTexture(wTex, (int)playerX, (int)playerY, WHITE);
            }
        }

        // Draw Top UI HUD
        DrawText(TextFormat("1P-%06d", currentScore), 10, 10, 10, RED);
        DrawText(TextFormat("HI-%06d", highScore), 160, 10, 10, RED);
        // Only draw Distance and Bonus if playing Normal mode
        if (mode == 0) {
            // Draw the bonus right below the High Score (matching the original game)
            DrawText(TextFormat("BONUS-%04d", bonus), 160, 25, 10, PINK);
            DrawText(TextFormat("DIST: %02d", (int)distance), 340, 10, 10, WHITE);
        }

        EndNESDrawing();
    }
    return -1;
}
// --- END OF FILE stage2.c ---