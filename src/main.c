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

// Function to handle player input
static void handleMovement() {
    VDP_clearTextArea(0, 0, 40, 1);  // Clear previous debug text
    VDP_drawText("Reading Input...", 0, 0);  // Debugging text
    
    u16 value = JOY_readJoypad(JOY_1);
    bool player_moved = FALSE;
    
    // horizontal
    if (value & BUTTON_RIGHT) {
        player_x += 1;
        SPR_setAnim(player, ANIM_WALK);
		SPR_setHFlip(player, TRUE);
        player_moved = TRUE;
    } 
	else if (value & BUTTON_LEFT) {
        player_x -= 1;
        SPR_setAnim(player, ANIM_WALK);
		SPR_setHFlip(player, FALSE);
        player_moved = TRUE;
    }
    
    //Vertical
    if (value & BUTTON_UP) {
        player_y -= 1;
        SPR_setAnim(player, ANIM_WALK);
        player_moved = TRUE;
    }
	else if (value & BUTTON_DOWN) {
        player_y += 1;
        SPR_setAnim(player, ANIM_WALK);
        player_moved = TRUE;
    }

    // If no button is pressed, play the still animation
    if (!player_moved) {
        SPR_setAnim(player, ANIM_STILL);
    }

    // Update player position
    SPR_setPosition(player, player_x, player_y);
    VDP_clearTextArea(0, 0, 40, 1);  // Clear previous debug text
}

//*
//* Main Game logic
//*
int main() {
    // Sprite initialization
    SPR_init();

    PAL_setPalette(PAL2, axel.palette->data, DMA);
    player = SPR_addSprite(&axel, player_x, player_y, TILE_ATTR(PAL2, FALSE, FALSE, TRUE));
    SPR_setAnim(player, ANIM_STILL);

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

        handleMovement();
        SPR_update();

        SYS_doVBlankProcess();
    }
    return 0;
}
