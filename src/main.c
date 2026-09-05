// --- START OF FILE main.c ---

// ==========================================
// MAIN MENU AND GAME CONTROLLER
// ==========================================
#include "raylib.h"
// --- INCLUDE STAGES ---
#include "game.h"
#include <math.h>
// math.h to use mathematical functions for min and max functions for collision hitboxes.

// --- CONSTANTS & GLOBALS ---
// We use 426x240 to achieve a perfect 16:9 widescreen aspect ratio.
// Classic NES was 256x240 (4:3), so widening the X-axis gives us that modern screen fit.
// The #define swaps any NES_WIDTH variable to value 426 and NES_HEIGHT variable value to 240 to achieve modern widescreen format.
#define NES_WIDTH 426
#define NES_HEIGHT 240

GameAudio audio;
RenderTexture2D target; // The internal canvas we draw the game on before scaling it up.
// Render Texture 2D is a drawing tool built in Raylib and we gave it a blank canvas called target.
// Render Texture 2D is also used to scale up the resolution to fit the monitor.
int screenW, screenH;

// Global High Score: Stored globally so it persists between Stage 1, Stage 2, and the Menu.
int highScore = 0;
int currentScore = 0;

// --- HELPER FUNCTIONS FOR SCALING ---
// This function starts drawing to our low-resolution internal canvas.
void BeginNESDrawing(void) {
    BeginTextureMode(target); //starts drawing on the virtual canvas we named target above.
    ClearBackground(BLACK); //clears the entire canvas and paints it black.
    //It is important or else each frame will leave old sprites on basically a new frame is pasted on top of old frame instead of a clean canvas.
}

// This function scales the low-res canvas up to your monitor's full size 
// while perfectly maintaining the 16:9 aspect ratio (preventing stretching/squishing).
void EndNESDrawing(void) {
    EndTextureMode(); //ends drawing on the virtual canvas.
    BeginDrawing(); //begins drawing on the acutal monitor.
    ClearBackground(BLACK); //after the canvas is upscaled to 16:9, if your screen doesn't support it, then it leaves glitchy old pixels, using this command we clear it to solid black. This forms the clean backdrop/black bars around your scaled-up game.
    
    float scale = fmin((float)screenW / NES_WIDTH, (float)screenH / NES_HEIGHT); //claculates how many times the game can get bigger before hitting to the edge of your screen - prevents cutting of the sides of game if your screen is not 16:9.
    // fmin picks the smaller multiplier. This prevents your game from stretching unevenly or cutting off at the edges.
    Rectangle source = { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }; //negative is used cause Raylib uses upside down screen, making it negative invertes it for your monitor.
    Rectangle dest = { 
        (screenW - ((float)NES_WIDTH * scale)) * 0.5f, 
        (screenH - ((float)NES_HEIGHT * scale)) * 0.5f, 
        (float)NES_WIDTH * scale, 
        (float)NES_HEIGHT * scale 
    };
    DrawTexturePro(target.texture, source, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
    EndDrawing();
}

// we using return type as int so that we know if game is open or closed.
int main(void) {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI); //Tell Windows to handle 125%/150% display scaling properly.
    // Enable Fullscreen Setup and booting everything up.
    InitWindow(800, 600, "Circus Charlie NES"); //InitWindow is a built ni Raylib funcwhich Initializes window and graphics. 800 and 600 are staring ratios for window. Raylib needs an esiting window before it can scan your hardware. That's what it does.
    Image icon = LoadImage("../images/icon.png"); // icon
    SetWindowIcon(icon); //inbuilt Raylib to set windows icon.
    UnloadImage(icon); // Free RAM after passing to GPU/window manager
    int monitor = GetCurrentMonitor(); //getting current monitor index number (if using dual monitor or screen it gets index 1 or 2 and stores the corresponding data).
    screenW = GetMonitorWidth(monitor); //getting current monitor width.
    screenH = GetMonitorHeight(monitor); //getting current monitor height.
    SetWindowPosition(0, 0); //Force window to Top-Left (0, 0) before resizing.
    SetWindowSize(screenW, screenH); //setting our game size to same size as monitor.
    ToggleFullscreen(); //enabling fullscreen.
    SetTargetFPS(60); //locking game on 60 fps so physics run on same speed in every computer.

    // Initialize Audio
    InitAudioDevice(); //inbuilt raylib func to load audio.
    audio.stage1 = LoadMusicStream("../audio/01_Jump_Through_the_Wheel_of_Fire_Get on the Bend.mp3"); //plays "01_Jump_Through_the_Wheel_of_Fire_Get on the Bend.mp3" music when audio.stage1 is used.
    audio.stage2 = LoadMusicStream("../audio/02_Ropewalking_Jump on the Jewel.mp3");
    // plays "02_Ropewalking_Jump on the Jewel.mp3" when audio.stage2 is used.
    // plays corresponding sound effects when corresponing func is used.
    audio.gameOver = LoadMusicStream("../audio/04_Game_Over.mp3");
    audio.gameOver.looping = false;
    audio.jump = LoadSound("../audio/jump.wav");     
    audio.score = LoadSound("../audio/score.wav"); 
    audio.hurt = LoadSound("../audio/hurt.wav");   
    audio.win = LoadSound("../audio/win.wav");     
    
    // Internal Render Target for Pixel Perfect Scaling
    target = LoadRenderTexture(NES_WIDTH, NES_HEIGHT);

    Texture2D titleTex = LoadTexture("images/title.png");

    int gameMode = 0; // 0=Menu, 1=Normal, 2=Endless1, 3=Endless2, 4=GameOver, 5=Victory
    // by default we are in menu.
    int menuOption = 0;
    int finalScore = 0;
    // bydefault menuOption and finalScore is 0 unless updated.

    while (!WindowShouldClose()) {

        if (gameMode == 0) { // MENU
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) menuOption++; //takes to next menu.
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) menuOption--; //takes to lower index (upper menu).
            if (menuOption > 2) menuOption = 0; //if you exceed the third menu it takes you to first option.
            if (menuOption < 0) menuOption = 2; //if you go even above first option, it takes you to last option.

            // Allow both ENTER and SPACE to select menu options
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                currentScore = 0;
                if (menuOption == 0) gameMode = 1;
                if (menuOption == 1) gameMode = 2;
                if (menuOption == 2) gameMode = 3;
            }

            BeginNESDrawing();
            DrawTexture(titleTex, (NES_WIDTH - titleTex.width) / 2, 20, WHITE); //This is a math formula used to perfectly center your title image horizontally on the screen. It takes the total canvas width (426), subtracts the width of your title image, and divides the leftover space by 2.
            
            // DrawText("text", x, y, fontSize, color);
            DrawText("PLAY SELECT", 160, 140, 10, YELLOW);
            
            DrawText("1. NORMAL MODE", 160, 170, 10, WHITE);
            DrawText("2. ENDLESS STAGE 1", 160, 190, 10, WHITE);
            DrawText("3. ENDLESS STAGE 2", 160, 210, 10, WHITE);

            DrawText("->", 140, 170 + (menuOption * 20), 10, RED);
            EndNESDrawing();
            
            /* 140 (X-coordinate): Places the red arrow selection cursor slightly to the left of your text column (which starts at 160) so it doesn't overlap your words.
            
            170 + (menuOption * 20) (Y-coordinate): This is dynamic math.
            If menuOption is 0 (Normal Mode): 170 + (0 * 20) = 170 (Arrow points to item 1).
            
            If menuOption is 1 (Endless 1): 170 + (1 * 20) = 190 (Arrow jumps down to item 2).
            
            If menuOption is 2 (Endless 2): 170 + (2 * 20) = 210 (Arrow jumps down to item 3). */
        }
        else if (gameMode == 1) { // NORMAL MODE
        // Pulling gamemode 1 pauses the main.c until it returns a value.
            int result1 = RunStage1(0); //number stores mode (normal or endless).
            if (result1 == -1) {
                finalScore = currentScore;
                PlayMusicStream(audio.gameOver);
                gameMode = 4; // -1 means Player died - 0 score, go to game over.
            } else {
                int result2 = RunStage2(0); //score carries over to second level.
                if (result2 == -1) {
                    finalScore = currentScore;
                    PlayMusicStream(audio.gameOver);
                    gameMode = 4;
                } else {
                    finalScore = currentScore;
                    gameMode = 5; // Player beat stage 2, go to victory screen
                }
            }
        }
        else if (gameMode == 2) { // ENDLESS Stage 1.
            RunStage1(1);
            finalScore = currentScore;
            PlayMusicStream(audio.gameOver);
            gameMode = 4; 
        }
        else if (gameMode == 3) { // ENDLESS Stage 2.
            RunStage2(1);
            finalScore = currentScore;
            PlayMusicStream(audio.gameOver);
            gameMode = 4; 
        }
        else if (gameMode == 4) { // GAME OVER
            // Keep streaming the game over music
            UpdateMusicStream(audio.gameOver);
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)){
                gameMode = 0;
                StopMusicStream(audio.gameOver);
            }

            BeginNESDrawing();
            DrawText("GAME OVER", 170, 100, 20, RED);
            if (finalScore == -1) finalScore = 0; 
            
            // Check if final score beats high score
            if (finalScore > highScore) highScore = finalScore;

            DrawText(TextFormat("FINAL SCORE: %06d", finalScore), 140, 140, 10, WHITE);
            DrawText("Press ENTER", 175, 180, 10, GRAY);
            EndNESDrawing();
        }
        else if (gameMode == 5) { // VICTORY
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) gameMode = 0; 

            BeginNESDrawing();
            if (finalScore > highScore) highScore = finalScore;

            DrawText("YOU BEAT THE CIRCUS!", 110, 100, 20, GREEN);
            DrawText(TextFormat("FINAL SCORE: %06d", finalScore), 140, 140, 10, WHITE);
            DrawText("Press ENTER", 175, 180, 10, GRAY);
            EndNESDrawing();
        }
    }

    // Releases all audio and music buffers from your computer's RAM.
    UnloadTexture(titleTex);
    UnloadRenderTexture(target);
    UnloadMusicStream(audio.stage1);
    UnloadMusicStream(audio.stage2);
    UnloadMusicStream(audio.gameOver);
    UnloadSound(audio.jump);
    UnloadSound(audio.score);
    UnloadSound(audio.hurt);
    UnloadSound(audio.win);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
// --- END OF FILE main.c ---