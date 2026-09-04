// --- START OF FILE stage1.c ---
#include "raylib.h" //Brings in Raylib so we can draw images and play sounds.

extern int highScore; // Pulling the global high score from main.c
extern int currentScore; //Pulling curentScore global variable from main.c

// extern is a keyword - it tells these variables exist, but they were created in a different file (main.c) and brings them here.

int RunStage1(int mode) { //This is the function main.c calls to start the level. It takes mode (0 for Normal, 1 for Endless).
    // --- LOAD TEXTURES ---
    Texture2D bg = LoadTexture("../images/stage1_bg.png"); //Texture2D is a Raylib data type for images that have been sent to your Graphics Card (GPU) so they can be drawn super fast.
    Texture2D lion1 = LoadTexture("../images/lion1.png");
    Texture2D lion2 = LoadTexture("../images/lion2.png");
    Texture2D lionJump = LoadTexture("../images/lion_jump.png");
    Texture2D lionHurt = LoadTexture("../images/lion_hurt.png");
    Texture2D cLion1 = LoadTexture("../images/clown_lion1.png");
    Texture2D cLion2 = LoadTexture("../images/clown_lion2.png");
    Texture2D cHurt = LoadTexture("../images/clown_hurt.png");
    Texture2D cWin1 = LoadTexture("../images/clown_win1.png");
    Texture2D cWin2 = LoadTexture("../images/clown_win2.png");
    Texture2D ringS1 = LoadTexture("../images/ring_s1.png");
    Texture2D ringS2 = LoadTexture("../images/ring_s2.png");
    Texture2D ringL1 = LoadTexture("../images/ring_l1.png");
    Texture2D ringL2 = LoadTexture("../images/ring_l2.png");
    Texture2D bagTex = LoadTexture("../images/bag.png");
    Texture2D potS = LoadTexture("../images/pot_short.png");
    Texture2D podium1 = LoadTexture("../images/podium1.png");

    // --- PLAYER VARIABLES ---
    float playerX = 40.0f; //f is used to tell it's float and not double cause we do not need double.
    float playerY = 192.0f; // The ground level (starting coordinates) for the lion.
    float speedY = 0.0f;    // Vertical speed for jumping
    bool isJumping = false; //a simple true and false case so see if you are jumping or not.
    
    /* Coordinates explanation.
    playerX is Lion's X coordinate while playerY is Lion's Y coordinate.
    Unlike math class where (0,0) is at the bottom-left, in computer graphics:
    (0, 0) is at the TOP-LEFT corner of the screen.
    playerX (Horizontal: Left ↔ Right):
    playerX = 40.0f; means the lion is placed 40 pixels away from the left edge.
    playerY (Vertical: Up ↕ Down):
    playerY = 180.0f; means the lion is placed 180 pixels down from the top edge (which is where the green grass floor is).*/
    
    /* Vertical Speed explanation
    SpeedY is vertical velocity of Lion - When the stage first boots up, the lion is just standing firmly on the grass. He is not jumping up and not falling down, so his vertical movement speed is 0 (standing still).
    How speedY works during gameplay:
    Standing on the ground:
    speedY = 0.0f (no vertical movement).
    The moment you press Space/Up to jump:
    speedY = -3.5f (Negative moves him UP toward Y = 0).
    While in mid-air (Gravity takes over):
    Every single frame, gravity adds a little bit to it: speedY += 0.15f.
    -3.5 becomes -3.0 → -2.0 → 0 (peak of the jump) → +1.0 → +3.0 (falling down).
    The moment he lands on the grass (playerY >= 180.0f):
    We reset speedY = 0.0f so he stops moving down and stands on the floor again. */
    
    // --- GAME STATE VARIABLES ---
    float bgScrollX = 0.0f; // Keeps track of background parallax. Controls the parallax scrolling and infinite looping of the circus tent background.
    // bgScrollX stores the horizontal position (X-offset) where the background image begins drawing. It starts at 0.0f.
    int state = 0;          // 0 = Playing, 1 = Burned/Dead, 2 = Won/Podium
    int endTimer = 0;       // Timer to freeze the screen before returning to menu
    /* Purpose: A frame delay counter that pauses the screen after death or victory before exiting back to main.c.
    Why it's essential:
    If this variable didn't exist, the exact millisecond your lion touched fire or the podium, the function would instantly hit return and throw you back to the main menu. You would never get to see Charlie burn or do his victory pose. */
    int animTimer = 0, frame = 0; // Controls sprite toggling (walking animation)
    // Controls the speed and toggling of walking animations. Our game loop runs at 60 FPS. If we swapped the lion's legs every frame, his legs would cycle 60 times a second, looking like a glitchy blur.
    int fireTimer = 0, fireFrame = 0; // Controls the fire flickering continuously
    
    float distance = 100.0f; // **Meters** left to reach the podium. We can change this value to change the distance required to reach the podium.
    // This distance is in meters, ahead the distances of fire rings and pots are measured in pixels not meters.
    /* distance -= (camShift * 0.02f);
    camShift is measured in PIXELS (how far you walk).
    0.02f is the conversion rate: 
    1 pixel of walking = 0.02 meters
    Total Pixels to walk = (Distance in meters)/ 0.02 */
    //In Endless Mode (mode == 1), distance is ignored so the level never spawns a podium and continues forever until death.
    
    // --- BONUS VARIABLES ---
    int bonus = 5000;      // Starts at 5000
    int bonusTimer = 0;    // A clock to measure 1 second (60 frames)

    // --- RINGS SETUP ---
    // Start with 3 long rings spaced out.
    /* Why aren't the first 3 rings random?
    If we made the initial 3 rings random the exact millisecond the game booted up:
    A fire ring might accidentally spawn at X = 50 (literally right on top of your lion's head), killing you on Frame 1 before you even touch a key.
    A pot and a ring might randomly spawn on the exact same pixel, making it impossible to survive.
    How classic retro games solve this:
    The game gives you a safe, pre-determined starting setup (the first 3 rings are long rings spaced comfortably apart).
    As soon as Ring 0 scrolls off the left screen, it goes into the - if (ringX[i] < -40.0f) - block where it gets recycled and becomes 100% random (random short/long, random bag, random spacing). */

    float ringX[3] = { 426.0f, 700.0f, 1000.0f };
    
    /* Sets the starting horizontal (X) positions of the 3 rings when the level begins.
    Our screen width is 426.
    Ring 0 is at 426.0f (waiting just off the right edge).
    Ring 1 is at 700.0f (further back in line).
    Ring 2 is at 1000.0f (even further back).
    What happens if you change the values?
    Example 1 (Instant Danger):
    float ringX[3] = { 100.0f, 250.0f, 400.0f };
    Result: Ring 0 spawns directly in front of the lion at X=100. The moment the level starts, you must jump immediately or get burned!
    Example 2 (Long Calm Start):
    float ringX[3] = { 1000.0f, 1500.0f, 2000.0f };
    Result: You will walk across empty grass for several seconds before the first ring finally reaches you. */
    
    int ringType[3] = { 1, 1, 1 }; // 1 = Long Ring, 0 = Short Ring
    int ringHasBag[3] = { 0, 0, 0 }; // Only short rings get bags
    
    /* A switch for whether a Money Bag (+100 pts) appears inside the ring:
    0 = Empty ring.
    1 = Money bag appears inside the ring.
    What happens if you change the values?
    Example (Rich start):
    int ringHasBag[3] = { 1, 1, 1 };
    Result: All 3 initial rings will have glowing money bags floating inside them. */

    bool bagCollected[3] = { false, false, false };
    
    /* Tracks whether the player has already grabbed the bag for each ring.
    false = Bag is still there waiting to be collected.
    true = Bag was grabbed; disappear from the screen and stop awarding points. */
    
    bool ringPassed[3] = { false, false, false }; // Ensures we only give points ONCE per ring
    
    // --- POTS SETUP ---
    float potX[2] = { 550.0f, 850.0f };
    bool potPassed[2] = { false, false }; // Ensures we only give points ONCE per pot
    
    // --- PODIUM SETUP ---
    float podX = 500.0f;
    bool podSpawned = false;
    /* Why is podX = 500.0f at the start?
    Our screen is 426 pixels wide.
    podX = 500.0f is off-stage in the backstage wings. It's placed past the right edge of the screen so nobody can see it.
    When you finally finish walking all 5,000 pixels and distance <= 0, this code activates:
    if (!podSpawned) { 
        podX = 426.0f;       // Move podium to the right edge of the screen
        podSpawned = true;   // Turn on the podium
    }
    Once distance hits 0M, the podium snaps to the right edge of your screen (426px) and glides into view as you walk those final few steps. */

    PlayMusicStream(audio.stage1); //This tells the computer to open the stage1.mp3 file and start playing the audio track from the beginning. We do this outside the loop so it only triggers once.

    while (!WindowShouldClose()) {
        UpdateMusicStream(audio.stage1);
        
        /* while (!WindowShouldClose()) is the engine of the game. This loop runs 60 times every second. Inside it, UpdateMusicStream constantly feeds small chunks of the mp3 file to the speakers.
        Common Question: Why do I need UpdateMusicStream if I already used PlayMusicStream?
        Answer: Music files are huge. If the game loaded the entire song into memory at once, it would lag. Instead, UpdateMusicStream trickles the audio to the speakers frame-by-frame. If you delete this line, the music will stop playing after one second. */

        if (state == 0) { // IF PLAYING
            
            // 1. Update High Score dynamically during gameplay
            if (currentScore > highScore) highScore = currentScore;
            
            /* state == 0 means the player is alive. If they are dead (1) or posing on the podium (2), the game ignores all the jumping and walking code below.
            The second line checks if your currentScore has beaten the highScore. If yes, it updates the highScore immediately. This is why you see the "HI-SCORE" number at the top of the screen go up in real-time as you collect bags. */

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
            
            /*
            What it does: We start by assuming the player isn't pressing anything (moveX = 0.0f). If they hold the Right Arrow, we give them forward speed. If they hold the Left Arrow, we give them backward (negative) speed. Finally, playerX += moveX applies that speed to the lion.
            Q: What is that ? and : symbol?
            This is called a Ternary Operator (a shortcut for an if/else statement).
            isJumping ? 2.0f : 1.5f; translates to: "Is the lion in the air? If YES, speed is 2.0. If NO, speed is 1.5."
            This perfectly mimics the classic arcade feel where jumping gives you a slight speed boost to clear wide gaps!
            🧪 Try Changing Values:
            If you change 1.5f to 5.0f: The lion will run on the ground at Sonic the Hedgehog speeds.
            If you change 2.0f to 0.0f: You will have zero air control. Once you jump, you won't be able to move left or right until you land. */
            
            float moveX = 0.0f;
            // Jumping gives slightly more forward momentum than walking
            if (IsKeyDown(KEY_RIGHT)) moveX = isJumping ? 2.0f : 1.5f; 
            else if (IsKeyDown(KEY_LEFT)) moveX = isJumping ? -2.0f : -1.5f;
            playerX += moveX;

            // 3. Camera/Scrolling Logic
            // In old 2D games, the player doesn't actually run across a giant map. The player stays somewhat in the middle of the screen, and the world moves backward like a treadmill.
            float camShift = 0.0f;
            // If the player tries to move past pixel 180 (middle of screen), 
            // we lock the player at 180 and calculate the "shift".
            // We apply this shift to the environment, making it look like the camera is moving right.
            if (playerX > 180.0f) { //as soon player crosses middle screen we stop it.
                camShift = playerX - 180.0f; //store extra pixels.
                playerX = 180.0f; //keep putting player on the middle of the screen repeatedly.
            }
            if (playerX < 10.0f) playerX = 10.0f; // Creates an invisible wall on the left side of the screen so you can't walk off-screen backward.

            /* If you change 180.0f to 400.0f: The camera won't move until your lion is practically touching the right edge of your monitor.
            If you change 10.0f to -50.0f: You could walk backward right off the left side of your screen and disappear. */

            // 4. Handle Player Jump & Gravity
            // Support both SPACEBAR and UP ARROW
            if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)) && !isJumping) { //Checks if you pressed Space or Up. - && !isJumping - ensures you can only jump if you are NOT already in the air (prevents double/infinite jumping).
                speedY = -3.5f; // Initial upward velocity - moves closer to 0.
                // Change -3.5f to -10.0f: The lion will launch into the stratosphere, flying completely off the top of the screen.
                isJumping = true;
                PlaySound(audio.jump);
            }
            
            /* What it does: If the lion is currently in the air, we update his Y position by adding his speedY. Then, we apply gravity (speedY += 0.15f).
            How Gravity works here:
            Frame 1: speedY is -3.50 (moving UP fast)
            Frame 10: speedY is -2.00 (moving UP slower)
            Frame 23: speedY is 0.00 (hanging perfectly still at the peak of the jump)
            Frame 30: speedY is +1.05 (falling DOWN)
            Frame 40: speedY is +2.50 (falling DOWN fast)
            The Landing (if playerY >= 180.0f): Once the lion falls back down to pixel 180 (the green grass), we snap him perfectly to 180.0, flip isJumping to false (allowing him to jump again), and kill his vertical speed (speedY = 0.0f).
            🧪 Try Changing Values:
            Change 0.15f to 0.01f: The lion will have far less Gravity. He will float upward very slowly and take forever to come back down.
            Change 180.0f to 200.0f: The lion will fall right through the green grass and land underneath the floor line.
            */
            
            
            if (isJumping) {
                playerY += speedY;
                speedY += 0.15f; // Gravity constantly pulls the lion down (change this value to change gravity)
                
                // If lion hits the ground, reset jump states
                if (playerY >= 192.0f) { //if player goes below it's original position of 180.0f, it teleports back to 180.0 the same instance and sets the jump to false and resetting speedY to 0.0f to make it walk again.
                    playerY = 192.0f;
                    isJumping = false;
                    speedY = 0.0f;
                }
            }

            // 5. Scroll Background
            bgScrollX -= (camShift * 0.5f); // 0.5x speed creates a nice parallax effect
            // When you push forward on the screen, the camera shifts, and we slide the background to the left. The 0.5f multiplier makes the background move at half-speed compared to the obstacles, creating a realistic sense of visual depth called parallax.
            if (bgScrollX <= -256.0f) bgScrollX += 256.0f; // Loop background seamlessly
            // Because the background image tile is 256 pixels wide, once it shifts 256 pixels to the left, it has scrolled completely out of view. By adding 256.0f, we snap it back to 0 seamlessly without the player seeing a jump or seam.
            // Change 0.5f to 1.0f: The background moves at the exact same speed as the rings. The game will look completely flat and lose its 3D depth.
            // Change 0.5f to -0.5f: The background will move to the right while you run right, which will look like a bizarre hallucination.
            // 6. Decrease distance to podium
            if (mode == 0 && distance > 0) distance -= (camShift * 0.02f);
            // The lion takes thousands of tiny pixel steps (camShift). We multiply by 0.02f to convert those thousands of pixels into a slow, steady countdown from 100M down to 0M.

            // 7. Process Rings
            for (int i = 0; i < 3; i++) {
                // Rings naturally float towards the player (1.0f speed) PLUS any camera shift
                ringX[i] -= (1.0f + camShift); //Rings direction and speed
                // 1.0f is the ring's natural floating speed. Even if you stand completely still, the rings slowly glide toward you. + camShift means if you run toward the ring, it approaches you faster.
                // If ring goes off the left side of the screen, respawn it on the right
                if (ringX[i] < -40.0f) { //The left edge of your screen is pixel 0. If the ring reaches -40.0f, it means it has safely passed behind the lion and off the left edge of your monitor. It's time to recycle it.
                    if (!podSpawned) { // Stop spawning rings if the podium is out
                        // Find the furthest ring currently on screen to ensure we don't spawn overlapping rings
                        float maxX = 426.0f;
                        for (int j = 0; j < 3; j++) { if (ringX[j] > maxX) maxX = ringX[j]; }
                        // Before we spawn the recycled ring on the right side, we need to know where the other rings are.
                        //We create a variable maxX and check all 3 rings. Whichever ring is furthest to the right (the highest X number), we save its location inside maxX.
                        
                        // --- DOUBLE RING & SPACING LOGIC ---
                        // Roll a 4-sided die. If it lands on 1 (25% chance), make a Double Ring!
                        if (GetRandomValue(1, 4) == 1) {
                            ringX[i] = maxX + 13.0f; // Spawn incredibly close to the last ring
                        } else {
                            // Normal spawn, but with a tighter gap to make the game faster
                            ringX[i] = maxX + GetRandomValue(210, 280); // Safe distance gap
                        }
                        
                        // --- NEW SPAWN LOGIC (RARE SHORT RINGS) ---
                        // GetRandomValue(1, 6) rolls a 6-sided die. 
                        // If it lands on 1, it spawns a SHORT ring (0). 
                        // If it lands on 2,3,4,5,6, it spawns a LONG ring (1).
                        // This makes short rings rarer (1 in 6 chance).
                        // We roll a 6-sided die (GetRandomValue(1, 6)). If it lands on 1, the ring becomes a 0 (Short Ring). Otherwise, it becomes a 1 (Long Ring). This creates your 1-in-6 rarity.
                        
                        // --- INDEPENDENT RANDOM ROLLS ---
                        // Roll 1: Is it a short ring? (1 in 6 chance)
                        ringType[i] = (GetRandomValue(1, 6) == 1) ? 0 : 1;
                        
                        // Roll 2: Does it have a bag?
                        if (ringType[i] == 0) {
                            // If it IS a short ring, roll a 3-sided die (1 in 3 chance for a bag)
                            ringHasBag[i] = (GetRandomValue(1, 3) == 1) ? 1 : 0; 
                        } else {
                            // If it is a LONG ring, roll a 6-sided die (1 in 6 chance for a bag)
                            ringHasBag[i] = (GetRandomValue(1, 6) == 1) ? 1 : 0; 
                        } 
                        
                        //Below, We reset the switches so you can collect points again. If the podium had spawned (the else block), we just hide the recycled ring at -100.0f forever.
                        bagCollected[i] = false;
                        ringPassed[i] = false;
                    } else {
                        ringX[i] = -100.0f; // Hide offscreen permanently if the podium is visible.
                    }
                }

                // --- HITBOX LOGIC ---
                // We define a small box around the core of the lion
                Rectangle pRect = { playerX + 8, playerY - 15, 16, 20 }; //A Rectangle is an invisible collision box. A Rectangle in Raylib always asks for exactly 4 numbers in this exact order: { Starting X, Starting Y, Width, Height }.
                
                /* Why playerX + 8 and playerY - 15? (The Starting Point)
                When Raylib draws an image, it draws it from the Top-Left Corner. But images are squares, and the lion/clown are not perfectly square. There is empty, transparent space around them.
                Also, remember: The Clown is riding on top of the lion!
                In the drawing code, we draw the lion at playerY, but we draw the clown higher up at playerY - 20.
                */
                
                /* If the hitbox was the size of the whole image, the game would be impossibly hard. If a pixel of fire touched the empty air near the lion's tail, you would die.
                So, we create a tight, custom invisible box:
                playerX + 8: We push the invisible box 8 pixels to the right. This ignores the lion's tail and the empty space in front of his nose. We only want the meaty center of his body.
                playerY - 15: Because Y=0 is the top of the screen, subtracting 15 moves the box UP. We move the box up to cover the Clown's body and the Lion's back.
                2. Why 16 and 20? (The Size)
                16: We make the box 16 pixels wide (a very skinny box covering just the center of the character).
                20: We make the box 20 pixels tall. */
                
                /* Change 16 to 100: The lion becomes incredibly fat to the computer. It will hit the fire rings even when it is standing 3 feet away from them!
                Change 'playerY - 15' to 'playerY - 50': The hitbox will float high up in the air above the clown's head. It could walk right through fire and not die, but if the empty air above clown head touches fire, it dies. */
                
                Rectangle rTop, rBot; //rTop is top hitbox of fire curve and rBot is bottom hitbox of fire curve.
                /* We create two fire boxes: one for the top curve of the ring, one for the bottom curve.
                A short ring (0) is drawn higher up. Its bottom fire rBot starts at pixel 145.
                A long ring (1) touches the ground. Its bottom fire rBot starts down at 170.
                The space between rTop and rBot is the empty hole you must pass through. */

                if (ringType[i] == 0) { 
                    // SHORT RING.
                    rTop = (Rectangle){ ringX[i] + 8, 82, 8, 30 }; 
                    rBot = (Rectangle){ ringX[i] + 8, 162, 8, 20 }; 
                } else { 
                    // LONG RING.
                    // You MUST jump to clear the bottom fire.
                    rTop = (Rectangle){ ringX[i] + 8, 82, 8, 35 }; 
                    rBot = (Rectangle){ ringX[i] + 8, 167, 8, 20 }; 
                }

                // Check if lion touches the top fire or bottom fire of the specific ring (checking collision)
                if (CheckCollisionRecs(pRect, rTop) || CheckCollisionRecs(pRect, rBot)) {
                    state = 1; // Burned!
                    PlaySound(audio.hurt); 
                    StopMusicStream(audio.stage1);
                }

                // --- RING SCORING ---
                // If the player's X passes the ring's X AND the player is currently in the air (jumping)
                /* !ringPassed[i]: You haven't already received points for this specific ring.
                playerX > ringX[i] + 10: Lion has physically crossed past the center of the ring.
                isJumping: You are currently in the air. (If you just walked under a short ring without jumping, no points for you.) */
                if (!ringPassed[i] && playerX > ringX[i] + 10 && isJumping) {
                    ringPassed[i] = true; //flipping the switch to true so you don't get points again on the very next frame.
                    // Short rings give 20 points, Long rings give 10 points.
                    currentScore += (ringType[i] == 0) ? 150 : 100; //using Ternary Operator (? :) again: "Is this a short ring (0)? If yes, add 20 points. If no, add 10 points."
                }

                // --- BAG COLLECTION ---
                if (ringHasBag[i] && !bagCollected[i] && playerX > ringX[i] && isJumping) {
                    bagCollected[i] = true;
                    currentScore += 500;
                    PlaySound(audio.score); //play the happy ding sound.
                }
            }

            // 8. Process Pots
            for (int i = 0; i < 2; i++) {
                // Pots sit on the ground, so they ONLY move when the camera shifts (player walks right)
                potX[i] -= camShift; 
                
                // Respawn pot
                if (potX[i] < -30.0f) {
                    if (!podSpawned) {
                        float maxX = 426.0f;
                        for (int j = 0; j < 2; j++) { if (potX[j] > maxX) maxX = potX[j]; }
                        potX[i] = maxX + GetRandomValue(300, 450);
                        potPassed[i] = false;
                    } else {
                        potX[i] = -100.0f;
                    }
                }
                
                // Check Pot Hitbox
                Rectangle pRect = { playerX + 10, playerY, 20, 20 };
                Rectangle potRect = { potX[i] + 5, 185, 20, 20 };
                if (CheckCollisionRecs(pRect, potRect)) {
                    state = 1; // Burned!
                    PlaySound(audio.hurt); 
                    StopMusicStream(audio.stage1);
                }

                // Pot Scoring (+10 points for jumping over a pot)
                if (!potPassed[i] && playerX > potX[i] + 15 && isJumping) {
                    potPassed[i] = true;
                    currentScore += 100;
                }
            }

            // 9. Podium Logic
            if (distance <= 0 && mode == 0) {
                if (!podSpawned) { 
                    // Look at ALL rings and pots to find the one furthest to the right
                    float furthestX = 426.0f;
                    for (int j = 0; j < 3; j++) { if (ringX[j] > furthestX) furthestX = ringX[j]; }
                    for (int j = 0; j < 2; j++) { if (potX[j] > furthestX) furthestX = potX[j]; }
                    
                    // Spawn the podium a safe 250 pixels BEHIND the absolute last obstacle!
                    podX = furthestX + 250.0f; 
                    podSpawned = true; 
                }
                
                podX -= camShift; 
                
                // The INSTANT the player's X aligns with the podium, snap them to the top.
                if (playerX + 10 > podX) {
                    state = 2; // WON!
                    playerX = podX + 5;
                    playerY = 165.0f; // Height of the podium
                    isJumping = false;
                    PlaySound(audio.win);
                    StopMusicStream(audio.stage1);
                }
            }

            // 10. Walk Animation Timer
            if (moveX != 0.0f) {
                animTimer++;
                if (animTimer > 8) { frame = !frame; animTimer = 0; } // Toggle between frame 0 and 1
            }
            /* animTimer counts frames from 0 to 8.
            Every 8 frames (~7.5 times per second), frame = !frame inverts frame (0 -> 1 -> 0 -> 1 -> 0..).
            In the drawing phase, we use this frame value to pick the sprite. When you release the arrow keys (moveX == 0), animTimer stops ticking, freezing the lion in his standing position instead of moonwalking in place. */
            
            // Fire ALWAYS animates, even when standing completely still.
            fireTimer++;
            if (fireTimer > 8) { fireFrame = !fireFrame; fireTimer = 0; }
            
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
            if (endTimer > 180) { // Wait 3 seconds before returning to menu
                if (currentScore > highScore) highScore = currentScore;
                if (state == 1) return -1;
                if (state == 2) return currentScore;
                // When state changes to 1 or 2, the main logic halts, and endTimer starts incrementing by 1 every frame (60 times a second). After exactly 3 seconds (180 frames), it returns to main.c.
                }
            }
        }

        // --- DRAWING THE SCREEN ---
        BeginNESDrawing();
        
        // Draw 16:9 Background seamlessly across the wider screen
        DrawTexture(bg, (int)bgScrollX, 40, WHITE);
        DrawTexture(bg, (int)bgScrollX + 256, 40, WHITE);
        DrawTexture(bg, (int)bgScrollX + 512, 40, WHITE);

        // Draw Rings
        for (int i = 0; i < 3; i++) {
            if (ringType[i] == 0) {
                // Short Ring
                Texture2D ring = (fireFrame) ? ringS1 : ringS2;
                DrawTexture(ring, (int)ringX[i], 112, WHITE); // 100 is y coordinate where it spawns.
                // Draw bag slightly below the top of the short ring
                if (ringHasBag[i] && !bagCollected[i]) DrawTexture(bagTex, (int)ringX[i] + 4, 124, WHITE); //Draw Bags
            } else {
                // Long Ring
                Texture2D ring = (fireFrame) ? ringL1 : ringL2;
                DrawTexture(ring, (int)ringX[i], 107, WHITE); //95 is y coordinate where it spawns.
                
                 // Draw bag slightly lower so it sits in the middle of the long ring
                if (ringHasBag[i] && !bagCollected[i]) DrawTexture(bagTex, (int)ringX[i] + 4, 122, WHITE);
            }
        }

        // Draw Pots
        for (int i = 0; i < 2; i++) {
            DrawTexture(potS, (int)potX[i], 180, WHITE);
        }

        // Draw Podium
        if (podSpawned) DrawTexture(podium1, (int)podX, 180, WHITE);

        // Draw Player Based on State
        if (state == 1) { 
            // Dead / Burned
            DrawTexture(lionHurt, (int)playerX, (int)playerY, WHITE);
            DrawTexture(cHurt, (int)playerX + 5, (int)playerY - 20, WHITE);
        } else if (state == 2) {
            // Victory Pose (Lion disappears, Clown poses)
            Texture2D cWin = (frame) ? cWin1 : cWin2;
            DrawTexture(cWin, (int)playerX, (int)playerY, WHITE);
        } else {
            // Playing
            if (isJumping) {
                DrawTexture(lionJump, (int)playerX, (int)playerY, WHITE);
                DrawTexture(cLion1, (int)playerX + 5, (int)playerY - 20, WHITE);
            } else {
                // Walking animation alternates textures
                Texture2D lTex = (frame) ? lion1 : lion2;
                Texture2D cTex = (frame) ? cLion1 : cLion2;
                DrawTexture(lTex, (int)playerX, (int)playerY, WHITE);
                DrawTexture(cTex, (int)playerX + 5, (int)playerY - 20, WHITE);
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
// --- END OF FILE stage1.c ---