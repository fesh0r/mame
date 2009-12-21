/*

      F2 System
         "bootleg tumble pop" hardware (like tumbleb.c)

  Driver by Pierpaolo Prazzoli with some bits by David Haywood


  Cross Pang        (c)1998 F2 System
  Heuk Sun Baek Sa  (c)199? Oksan / F2 System
  Bestri            (c)199? F2 System

  Cross Pang New - Is shown on the website but is not currently dumped

  No Copyright Notice is displayed for Cross Pang however the following page
  lists it as being by F2 System, Released April 1998
  http://www.f2.co.kr/eng/f2system/intro5.asp (page no longer valid, check www.f2.co.kr)


  Cross Pang:
    Audio Test isn't correct when a sound is tested, instead musics are right.
    The sample rom says 'Oksan' (Oksan made Pass, its unclear how they are
    related to Cross Pang)
  Bestri:
    Bestri includes Heuk San Baek Sa as one of it's three sub games.

  2008-08
  Added Service dipswitch and dip locations based on Service Mode.
*/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "cpu/m68000/m68000.h"
#include "sound/okim6295.h"
#include "sound/3812intf.h"
#include "includes/crospang.h"


static WRITE16_HANDLER ( crospang_soundlatch_w )
{
	if(ACCESSING_BITS_0_7)
	{
		soundlatch_w(space, 0, data & 0xff);
	}
}

/* main cpu */

static ADDRESS_MAP_START( crospang_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_ROM AM_WRITENOP // writes to rom quite often
	AM_RANGE(0x100000, 0x100001) AM_WRITENOP
	AM_RANGE(0x100002, 0x100003) AM_WRITE(crospang_fg_scrolly_w)
	AM_RANGE(0x100004, 0x100005) AM_WRITE(crospang_bg_scrollx_w)
	AM_RANGE(0x100006, 0x100007) AM_WRITE(crospang_bg_scrolly_w)
	AM_RANGE(0x100008, 0x100009) AM_WRITE(crospang_fg_scrollx_w)
	AM_RANGE(0x10000e, 0x10000f) AM_WRITENOP
	AM_RANGE(0x120000, 0x1207ff) AM_RAM_WRITE(crospang_fg_videoram_w) AM_BASE_MEMBER(crospang_state, fg_videoram)
	AM_RANGE(0x122000, 0x1227ff) AM_RAM_WRITE(crospang_bg_videoram_w) AM_BASE_MEMBER(crospang_state, bg_videoram)
	AM_RANGE(0x200000, 0x2005ff) AM_RAM_WRITE(paletteram16_xRRRRRGGGGGBBBBB_word_w) AM_BASE_GENERIC(paletteram)
	AM_RANGE(0x210000, 0x2107ff) AM_RAM AM_BASE_SIZE_MEMBER(crospang_state, spriteram, spriteram_size)
	AM_RANGE(0x270000, 0x270001) AM_WRITE(crospang_soundlatch_w)
	AM_RANGE(0x280000, 0x280001) AM_READ_PORT("P1_P2")
	AM_RANGE(0x280002, 0x280003) AM_READ_PORT("COIN")
	AM_RANGE(0x280004, 0x280005) AM_READ_PORT("DSW")
	AM_RANGE(0x320000, 0x32ffff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( bestri_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x0fffff) AM_ROM AM_WRITENOP // writes to rom quite often

	AM_RANGE(0x100004, 0x100005) AM_WRITE(bestri_fg_scrollx_w)
	AM_RANGE(0x100006, 0x100007) AM_WRITE(bestri_fg_scrolly_w)
	AM_RANGE(0x10000a, 0x10000b) AM_WRITE(bestri_bg_scrolly_w)
	AM_RANGE(0x10000c, 0x10000d) AM_WRITE(bestri_bg_scrollx_w)
	AM_RANGE(0x10000e, 0x10000f) AM_WRITE(bestri_tilebank_w)

	AM_RANGE(0x120000, 0x1207ff) AM_RAM_WRITE(crospang_fg_videoram_w) AM_BASE_MEMBER(crospang_state, fg_videoram)
	AM_RANGE(0x122000, 0x1227ff) AM_RAM_WRITE(crospang_bg_videoram_w) AM_BASE_MEMBER(crospang_state, bg_videoram)
	AM_RANGE(0x200000, 0x2005ff) AM_RAM_WRITE(paletteram16_xRRRRRGGGGGBBBBB_word_w) AM_BASE_GENERIC(paletteram)
	AM_RANGE(0x210000, 0x2107ff) AM_RAM AM_BASE_SIZE_MEMBER(crospang_state, spriteram, spriteram_size)
	AM_RANGE(0x270000, 0x270001) AM_WRITE(crospang_soundlatch_w)
	AM_RANGE(0x270004, 0x270005) AM_WRITENOP
	AM_RANGE(0x280000, 0x280001) AM_READ_PORT("P1_P2")
	AM_RANGE(0x280002, 0x280003) AM_READ_PORT("COIN")
	AM_RANGE(0x280004, 0x280005) AM_READ_PORT("DSW")

	AM_RANGE(0x3a0000, 0x3affff) AM_RAM
ADDRESS_MAP_END

/* sound cpu */

static ADDRESS_MAP_START( crospang_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_ROM
	AM_RANGE(0xc000, 0xc7ff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( crospang_sound_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x01) AM_DEVREADWRITE("ymsnd", ym3812_r, ym3812_w)
	AM_RANGE(0x02, 0x02) AM_DEVREADWRITE("oki", okim6295_r, okim6295_w)
	AM_RANGE(0x06, 0x06) AM_READ(soundlatch_r)
ADDRESS_MAP_END


static INPUT_PORTS_START( crospang )
	PORT_START("P1_P2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START("COIN")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0xfc00, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x0003, 0x0002, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SW1:1,2")
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0003, DEF_STR( 1C_2C ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x0004, 0x0004, "SW1:3" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0008, 0x0008, "SW1:4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0010, 0x0010, "SW1:5" )
	PORT_DIPNAME( 0x0020, 0x0020, "Number of Powers" ) PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(      0x0000, "1" )
	PORT_DIPSETTING(      0x0020, "2" )
	PORT_DIPNAME( 0x00c0, 0x0040, "Extra Balls" ) PORT_DIPLOCATION("SW1:7,8")
	PORT_DIPSETTING(      0x00c0, "1" )
	PORT_DIPSETTING(      0x0080, "2" )
	PORT_DIPSETTING(      0x0040, "3" )
	PORT_DIPSETTING(      0x0000, "4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0100, 0x0100, "SW2:1" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0200, 0x0200, "SW2:2" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0400, 0x0400, "SW2:3" )
	PORT_DIPNAME( 0x1800, 0x1000, "Minimum Balls per Row" ) PORT_DIPLOCATION("SW2:4,5")
	PORT_DIPSETTING(      0x1800, "3" )
	PORT_DIPSETTING(      0x1000, "4" )
	PORT_DIPSETTING(      0x0800, "5" )
	PORT_DIPSETTING(      0x0000, "6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x2000, 0x2000, "SW2:6" )
	PORT_SERVICE_DIPLOC( 0x4000, IP_ACTIVE_LOW, "SW2:7" )
	PORT_DIPUNKNOWN_DIPLOC( 0x8000, 0x8000, "SW2:8" )
INPUT_PORTS_END

static INPUT_PORTS_START( heuksun )
	PORT_START("P1_P2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START("COIN")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0xfc00, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x0003, 0x0002, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SW1:1,2")
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0003, DEF_STR( 1C_2C ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x0004, 0x0004, "SW1:3" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0008, 0x0008, "SW1:4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0010, 0x0010, "SW1:5" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0020, 0x0020, "SW1:6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0040, 0x0040, "SW1:7" )
	PORT_SERVICE_DIPLOC( 0x0080, IP_ACTIVE_LOW, "SW1:8" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0100, 0x0100, "SW2:1" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0200, 0x0200, "SW2:2" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0400, 0x0400, "SW2:3" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0800, 0x0800, "SW2:4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x1000, 0x1000, "SW2:5" )
	PORT_DIPUNKNOWN_DIPLOC( 0x2000, 0x2000, "SW2:6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x4000, 0x4000, "SW2:7" )
	PORT_DIPUNKNOWN_DIPLOC( 0x8000, 0x8000, "SW2:8" )
INPUT_PORTS_END

static INPUT_PORTS_START( bestri )
	PORT_START("P1_P2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START("COIN")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0xfc00, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_SERVICE_DIPLOC( 0x0001, IP_ACTIVE_LOW, "SW1:1" )
	PORT_DIPNAME( 0x0006, 0x0002, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SW1:2,3")
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( 1C_2C ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x0008, 0x0008, "SW1:4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0010, 0x0010, "SW1:5" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0020, 0x0020, "SW1:6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0040, 0x0040, "SW1:7" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0080, 0x0080, "SW1:8" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0100, 0x0100, "SW2:1" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0200, 0x0200, "SW2:2" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0400, 0x0400, "SW2:3" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0800, 0x0800, "SW2:4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x1000, 0x1000, "SW2:5" )
	PORT_DIPUNKNOWN_DIPLOC( 0x2000, 0x2000, "SW2:6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x4000, 0x4000, "SW2:7" )
	PORT_DIPUNKNOWN_DIPLOC( 0x8000, 0x8000, "SW2:8" )
INPUT_PORTS_END

static const gfx_layout tlayout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+8, RGN_FRAC(1,2)+0, 8, 0 },
	{ 32*8+0, 32*8+1, 32*8+2, 32*8+3, 32*8+4, 32*8+5, 32*8+6, 32*8+7,
			0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 },
	64*8
};


static GFXDECODE_START( crospang )
	//GFXDECODE_ENTRY( "gfx1", 0, tcharlayout, 256, 16 )    /* Characters 8x8 */
	//GFXDECODE_ENTRY( "gfx1", 0, tlayout,     512, 16 )    /* Tiles 16x16 */
	GFXDECODE_ENTRY( "gfx2", 0, tlayout,       0, 64 )	/* Tiles 16x16 */
	GFXDECODE_ENTRY( "gfx1", 0, tlayout,       0, 64 )	/* Sprites 16x16 */
GFXDECODE_END


static void irqhandler( const device_config *device, int linestate )
{
	crospang_state *state = (crospang_state *)device->machine->driver_data;
	cpu_set_input_line(state->audiocpu, 0, linestate);
}

static const ym3812_interface ym3812_config =
{
	irqhandler	/* IRQ Line */
};


static MACHINE_START( crospang )
{
	crospang_state *state = (crospang_state *)machine->driver_data;

	state->audiocpu = devtag_get_device(machine, "audiocpu");

	state_save_register_global(machine, state->bestri_tilebank);
	state_save_register_global(machine, state->xsproff);
	state_save_register_global(machine, state->ysproff);
}

static MACHINE_RESET( crospang )
{
	crospang_state *state = (crospang_state *)machine->driver_data;

	state->bestri_tilebank = 0;
//  state->xsproff = 4;
//  state->ysproff = 7;
	state->xsproff = 5;
	state->ysproff = 7;
}


static MACHINE_DRIVER_START( crospang )

	/* driver data */
	MDRV_DRIVER_DATA(crospang_state)

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 14318180/2)
	MDRV_CPU_PROGRAM_MAP(crospang_map)
	MDRV_CPU_VBLANK_INT("screen", irq6_line_hold)

	MDRV_CPU_ADD("audiocpu", Z80, 14318180/4)
	MDRV_CPU_PROGRAM_MAP(crospang_sound_map)
	MDRV_CPU_IO_MAP(crospang_sound_io_map)

	MDRV_MACHINE_START(crospang)
	MDRV_MACHINE_RESET(crospang)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*8, 64*8)
	MDRV_SCREEN_VISIBLE_AREA(0, 40*8-1, 0, 30*8-1)

	MDRV_PALETTE_LENGTH(0x300)
	MDRV_GFXDECODE(crospang)

	MDRV_VIDEO_START(crospang)
	MDRV_VIDEO_UPDATE(crospang)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ymsnd", YM3812, 14318180/4)
	MDRV_SOUND_CONFIG(ym3812_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MDRV_SOUND_ADD("oki", OKIM6295, 1056000)
	MDRV_SOUND_CONFIG(okim6295_interface_pin7high) // clock frequency & pin 7 not verified
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( bestri )

	/* driver data */
	MDRV_DRIVER_DATA(crospang_state)

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 14318180/2)
	MDRV_CPU_PROGRAM_MAP(bestri_map)
	MDRV_CPU_VBLANK_INT("screen", irq6_line_hold)

	MDRV_CPU_ADD("audiocpu", Z80, 14318180/4)
	MDRV_CPU_PROGRAM_MAP(crospang_sound_map)
	MDRV_CPU_IO_MAP(crospang_sound_io_map)

	MDRV_MACHINE_START(crospang)
	MDRV_MACHINE_RESET(crospang)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*8, 64*8)
	MDRV_SCREEN_VISIBLE_AREA(0, 40*8-1, 0, 30*8-1)

	MDRV_PALETTE_LENGTH(0x300)
	MDRV_GFXDECODE(crospang)

	MDRV_VIDEO_START(crospang)
	MDRV_VIDEO_UPDATE(crospang)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ymsnd", YM3812, 14318180/4)
	MDRV_SOUND_CONFIG(ym3812_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MDRV_SOUND_ADD("oki", OKIM6295, 1056000)
	MDRV_SOUND_CONFIG(okim6295_interface_pin7high) // clock frequency & pin 7 not verified
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


ROM_START( crospang )
	ROM_REGION( 0x100000, "maincpu", 0 ) /* 68k */
	ROM_LOAD16_BYTE( "p1.bin", 0x00001, 0x20000, CRC(0bcbbaad) SHA1(807f07be340d7af0aad8d49461b5a7f0221ea3b7) )
	ROM_LOAD16_BYTE( "p2.bin", 0x00000, 0x20000, CRC(0947d204) SHA1(35e7e277c51888a66d305994bf05c3f6bfc3c29e) )

	ROM_REGION( 0x10000, "audiocpu", 0 ) /* z80  */
	ROM_LOAD( "s1.bin", 0x00000, 0x10000, CRC(d61a224c) SHA1(5cd1b2d136ad58ab550c7ba135558d6c8a4cd8f6) )

	ROM_REGION( 0x40000, "oki", 0 ) /* samples */
	ROM_LOAD( "s2.bin", 0x00000, 0x20000, CRC(9f9ecd22) SHA1(631ffe14018ba39658c435b8ecb23b19a14569ee) ) // sample rom contains oksan?


	ROM_REGION( 0x80000, "gfx1", 0 ) /* bg tiles */
	ROM_LOAD16_BYTE( "rom1.bin", 0x00000, 0x40000, CRC(905042bb) SHA1(ed5b97e88d24e55f8fcfaaa34251582976cb2527) )
	ROM_LOAD16_BYTE( "rom2.bin", 0x00001, 0x40000, CRC(bc4381e9) SHA1(af0690c253bead3448db5ec8fb258d8284646e89) )

	ROM_REGION( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "rom3.bin", 0x000000, 0x80000, CRC(cc6e1fce) SHA1(eb5b3ca7222f48916dc6206f987b2669fe7e7c6b) )
	ROM_LOAD16_BYTE( "rom4.bin", 0x000001, 0x80000, CRC(9a91d494) SHA1(1c6280f662f1cf53f7f6defb7e215da75b573fdf) )
	ROM_LOAD16_BYTE( "rom5.bin", 0x100000, 0x80000, CRC(53a34dc5) SHA1(2e5cf8093bf507e81d7447736b7727c3fd20c471) )
	ROM_LOAD16_BYTE( "rom6.bin", 0x100001, 0x80000, CRC(9c633082) SHA1(18b8591b695ee429c9c9855d8cbba6249a4bd809) )
ROM_END


/*
Heuk Sun Baek Sa

+----------------------------------+
|  YM3014 YM3812  M6295 us08       |
|        6116                  uc07|
|      us02  Z80               uc08|
|J  6116                       ud14|
|A  6116                       ud15|
|M    62256 62256 6264         ud16|
|M DSW2 ua02 ua03 6264         ud17|
|A      68000  A1020B              |
|  DSW1                            |
|                                  |
|14.318MHz                         |
+----------------------------------+

Motorola MC68000P10
NEC D780C
Actel A1020B PL84C
YM3812/YM3014 (badged as UA010 & 5A14)
OKI M6295

*/

ROM_START( heuksun )
	ROM_REGION( 0x100000, "maincpu", 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "ua02.j3", 0x00001, 0x80000, CRC(db2b9c8e) SHA1(aa37e3a056957a12888e2e3112fe78a6bff7d76f) )
	ROM_LOAD16_BYTE( "ua03.j5", 0x00000, 0x80000, CRC(de9f01e8) SHA1(3ee9206e7c3c7bebd7cde6f201c2fa7f9f6553b7) )

	ROM_REGION( 0x040000, "audiocpu", 0 ) /* Z80 */
	ROM_LOAD( "us02.r4", 0x00000, 0x10000, CRC(c7cc05fa) SHA1(5fbf479be98f618c63e4c74a250d51279c2f5e3b) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "us08.u7", 0x00000, 0x40000, CRC(ae177589) SHA1(9a1e2b848046f3506ede4f218a9175cc8e984ad8) )

	ROM_REGION( 0x200000, "gfx1", 0 ) // tiles
	ROM_LOAD16_BYTE( "uc08.r11", 0x00001, 0x20000, CRC(242cee69) SHA1(71112ea6aac4db9b923315656f12d2f72173d9cd) )
	ROM_CONTINUE ( 0x100001,0x20000)
	ROM_CONTINUE ( 0x040001,0x20000)
	ROM_CONTINUE ( 0x140001,0x20000)
	ROM_LOAD16_BYTE( "uc07.t11", 0x00000, 0x20000, CRC(4d1ed885) SHA1(2868394658fac70e31ebd150377d76cfe63a4d5f) )
	ROM_CONTINUE ( 0x100000,0x20000)
	ROM_CONTINUE ( 0x040000,0x20000)
	ROM_CONTINUE ( 0x140000,0x20000)


	ROM_REGION( 0x100000, "gfx2", 0 ) // sprites
	ROM_LOAD16_BYTE( "ud14.p11", 0x00000, 0x40000, CRC(4fc2b574) SHA1(f3330d9cc3065b5a96e222300c2ae01e57241632) )
	ROM_LOAD16_BYTE( "ud15.m11", 0x00001, 0x40000, CRC(1d6187a6) SHA1(51f1ac086d67e8b35081ddc14e28b218d3153779) )
	ROM_LOAD16_BYTE( "ud16.l11", 0x80000, 0x40000, CRC(eabec43e) SHA1(fa0a7886ccaf90e9ed59dc283e27f9e8e9aa7d29) )
	ROM_LOAD16_BYTE( "ud17.j11", 0x80001, 0x40000, CRC(c6b04171) SHA1(4d142cad4e0d62764144784634fabeef97d07630) )
ROM_END


/*

Bestri

+----------------------------------+
| YM3014 YM3812  M6295 us08        |
|        us02   6116           uc07|
|         Z80                  uc28|
|       6116                   uc08|
|J      6116                   ud29|
|A    62256 62256 6264         ud14|
|M      ua02 ua03 6264         ud15|
|M DSW2 68000    QL2003        ud16|
|A                   6116      ud17|
|  DSW1              6116          |
|               6116               |
|               6116               |
|    14.31818MHz                   |
+----------------------------------+

Motorola MC68000P10
ZiLOG Z840006PSC (6MHz rated)
QuickLogic QL20003-XPL84C
Yamaha YM3812/YM3014
OKI M6295


ua02.i3 (Odd)
ua03.i5 (even)

Numbers/letters to right of rom name denotes
  numbers/letters silkscreened under socket

uc07.p12 0
uc28.n12 1
uc08.m12 2
uc29.k12 3
ud14.j12 A
ud15.h12 B
ud16.g12 C
ud17.e12 D

*/

ROM_START( bestri )
	ROM_REGION( 0x100000, "maincpu", 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "ua02.i3", 0x00001, 0x80000, CRC(9e94023d) SHA1(61a07eb835d324cb4fe7e3d366dd3907838b2554) )
	ROM_LOAD16_BYTE( "ua03.i5", 0x00000, 0x80000, CRC(08cfa8d8) SHA1(684729887bf2dd2fe22e5bd2e32073169d426e02) )

	ROM_REGION( 0x040000, "audiocpu", 0 ) /* Z80 */
	ROM_LOAD( "us02.p3", 0x00000, 0x10000, CRC(c7cc05fa) SHA1(5fbf479be98f618c63e4c74a250d51279c2f5e3b) ) // same as huek

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "us08.q7", 0x00000, 0x40000, CRC(85d8f3de) SHA1(af55678bbe2c187cfee063c6f74cdd568307a7a2) )

	ROM_REGION( 0x200000, "gfx1", 0 ) // tiles

	ROM_LOAD16_BYTE( "uc08.m12", 0x00001, 0x20000, CRC(2fc0c30e) SHA1(0c50efd20340f10961e872b3cd63c36aefed26f0) )
	ROM_CONTINUE ( 0x100001,0x20000)
	ROM_CONTINUE ( 0x040001,0x20000)
	ROM_CONTINUE ( 0x140001,0x20000)
	ROM_LOAD16_BYTE( "uc07 p12", 0x00000, 0x20000,  CRC(3d299954) SHA1(f3a4d6fd02ed0803758b1ea3fbaccbb4dbb04718) )
	ROM_CONTINUE ( 0x100000,0x20000)
	ROM_CONTINUE ( 0x040000,0x20000)
	ROM_CONTINUE ( 0x140000,0x20000)
	ROM_LOAD16_BYTE( "uc28.n12", 0x80001, 0x20000, CRC(9938be27) SHA1(1da7861dc44eba6e4ed6a27997428f7652b2f3b5) )
	ROM_CONTINUE ( 0x180001,0x20000)
	ROM_CONTINUE ( 0x0c0001,0x20000)
	ROM_CONTINUE ( 0x1c0001,0x20000)
	ROM_LOAD16_BYTE( "uc29.k12", 0x80000, 0x20000, CRC(0260c321) SHA1(0ae7754c0f7de314bd72c51e273f7aaea2bae705) )
	ROM_CONTINUE ( 0x180000,0x20000)
	ROM_CONTINUE ( 0x0c0000,0x20000)
	ROM_CONTINUE ( 0x1c0000,0x20000)


	ROM_REGION( 0x200000, "gfx2", 0 ) // sprites
	ROM_LOAD16_BYTE( "ud14.j12", 0x000000, 0x80000, CRC(141c696e) SHA1(3d35a20f7c12a8d8a9f6d351f06fb9df0c673354) )
	ROM_LOAD16_BYTE( "ud15.h12", 0x000001, 0x80000, CRC(7c04adc0) SHA1(9883565d6556ce8ae3da6c91cbf04894e87e6923) )
	ROM_LOAD16_BYTE( "ud16.g12", 0x100000, 0x80000, CRC(3282ea76) SHA1(cc21cac35f47ba299823c2cfe6b4946f8483b821) )
	ROM_LOAD16_BYTE( "ud17.e12", 0x100001, 0x80000, CRC(3a3a3f1a) SHA1(48843140cd63c9387e09b84bd41b13dba35f48ad) )
ROM_END


static void tumblepb_gfx1_rearrange(running_machine *machine)
{
	UINT8 *rom = memory_region(machine, "gfx1");
	int len = memory_region_length(machine, "gfx1");
	int i;

	/* gfx data is in the wrong order */
	for (i = 0; i < len; i++)
	{
		if ((i & 0x20) == 0)
		{
			int t = rom[i]; rom[i] = rom[i + 0x20]; rom[i + 0x20] = t;
		}
	}
	/* low/high half are also swapped */
	for (i = 0; i < len / 2; i++)
	{
		int t = rom[i]; rom[i] = rom[i + len / 2]; rom[i + len / 2] = t;
	}
}

static DRIVER_INIT( crospang )
{
	tumblepb_gfx1_rearrange(machine);
}

GAME( 1998, crospang, 0, crospang, crospang, crospang, ROT0, "F2 System", "Cross Pang", GAME_SUPPORTS_SAVE )
GAME( 199?, heuksun,  0, crospang, heuksun,  crospang, ROT0, "Oksan / F2 System", "Heuk Sun Baek Sa (Korea)", GAME_SUPPORTS_SAVE )
GAME( 1998, bestri,   0, bestri,   bestri,   crospang, ROT0, "F2 System", "Bestri (Korea)", GAME_SUPPORTS_SAVE )
