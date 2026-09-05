#pragma once //this guarantees the file is never copy-pasted twice.
#include "raylib.h"

// Audio Struct to hold all our game sounds in one place (much like a folder).
// GameAudio is a custom datatype, that's why we used typedef (type defination).
typedef struct {
    Music stage1;
    Music stage2;
    Music gameOver;
    Sound jump;
    Sound score;
    Sound hurt;
    Sound win;
} GameAudio; //our custom datatype name.
// Creating a type definition for a structure containing these 6 sounds, and naming this new type GameAudio.
// Music and Sound above are built-in Raylib.

// Tell all files that these globals exist
extern GameAudio audio;
extern int highScore;
extern int currentScore;

// Function declarations
void BeginNESDrawing(void);
void EndNESDrawing(void);

int RunStage1(int mode);
int RunStage2(int mode);