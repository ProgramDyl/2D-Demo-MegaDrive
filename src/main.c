#include <genesis.h>
#include <resources.h>

//*
//* Variables
//* 

// Background is stored in VRAM
u16 ind = TILE_USER_INDEX;
// Store the hscroll value
double hscroll_offset = 0;
double hscroll_offset_fg = 0;

// Player sprite and position
Sprite* player;
int player_x = 20;
int player_y = 76;

Sprite* player2Sprite;
int player2_x = 90;
int player2_y = 106;
double player2_x_f = 70.0;
double player2_y_f = 76.0;

// Player animations
#define ANIM_STILL 0
#define ANIM_IDLE 1
#define ANIM_WALK 2
#define ANIM_PUNCH 3

//player2 anim
#define PLAYER2_ANIM_STILL 0
#define PLAYER2_ANIM_IDLE 1
#define PLAYER2_ANIM_WALK 2
#define PLAYER2_ANIM_RUN 3
#define PLAYER2_ANIM_STOP 4
#define PLAYER2_ANIM_LOOKUP 5
#define PLAYER2_ANIM_DUCK 6
#define PLAYER2_ANIM_ROLL 7

int att_timer = 0;
const int att_duration = 48; // Adjust this to match the duration of the punch animation

// Define level boundaries
#define MIN_X -30
#define MAX_X 512
#define MIN_Y 50
#define MAX_Y 102

// Define scrolling limits
#define SCROLL_MAX_X (512 - 320)

// Current background
u16 current_background = 1;

// Event handler for punch action
static void joyEvent(u16 joy, u16 changed, u16 state) {
    if (changed & state & BUTTON_B) {
        SPR_setAnim(player, ANIM_PUNCH);
        att_timer = att_duration; // Set the attack timer
    }
}

// Function to load a background
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
            VDP_clearPlane(BG_A, TRUE); // Clear foreground for bg1
            break;
        case 2:
            VDP_drawImageEx(BG_A, &fg2, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE); // Set high priority for fg2
            break;
    }
    ind += fg1.tileset->numTile;
}

double player_x_f = 20.0;
double player_y_f = 76.0;
double scroll_speed = 1.0; // Set a consistent scroll speed

//player movement
static void handleMovement() {

    u16 value = JOY_readJoypad(JOY_1);

    if (att_timer > 0) return;

    // horizontal
    if (value & BUTTON_RIGHT && player_x < MAX_X) {
        player_x_f += scroll_speed;
        SPR_setAnim(player, ANIM_WALK);
        SPR_setHFlip(player, TRUE);
    } 
    else if (value & BUTTON_LEFT && player_x > MIN_X) {
        player_x_f -= scroll_speed;
        SPR_setAnim(player, ANIM_WALK);
        SPR_setHFlip(player, FALSE);
    }
    
    // vertical
    if (value & BUTTON_UP && player_y > MIN_Y) {
        player_y_f -= scroll_speed;
        SPR_setAnim(player, ANIM_WALK);
    }
    else if (value & BUTTON_DOWN && player_y < MAX_Y) {
        player_y_f += scroll_speed;
        SPR_setAnim(player, ANIM_WALK);
    }

    // if no button is pressed, play the idle animation
    if (!(value & (BUTTON_RIGHT | BUTTON_LEFT | BUTTON_UP | BUTTON_DOWN))) {
        SPR_setAnim(player, ANIM_IDLE);
    }

    player_x = (int)player_x_f;
    player_y = (int)player_y_f;

    SPR_setPosition(player, player_x, player_y);
}

//player2 movement
static void handlePlayer2Movement() {

    u16 value = JOY_readJoypad(JOY_2);

    // horizontal
    if (value & BUTTON_RIGHT && player2_x < MAX_X) {
        player2_x_f += scroll_speed;
        SPR_setAnim(player2Sprite, PLAYER2_ANIM_WALK);
        SPR_setHFlip(player2Sprite, FALSE); // Ensure HFlip is set correctly for right direction
    } 
    else if (value & BUTTON_LEFT && player2_x > MIN_X) {
        player2_x_f -= scroll_speed;
        SPR_setAnim(player2Sprite, PLAYER2_ANIM_WALK);
        SPR_setHFlip(player2Sprite, TRUE); // Ensure HFlip is set correctly for left direction
    }
    
    // vertical
    if (value & BUTTON_UP && player2_y > MIN_Y) {
        player2_y_f -= scroll_speed;
        SPR_setAnim(player2Sprite, PLAYER2_ANIM_WALK);
    }
    else if (value & BUTTON_DOWN && player2_y < MAX_Y) {
        player2_y_f += scroll_speed;
        SPR_setAnim(player2Sprite, PLAYER2_ANIM_WALK);
    }

    // if no button is pressed, play the idle animation
    if (!(value & (BUTTON_RIGHT | BUTTON_LEFT | BUTTON_UP | BUTTON_DOWN))) {
        SPR_setAnim(player2Sprite, PLAYER2_ANIM_IDLE);
    }

    player2_x = (int)player2_x_f;
    player2_y = (int)player2_y_f;

    SPR_setPosition(player2Sprite, player2_x, player2_y);
}


int main() {
    SPR_init();

    // Player 1
    PAL_setPalette(PAL2, axel.palette->data, DMA);
    player = SPR_addSprite(&axel, player_x, player_y, TILE_ATTR(PAL2, FALSE, FALSE, TRUE));

    // Player 2
    PAL_setPalette(PAL3, player2.palette->data, DMA);
    player2Sprite = SPR_addSprite(&player2, player2_x, player2_y, TILE_ATTR(PAL3, FALSE, FALSE, TRUE));

    JOY_setEventHandler(joyEvent);

    PAL_setPalette(PAL0, bg1.palette->data, DMA);
    loadBackground(current_background);

    PAL_setPalette(PAL1, fg1.palette->data, DMA);
    loadForeground(current_background);

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    while (1) {
        // handle punch animation timer
        if (att_timer > 0) {
            att_timer--;
            if (att_timer == 0) {
                SPR_setAnim(player, ANIM_IDLE);
            }
        } else {
            handleMovement();
            handlePlayer2Movement(); // Call the new player 2 movement function
        }

        // implement horizontal scrolling and background continuation
        if (player_x >= 160 && hscroll_offset < SCROLL_MAX_X) {
            hscroll_offset += scroll_speed;
            if (hscroll_offset > SCROLL_MAX_X) {
                hscroll_offset = SCROLL_MAX_X;
            }
            player_x = 160;
        } else if (player_x <= 160 && hscroll_offset > 0) {
            hscroll_offset -= scroll_speed;
            if (hscroll_offset < 0) {
                hscroll_offset = 0;
            }
            player_x = 160;
        }

        if (hscroll_offset >= SCROLL_MAX_X && current_background < 2) {
            current_background++;
            loadBackground(current_background);
            loadForeground(current_background);
            hscroll_offset = 0;
            player_x = 10;
        }

        VDP_setHorizontalScroll(BG_B, -hscroll_offset);
        VDP_setHorizontalScroll(BG_A, -hscroll_offset);

        SPR_update();
        SYS_doVBlankProcess();
    }
    return 0;
}
