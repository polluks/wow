/*******************************************************
 * __      _____ ____  _   ___ ___     ___  ___
 * \ \    / /_ _|_  / /_\ | _ \   \   / _ \| __|
 *  \ \/\/ / | | / / / _ \|   / |) | | (_) | _|
 *   \_/\_/ |___/___/_/ \_\_|_\___/   \___/|_|
 *
 *           __      _____  ___
 *           \ \    / / _ \| _ \
 *            \ \/\/ / (_) |   /
 *             \_/\_/ \___/|_|_\
 *
 * Version: 0.0
 *
 * Author: 
 *  Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Started:       2017-06-12
 * Code Started:  2017-07-05
 *
 ******************************************************/

#ifndef WOW_H
#define WOW_H

/******************************************************
 * Includes                                           *
 ******************************************************/

/* Libraries */
#include "neslib.h"

/* Nametables */
#include "wow_monsters.h"
#include "wow_scores.h"
#include "wow_dungeon.h"

/* Dungeons */
#include "dungeon1.h"
#include "dungeons.h"

/******************************************************
 * Constants                                          *
 ******************************************************/

/* States */
#define OPEN 0
#define CLOSED 1

#define YELLOW_SPAWN_X 208
#define YELLOW_SPAWN_Y 169

/* Door update, three tiles. */
#define NT_UPD_DOOR_BLUE 0
#define NT_UPD_DOOR_YELLOW 7 

/* Radar Update Nametable tiles */
#define NT_RADAR_OFF_X 11
#define NT_RADAR_OFF_Y 22

/* Palettes */
const unsigned char palette[16]={ 0x0f,0x11,0x16,0x28,0x0f,0x16,0x21,0x31,0x0f,0x28,0x16,0x11,0x0f,0x0c,0x1c,0x2c };

/* metasprites */
#include "metasprites.h"

/* metasprite animations */
#include "metasprite_animations.h"

/******************************************************
 * Variables                                          *
 ******************************************************/

/* BG Update Buffer */
static unsigned char update_buffer[80]; 

/* Score registers */
static unsigned char score0[7]={1,1,1,1,1,1,2};
static unsigned char score1[7]={1,1,1,1,1,1,1};
static unsigned char score2[7]={1,1,1,1,1,1,1};

/**
 * 8 objects on screen, two players and 6 enemies.
 */
static unsigned char stamps[72];                  // 8 slots

#define STAMP_NUM_FIELDS     9                    // Number of fields in each stamp slot
#define STAMP_NUM_SLOTS      8                    // Number of slots in stamp structure
#define STAMP_CENTER_BIAS_X  12                   // Offset to apply to box multiply to center sprite (X)
#define STAMP_CENTER_BIAS_Y  10                   // Offset to apply to box multiply to center sprite (Y)

#define RADAR_SPR_OFFSET_X   88                   // Radar sprite top-left offset X
#define RADAR_SPR_OFFSET_Y   176                  // Radar sprite top-left offset Y

#define STAMP_NUM(x)         (x*STAMP_NUM_FIELDS) // Stamp Number
#define STAMP_X(x)           (STAMP_NUM(x)+0)     // Stamp Field: X pixel position
#define STAMP_Y(x)           (STAMP_NUM(x)+1)     // Stamp Field: Y pixel position
#define STAMP_TYPE(x)        (STAMP_NUM(x)+2)     // Stamp Field: Type
#define STAMP_STATE(x)       (STAMP_NUM(x)+3)     // Stamp Field: state (which frames to use).
#define STAMP_LAST_STATE(x)  (STAMP_NUM(x)+4)
#define STAMP_FRAME(x)       (STAMP_NUM(x)+5)     // Stamp Field: Current frame
#define STAMP_DELAY(x)       (STAMP_NUM(x)+6)     // Stamp Field: Delay
#define STAMP_XTRA_A(x)      (STAMP_NUM(x)+7)     // Stamp Field: Extra A (Player Timer)
#define STAMP_XTRA_B(x)      (STAMP_NUM(x)+8)     // Stamp Field: Extra B (Player Pad Data)

#define PLAYER_PAD(x)        (stamps[STAMP_XTRA_B(x)])    // Alias for reading stored player pad value.
#define PLAYER_PAD_RIGHT(x)  (PLAYER_PAD(x)&1<<0) // is player pressing right?
#define PLAYER_PAD_LEFT(x)   (PLAYER_PAD(x)&1<<1) // is player pressing left?
#define PLAYER_PAD_DOWN(x)   (PLAYER_PAD(x)&1<<2) // is player pressing down?
#define PLAYER_PAD_UP(x)     (PLAYER_PAD(x)&1<<3) // is player pressing up?
#define PLAYER_PAD_IDLE(x)   (PLAYER_PAD(x)==0x00)      // is player idle?
  
#define PIXEL_BOX_X(x)       ((x*24)+STAMP_CENTER_BIAS_X)             // Convert Box X coordinates to pixels
#define PIXEL_BOX_Y(x)       ((x*24)+STAMP_CENTER_BIAS_Y)             // Convert Box Y coordinates to pixels
#define BOX_PIXEL_X(x)       (div24(x-STAMP_CENTER_BIAS_X))           // Convert Stamp X coordinates to Box X
#define BOX_PIXEL_Y(x)       (div24(x-STAMP_CENTER_BIAS_Y))           // Convert Stamp Y coordinates to Box Y

#define STAMP_X_TO_RADAR(x)  RADAR_SPR_OFFSET_X+BOX_PIXEL_X(x)*8          // Convert box position to radar sprite position
#define STAMP_Y_TO_RADAR(x)  RADAR_SPR_OFFSET_Y+BOX_PIXEL_Y(x)*8          // Convert box position to radar sprite position

#define BOX_WALL_RIGHT(x)    (x&1<<4)            // Box has right wall
#define BOX_WALL_DOWN(x)     (x&1<<5)            // Box has down wall
#define BOX_WALL_LEFT(x)     (x&1<<6)            // Box has left wall
#define BOX_WALL_UP(x)       (x&1<<7)            // Box has up wall


/******************************************************
 * Zero Page Variables                                *
 ******************************************************/
#pragma bssseg (push,"ZEROPAGE")
#pragma dataseg(push,"ZEROPAGE")

static unsigned char i,j,a,b,c,d;          // Index counters or temporary
static unsigned char spr;                // Pointers
static unsigned char frame_cnt;         // Frame counter (up to 256 frames)
static unsigned char sec;               // counts from 49 to 0 (one second)
static unsigned char bright;            // Brightness counter.
static unsigned int adr;                // Address
static unsigned char* str;              // String
static unsigned char* dungeon;         // Dungeon pointer.
static unsigned char* ptr;             // Generic reusable pointer.
static unsigned char blue_door_state;  // Blue door state
static unsigned char yellow_door_state; // Yellow door state
static unsigned char teleport_state;   // Teleport state

/****************************************************
 * Prototypes                                       *
 ****************************************************/

/**
 * pal_fade_to(unsigned to) - smoothly fade palette
 * to the given brightness value.
 */
void pal_fade_to(unsigned to);

/**
 * dungeon_red() - Turn dungeon red
 */
void dungeon_red(void);

/**
 * dungeon_blue() - Turn dungeon blue
 */
void dungeon_blue(void);

/**
 * double_score_win() - turn screen colors and set "DOUBLE SCORE" text on radar.
 */
void double_score_win(void);

/**
 * handle_player_in_field()
 * Handle when player is on the playfield
 */
void handle_player_in_field(void);

/**
 * handle_player_in_box()
 * Handle when player is in box.
 */
void handle_player_in_box(void);

/**
 * move_players()
 */
void move_players(void);

/**
 * move_monsters()
 * Move the monsters
 */
void move_monsters(void);

/**
 * get_current_box()
 * Get the current dungeon box for player
 * i = the stamp to return in a,b,c,d
 * a = the X box
 * b = the Y box
 * c = the dungeon box #
 * d = the box data.
 */
void get_current_box(void);

/**
 * run_dungeon() - dungeon code
 * dungeon_num - Dungeon Number to run
 */
void run_dungeon(unsigned char dungeon_num);

/**
 * attract_score() - show scores
 */
void attract_scores(void);

/**
 * attract_monsters() - shown while game isn't playing
 */
void attract_monsters(void);

/**
 * update_doors()
 */
void update_doors();

/**
 * set_teleport(openclose)
 * openclose = 0 for open, 1 for close
 */
void set_teleport(unsigned char openclose);

/**
 * clear_update_buffer() - Clear the update buffer
 */
void clear_update_buffer(void);

/**
 * init() - just as it says.
 */
void init(void);

/**
 * update_scores() - Update the score data for both players
 */
void update_scores(void);

/**
 * add_points(player)
 * player = scoreX to add points in score0 to
 */
void add_points(unsigned char player);

/**
 * clear_stamps() - Clear the on screen stamp buffer
 */
void clear_stamps(void);

/**
 * update_stamps() - Update the on-screen stamps
 */
void update_stamps(void);

/**
 * update_box_timers() - Update the box timers, if active
 */
void update_box_timers(void);


#endif /* WOW_H */

/* Notes */

// ULDR
// 1111
