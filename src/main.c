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
int player_x = 100;
int player_y = 50;

// Player animations
#define ANIM_STILL 0
#define ANIM_IDLE 1
#define ANIM_WALK 2
#define ANIM_PUNCH 3

int att_timer = 0;
const int att_duration = 48; // Adjust this to match the duration of the punch animation

// Function to handle player input
static void handleMovement() {    
    u16 value = JOY_readJoypad(JOY_1);
    
    // If player is punching, skip movement input
    if (att_timer > 0) return;

    // horizontal
    if (value & BUTTON_RIGHT) {
        player_x += 1;
        SPR_setAnim(player, ANIM_WALK);
        SPR_setHFlip(player, TRUE);
    } 
    else if (value & BUTTON_LEFT) {
        player_x -= 1;
        SPR_setAnim(player, ANIM_WALK);
        SPR_setHFlip(player, FALSE);
    }
    
    // Vertical
    if (value & BUTTON_UP) {
        player_y -= 1;
        SPR_setAnim(player, ANIM_WALK);
    }
    else if (value & BUTTON_DOWN) {
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

    // <<About the VDP function>>
    // '0, 0' are x y coords. If you change x to 10, the background will scroll significantly
    // more than the same int in `VDP_setHorizontalScroll`. The reason why is because it uses
    // tiles. That means its 10x8px, whereas setHorizontalScroll is measured in pixels (much smoother)
    VDP_drawImageEx(BG_B, &bg1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    
    // Put tiles in VRAM
    ind += bg1.tileset->numTile;
    PAL_setPalette(PAL1, fg1.palette->data, DMA);
    // '0, 0' are the x and y coords of the background
    VDP_drawImageEx(BG_A, &fg1, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += fg1.tileset->numTile;

    // VDP_setScrollingMode() will scroll by plane, tile, or line (individually), depending on what you need. 
    // This will be by plane for simplicity
    // Plane: Scroll the whole bg image    
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    while (1) {
        // Implement scrolling
        VDP_setHorizontalScroll(BG_B, hscroll_offset);
        hscroll_offset -= 1;

        VDP_setHorizontalScroll(BG_A, hscroll_offset_fg);
        hscroll_offset_fg -= 1;

        // Handle punch animation timer
        if (att_timer > 0) {
            att_timer--;
            if (att_timer == 0) {
                SPR_setAnim(player, ANIM_IDLE);
            }
        } else {
            handleMovement();
        }

        SPR_update();
        SYS_doVBlankProcess();
    }
    return 0;
}
