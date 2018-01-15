#include "run_dungeon.h"
#include "constants.h"
#include "neslib.h"
#include "wow_dungeon.h"
#include "dungeon1.h"
#include "dungeons.h"

extern const unsigned char palette[16];
extern unsigned char update_buffer[80];

extern unsigned char* str;
#pragma zpsym("str")
extern unsigned char* dungeon;
#pragma zpsym("dungeon")
extern unsigned char* ptr;
#pragma zpsym("ptr")
extern unsigned char b;
#pragma zpsym("b")
extern unsigned int adr;
#pragma zpsym("adr")
extern unsigned char a;
#pragma zpsym("a")
extern unsigned char c;
#pragma zpsym("c")
extern unsigned char d;
#pragma zpsym("d")
extern unsigned char i;
#pragma zpsym("i")
extern unsigned char j;
#pragma zpsym("j")
extern unsigned char spr;
#pragma zpsym("spr")
extern unsigned char frame_cnt;
#pragma zpsym("frame_cnt")
extern unsigned char sec;
#pragma zpsym("sec")
extern unsigned char teleport_state;
#pragma zpsym("teleport_state")

extern void pal_fade_to(unsigned to);
extern void setup_enemy_sprites(void);
extern void ready_yellow_player(void);
extern void ready_blue_player(void);
extern void animate_stamps(void);
extern void move_monsters(void);
extern void move_players(void);
extern void update_stamps(void);
extern void update_radar(void);
extern void update_teleport_timer(void);
extern void update_doors(void);
extern void update_box_timers(void);
extern void set_teleport(unsigned char openclose);
extern void update_scores(void);
extern void clear_update_buffer(void);

/**
 * dungeon_red() - Turn dungeon red
 */
void dungeon_red()
{
  pal_col(1,0x16);
}

/**
 * dungeon_blue() - Turn dungeon blue
 */
void dungeon_blue()
{
  pal_col(1,0x11);
}

/**
 * double_score_win() - turn screen colors and set "DOUBLE SCORE" text on radar.
 */
void double_score_win(void)
{
  clear_update_buffer();
  update_buffer[0]=MSB(NTADR_A(10,20))|NT_UPD_HORZ;
  update_buffer[1]=LSB(NTADR_A(10,20));
  update_buffer[2]=12;
  update_buffer[3]=0x0e; // D
  update_buffer[4]=0x29; // O
  update_buffer[5]=0x2f; // U
  update_buffer[6]=0x0c; // B
  update_buffer[7]=0x26; // L
  update_buffer[8]=0x0f; // E
  update_buffer[9]=0x00; // 
  update_buffer[10]=0x2D; // S
  update_buffer[11]=0x0d;// C
  update_buffer[12]=0x29;// O
  update_buffer[13]=0x2C;// R
  update_buffer[14]=0x0f;// E
  
  update_buffer[15]=MSB(NTADR_A(10,21))|NT_UPD_HORZ;
  update_buffer[16]=LSB(NTADR_A(10,21));
  update_buffer[17]=12;
  update_buffer[18]=0x1e; // D
  update_buffer[19]=0x39; // O
  update_buffer[20]=0x3f; // U
  update_buffer[21]=0x1c; // B
  update_buffer[22]=0x36; // L
  update_buffer[23]=0x1f; // E
  update_buffer[24]=0x00; //
  update_buffer[25]=0x3D; // S
  update_buffer[26]=0x1d; // C
  update_buffer[27]=0x39; // O
  update_buffer[28]=0x3C; // R
  update_buffer[29]=0x1f; // E
  
  dungeon_red();   // Leave the dungeon red.
  j=0x20;
  for (i=0;i<150;i++) // This effect lasts for 150 frames (or 3 seconds at our 50 drop rate)
    {
      pal_col(0,j);
      if (j==0x3C)
	{
	  j=0x20;
	}
      else
	{
	  ++j;
	}
      ppu_wait_frame();
    }
  pal_col(0,0x0f);
}

/**
 * run_dungeon() - dungeon code
 * dungeon_num - Dungeon Number to run
 */
void dungeon_run(unsigned char dungeon_num)
{
  
  vram_adr(NAMETABLE_C);
  vram_fill(0,1024);

  pal_bright(0);

  // Dump dungeon template into Nametable A
  vram_adr(NAMETABLE_A);
  vram_unrle(wow_dungeon);
  pal_bg(palette);
  pal_spr(palette);
  vram_adr(NTADR_A(0,0));
  
  // Print dungeon name
  str=(unsigned char*)dungeon_names[dungeon_num-1];
  b=10;
  adr=(NTADR_A(11,20));
  vram_adr(adr);
  a=c=d=0;

  for (i=0;i<2;i++)
    {
      for (j=0;j<b;++j)
  	{
  	  d=str[j];
  	  if ((d>0x5B) && (d<0x55))
  	    {
  	      c=0x36;
  	    }
  	  else if ((d>0x45) && (d<0x56))
  	    {
  	      c=0x26;
  	    }
  	  else if ((d>0x40) && (d<0x46))
  	    {
  	      c=0x36;
  	    }
  	  else if ((d>0x29) && (d<0x40))
  	    {
  	      c=0x2F; // Needed because charset doesn't have '@'
  	    }
  	  else if (d==0x20)
  	    {
  	      c=0x20; // Get tiles 0x00 and 0x10 for space.
  	    }
  	  else
  	    {
  	      c=0x16;
  	    }
  	  vram_put(d-c+a);
  	}
      a=a+0x10;  // second part of letter is 16 cells apart.
      vram_adr(adr+0x20);
    }
  
  dungeon=(unsigned char*)dungeons[dungeon_num-1];
  
  b=0;  // dungeon array index

  for (i=0;i<6;i++)
    {
      for (j=0;j<10;++j)
  	{
  	  /* Tile 1 */
  	  vram_adr(NTADR_A((j*3)+1,(i*3)+2));
  	  if (( (dungeon[b] & 1<<7) ) && ( (dungeon[b] & 1<<6) ))            /* UP AND LEFT */
  	    {
  	      if ( (dungeon[b] & 1<<3) )                          /* LEFT WITH TELEPORT */
  		{
  		  vram_put(0x73);
  		}
  	      else
  		{
  		  vram_put(0x74);
  		}
  	    }
  	  else if ( (dungeon[b] & 1<<7) )                           /* UP */
  	    {
  	      vram_put(0x63);
  	    }
  	  else if ( (dungeon[b] & 1<<6) )                           /* LEFT */
  	    {
  	      if ( (dungeon[b] & 1<<3) )                          /* LEFT WITH TELEPORT */
  		{
  		  vram_put(0x73);
  		}
  	      else
  		{
  		  vram_put(0x65);
  		}
  	    }
  	  else
  	    {
  	      vram_put(0x00);
  	    }

  	  // Tile 2
  	  if (dungeon[b] & (1<<7))            /* UP */
  	    {
  	      vram_put(0x63);
  	    }
  	  else
  	    {
  	      vram_put(0x00);
  	    }
	  
  	  // Tile 3
  	  if (( (dungeon[b] & 1<<7) ) && ( (dungeon[b] & 1<<4) ))            /* UP AND RIGHT */
  	    {
  	      if ( (dungeon[b] & 1<<3) )                          /* RIGHT WITH TELEPORT */
  		{
  		  vram_put(0x78);
  		}
  	      else
  		{
  		  vram_put(0x75);
  		}
  	    }
  	  else if ( (dungeon[b] & 1<<7) )                           /* UP */
  	    {
  	      vram_put(0x63);
  	    }
  	  else if ( (dungeon[b] & 1<<4) )                           /* RIGHT */
  	    {
  	      if ( (dungeon[b] & 1<<3) )                          /* RIGHT WITH TELEPORT */
  		{
  		  vram_put(0x78);
  		}
  	      else
  		{
  		  vram_put(0x66);
  		}
  	    }
  	  else
  	    {
  	      vram_put(0x00);
  	    }

  	  vram_adr(NTADR_A((j*3)+1,(i*3)+3));
  	  // Tile 4
  	  if (dungeon[b] & (1<<6))            /* LEFT */
  	    {
  	      if (dungeon[b] & (1<<3))      /* LEFT WITH TELEPORT */
  		{
  		  vram_put(0x73);
  		}
  	      else
  		{
  		  vram_put(0x65);
  		}
  	    }
  	  else
  	    {
  	      vram_put(0x00);
  	    }

  	  // Tile 5 is always empty.
  	  vram_put(0x00);

  	  // Tile 6
  	  if (dungeon[b] & (1<<4))            /* RIGHT */
  	    {
  	      if (dungeon[b] & (1<<3))      /* RIGHT WITH TELEPORT */
  		{
  		  vram_put(0x78);
  		}
  	      else
  		{
  		  vram_put(0x66);
  		}
  	    }
  	  else
  	    {
  	      vram_put(0x00);
  	    }
	  

  	  vram_adr(NTADR_A((j*3)+1,(i*3)+4));
  	  // Tile 7
  	  if (( (dungeon[b] & 1<<6) ) && ( (dungeon[b] & 1<<5) ))            /* LEFT AND DOWN */
  	    {
  	      if ( (dungeon[b] & 1<<3) )                          /* LEFT WITH TELEPORT */
  		{
  		  vram_put(0x73);
  		}
  	      else
  		{
  		  vram_put(0x76);
  		}
  	    }
  	  else if ( (dungeon[b] & 1<<6) )                           /* LEFT */
  	    {
  	      if ( (dungeon[b] & 1<<3) )                          /* LEFT WITH TELEPORT */
  		{
  		  vram_put(0x73);
  		}
  	      else
  		{
  		  vram_put(0x65);
  		}
  	    }
  	  else if ( (dungeon[b] & 1<<5) )                           /* DOWN */
  	    {
  	      vram_put(0x64);
  	    }
  	  else
  	    {
  	      vram_put(0x00);
  	    }

  	  // Tile 8
  	  if (dungeon[b] & (1<<5))            /* DOWN */
  	    {
  	      vram_put(0x64);
  	    }
  	  else
  	    {
  	      vram_put(0x00);
  	    }

  	  // Tile 9
  	  if (( (dungeon[b] & 1<<4) ) && ( (dungeon[b] & 1<<5) ))            /* DOWN AND RIGHT */
  	    {
  	      if ( (dungeon[b] & 1<<3) )                          /* RIGHT WITH TELEPORT */
  		{
  		  vram_put(0x78);
  		}
  	      else
  		{
  		  vram_put(0x77);
  		}
  	    }
  	  else if ( (dungeon[b] & 1<<5) )                           /* DOWN */
  	    {
  	      vram_put(0x64);
  	    }
  	  else if ( (dungeon[b] & 1<<4) )                           /* RIGHT */
  	    {
  	      if ( (dungeon[b] & 1<<3) )                          /* RIGHT WITH TELEPORT */
  		{
  		  vram_put(0x78);
  		}
  	      else
  		{
  		  vram_put(0x66);
  		}
  	    }
  	  else
  	    {
  	      vram_put(0x00);
  	    }
  	  ++b;
  	}
    }
  
  // Point to vram update buffer for further updates.
  set_vram_update(update_buffer);
  
  ppu_on_all();
  ppu_wait_frame();
  bank_spr(1);
  bank_bg(0);
  
  pal_fade_to(4);

  // Set up the stamps for this initial run of the dungeon.
  setup_enemy_sprites();
  ready_yellow_player();
  ready_blue_player();
  a=spr=OAM_OFFSET_TOP;

  music_play(1);
  
  // At this point, we loop around and handle frame scheduling
  
  ////////////////////////////////////////////////////////////

  while(1)
    {
      
      // Set Game State

      /* blue_door_state=OPEN; */
      /* yellow_door_state=OPEN; */
      /* add_points(0); */
      /* add_points(1); */

      animate_stamps();
      move_monsters();
      move_players();
      
      // End Set Game State

      ppu_wait_frame();
      
      // Update counters
      ++frame_cnt;
      if (sec==0xff)
	{
	  sec=49;
	}
      else
	{
	  sec--;
	}

      // Update sprites
      
      update_stamps();
      update_radar();
      update_teleport_timer();
     
      // VRAM update scheduler
      
      a=frame_cnt&0x03;
      switch (a)
	{
	case 0:
	  update_doors();
	  break;
	case 1:
	  update_box_timers();
	  break;
	case 2:
	  set_teleport(teleport_state);
	  break;
	case 3:
	  update_scores();
	  break;
	}

      // End VRAM update scheduler
      
    }

  ////////////////////////////////////////////////////////////

  pal_fade_to(0);
  oam_clear();
  clear_update_buffer();
}
