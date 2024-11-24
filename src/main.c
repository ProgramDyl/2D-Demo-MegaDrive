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

//bomb sprite
// Sprite* bomb;
// int bomb_x = 30;
// int bomb_y = 90;
// bool bomb_active = FALSE; //flag for tracking if exploded

// Player animations
#define ANIM_STILL 0
#define ANIM_IDLE 1
#define ANIM_WALK 2
#define ANIM_PUNCH 3

// bomb animations
#define BOMB_STILL 0

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

// Function to handle player input
static void handleMovement() {    
    u16 value = JOY_readJoypad(JOY_1);
    
    // If player is punching, skip movement input
    if (att_timer > 0) return;

    // horizontal
    if (value & BUTTON_RIGHT && player_x < MAX_X) {
        player_x += 1;
        SPR_setAnim(player, ANIM_WALK);
        SPR_setHFlip(player, TRUE);
    } 
    else if (value & BUTTON_LEFT && player_x > MIN_X) {
        player_x -= 1;
        SPR_setAnim(player, ANIM_WALK);
        SPR_setHFlip(player, FALSE);
    }
    
    // Vertical
    if (value & BUTTON_UP && player_y > MIN_Y) {
        player_y -= 1;
        SPR_setAnim(player, ANIM_WALK);
    }
    else if (value & BUTTON_DOWN && player_y < MAX_Y) {
        player_y += 1;
        SPR_setAnim(player, ANIM_WALK);
    }

    // If no button is pressed, play the still animation
    if (!(value & (BUTTON_RIGHT | BUTTON_LEFT | BUTTON_UP | BUTTON_DOWN))) {
        SPR_setAnim(player, ANIM_IDLE);
    }

    // Update player position
    SPR_setPosition(player, player_x, player_y);
}

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

//*
//* Main Game logic
//*
int main() {
    // Sprite initialization
    SPR_init();

    PAL_setPalette(PAL2, axel.palette->data, DMA);
    player = SPR_addSprite(&axel, player_x, player_y, TILE_ATTR(PAL2, FALSE, FALSE, TRUE));
    JOY_setEventHandler(joyEvent);

    PAL_setPalette(PAL0, bg1.palette->data, DMA);
    loadBackground(current_background);

    PAL_setPalette(PAL1, fg1.palette->data, DMA);
    loadForeground(current_background);

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    while (1) {
        // Handle punch animation timer
        if (att_timer > 0) {
            att_timer--;
            if (att_timer == 0) {
                SPR_setAnim(player, ANIM_IDLE);
            }
        } else {
            handleMovement();
        }

        // Implement horizontal scrolling and background continuation
        if (player_x >= 160 && hscroll_offset < SCROLL_MAX_X) {
            hscroll_offset += (player_x - 160);
            if (hscroll_offset > SCROLL_MAX_X) {
                hscroll_offset = SCROLL_MAX_X;
            }
            player_x = 160;
        } else if (player_x <= 160 && hscroll_offset > 0) {
            hscroll_offset -= (160 - player_x);
            if (hscroll_offset < 0) {
                hscroll_offset = 0;
            }
            player_x = 160;
        }

        // Check if player reaches the edge to continue background
        if (hscroll_offset >= SCROLL_MAX_X && current_background < 2) {
            current_background++;
            loadBackground(current_background);
            loadForeground(current_background);
            hscroll_offset = 0;
            player_x = 10; // Reset player position
        }

        VDP_setHorizontalScroll(BG_B, -hscroll_offset); // Reverse the scroll direction
        VDP_setHorizontalScroll(BG_A, -hscroll_offset); // Reverse the scroll direction

        SPR_update();
        SYS_doVBlankProcess();
    }
    return 0;
}
