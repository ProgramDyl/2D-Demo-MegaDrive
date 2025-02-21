#include <genesis.h>
#include <resources.h>

// Game states
#define GAME_STATE_TITLE 0
#define GAME_STATE_PLAYING 1
#define GAME_STATE_TRANSITION 2

#define TRANSITION_ZONE_WIDTH 32

int game_state = GAME_STATE_TITLE; // Start with the title screen
int current_level = 0;

// Level structure
typedef struct {
    u16 background;
    u16 foreground;
} Level;

// Level array
Level levels[] = {
    {1, 1},
    {2, 2}
};

// Player sprite and position
Sprite* player;
int player_x = 20;
int player_y = 76;

Sprite* player2Sprite; 
int player2_x = 90;
int player2_y = 76;
double player2_x_f = 165.0;
double player2_y_f = 150.0;

double player_x_f = 20.0;
double player_y_f = 76.0;
double scroll_speed = 1.0; // Set a consistent scroll speed


void loadBackground(u16 bg);
void loadForeground(u16 fg);
void resetPlayerPosition();

// Function to reset the player's position
void resetPlayerPosition() {
    player_x = 20;
    player_y = 76;
    player_x_f = 20.0;
    player_y_f = 76.0;
    SPR_setPosition(player, player_x, player_y);
}

// Function to draw the title screen
void drawTitleScreen() {
   
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    
    VDP_drawText("PRESS START TO PLAY", 10, 12);

   
    VDP_drawText("Dylan's Game Demo!", 5, 5);
}

// Function to handle input on the title screen
void handleTitleScreenInput() {
    u16 joypad_state = JOY_readJoypad(JOY_1);
    if (joypad_state & BUTTON_START) {
        game_state = GAME_STATE_PLAYING; // Transition directly to gameplay

        // Debug 
        VDP_drawText("START PRESSED", 10, 14);

       
        PAL_setPalette(PAL0, bg1.palette->data, DMA);
        PAL_setPalette(PAL1, fg1.palette->data, DMA);
        loadBackground(levels[current_level].background);
        loadForeground(levels[current_level].foreground);

      
        PAL_setPalette(PAL2, axel.palette->data, DMA);
        player = SPR_addSprite(&axel, player_x, player_y, TILE_ATTR(PAL2, FALSE, FALSE, TRUE));

        PAL_setPalette(PAL3, player2.palette->data, DMA);
        player2Sprite = SPR_addSprite(&player2, player2_x, player2_y, TILE_ATTR(PAL3, FALSE, FALSE, TRUE));

        
        resetPlayerPosition();

       
        VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

        // Debug message
        VDP_drawText("GAMEPLAY STARTED", 10, 16);
    }
}

// Background is stored in VRAM
u16 ind = TILE_USER_INDEX;

double hscroll_offset = 0;
double target_hscroll_offset = 0; // Target scroll offset for smooth scrolling
double hscroll_offset_fg = 0;


#define ANIM_STILL 0
#define ANIM_IDLE 1
#define ANIM_WALK 2
#define ANIM_PUNCH 3


#define PLAYER2_ANIM_STILL 0
#define PLAYER2_ANIM_IDLE 1
#define PLAYER2_ANIM_WALK 2
#define PLAYER2_ANIM_RUN 3
#define PLAYER2_ANIM_STOP 4
#define PLAYER2_ANIM_LOOKUP 5
#define PLAYER2_ANIM_DUCK 6
#define PLAYER2_ANIM_ROLL 7

#define COFFEE_IDLE 0

int att_timer = 0;
const int att_duration = 48; // Adjust this to match the duration of the punch animation

// level boundaries
#define MIN_X -30
#define MAX_X 512
#define MIN_Y 50
#define MAX_Y 102

// Define scrolling limits
#define SCROLL_MAX_X (512 - 320)

u16 current_background = 1;


void loadBackground(u16 bg) {
    // Clear current backgrounds
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    // Load the new background based on the current background index
    switch (bg) {
        case 1:
            VDP_drawImageEx(BG_B, &bg1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
            break;
        case 2:
            VDP_drawImageEx(BG_B, &bg2, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
            break;
    }
    ind += bg1.tileset->numTile;
}

// Function to load a foreground
void loadForeground(u16 fg) {
    switch (fg) {
        case 1:
            VDP_clearPlane(BG_A, TRUE); 
            break;
        case 2:
            VDP_drawImageEx(BG_A, &fg2, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE); // Set high priority for fg2
            break;
    }
    ind += fg1.tileset->numTile;
}

// Function to handle player movement
void handlePlayerMovement(Sprite* playerSprite, double* player_x_f, double* player_y_f, int* player_x, int* player_y, u16 joypad_state, int animWalk, int animIdle) {

    // Horizontal movement
    if (joypad_state & BUTTON_RIGHT && *player_x < MAX_X) {
        *player_x_f += scroll_speed;
        SPR_setAnim(playerSprite, animWalk);
        SPR_setHFlip(playerSprite, TRUE);
    } 
    else if (joypad_state & BUTTON_LEFT && *player_x > MIN_X) {
        *player_x_f -= scroll_speed;
        SPR_setAnim(playerSprite, animWalk);
        SPR_setHFlip(playerSprite, FALSE);
    }
    
    // Vertical movement
    if (joypad_state & BUTTON_UP && *player_y > MIN_Y) {
        *player_y_f -= scroll_speed;
        SPR_setAnim(playerSprite, animWalk);
    }
    else if (joypad_state & BUTTON_DOWN && *player_y < MAX_Y) {
        *player_y_f += scroll_speed;
        SPR_setAnim(playerSprite, animWalk);
    }

    // If no button is pressed, play the idle animation
    if (!(joypad_state & (BUTTON_RIGHT | BUTTON_LEFT | BUTTON_UP | BUTTON_DOWN))) {
        SPR_setAnim(playerSprite, animIdle);
    }

    *player_x = (int)*player_x_f;
    *player_y = (int)*player_y_f;

    SPR_setPosition(playerSprite, *player_x, *player_y);
}

// Function to handle enemy movement (Player 2)
void handleEnemyMovement() {
    // Simple patrolling logic: move back and forth
    static int direction = 1; // 1 = right, -1 = left
    player2_x_f += direction * scroll_speed;

    // Reverse direction if the enemy hits a boundary
    if (player2_x_f >= MAX_X || player2_x_f <= MIN_X) {
        direction *= -1;
    }

    player2_x = (int)player2_x_f;
    player2_y = (int)player2_y_f;

    SPR_setPosition(player2Sprite, player2_x, player2_y);
}



// Function to check collision between Player 1 and Player 2
int checkCollision(int x1, int y1, int x2, int y2, int width, int height) {
    return (x1 < x2 + width && x1 + width > x2 && y1 < y2 + height && y1 + height > y2);
}



int main() {
    SPR_init();

    // Initialize the font for text rendering
    VDP_loadFont(&font_default, DMA);
    VDP_setTextPalette(PAL0);

    // Initialize the title screen
    drawTitleScreen();
    

    // Main game loop
    while (1) {
        // Handle title screen
        if (game_state == GAME_STATE_TITLE) {
            handleTitleScreenInput();
        }


        // Handle gameplay
        else if (game_state == GAME_STATE_PLAYING) {
        
            // Update player movement and animations
            if (att_timer > 0) {
                att_timer--;
                if (att_timer == 0) {
                    SPR_setAnim(player, ANIM_IDLE);
                }
            } else {
                // Handle Player 1 movement
                u16 joypad_state_p1 = JOY_readJoypad(JOY_1);
                handlePlayerMovement(player, &player_x_f, &player_y_f, &player_x, &player_y, joypad_state_p1, ANIM_WALK, ANIM_IDLE);

                // Handle Enemy (Player 2) movement
                handleEnemyMovement();
            }

            // Check for collision between Player 1 and Player 2
            if (checkCollision(player_x, player_y, player2_x, player2_y, 16, 16)) {
                // Handle collision (e.g., reduce health, reset position, etc.)
                VDP_drawText("COLLISION!", 10, 18);
            }

            // Smooth scrolling logic
            if (player_x >= 160 && hscroll_offset < SCROLL_MAX_X) {
                target_hscroll_offset = SCROLL_MAX_X;
            } else if (player_x <= 160 && hscroll_offset > 0) {
                target_hscroll_offset = 0;
            }

            // Smoothly interpolate hscroll_offset toward target_hscroll_offset
            if (hscroll_offset < target_hscroll_offset) {
                hscroll_offset += scroll_speed;
                if (hscroll_offset > target_hscroll_offset) {
                    hscroll_offset = target_hscroll_offset;
                }
            } else if (hscroll_offset > target_hscroll_offset) {
                hscroll_offset -= scroll_speed;
                if (hscroll_offset < target_hscroll_offset) {
                    hscroll_offset = target_hscroll_offset;
                }
            }

            // Check for background transition
            if (hscroll_offset >= (SCROLL_MAX_X - TRANSITION_ZONE_WIDTH) && current_background < 2) {
                game_state = GAME_STATE_TRANSITION; // Enter transition state
            }

            // Update scrolling
            VDP_setHorizontalScroll(BG_B, -hscroll_offset);
            VDP_setHorizontalScroll(BG_A, -hscroll_offset);
        }
        // Handle background transition
        else if (game_state == GAME_STATE_TRANSITION) {
            // Load the next background
            current_background++;
            loadBackground(current_background);
            loadForeground(current_background);

            // Reset hscroll_offset and adjust player position
            hscroll_offset = 0;
            player_x = 7; // Adjust this value to position the player correctly in the new background

            // Transition back to gameplay
            game_state = GAME_STATE_PLAYING;
        }

        // Update sprites and process VBlank
        SPR_update();
        SYS_doVBlankProcess();
    }

    return 0;
}