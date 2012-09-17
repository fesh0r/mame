/***************************************************************************

        PK-8020 driver by Miodrag Milanovic
            based on work of Sergey Erokhin from pk8020.narod.ru

        18/07/2008 Preliminary driver.

****************************************************************************/


#include "emu.h"
#include "cpu/i8085/i8085.h"
#include "machine/i8255.h"
#include "machine/wd17xx.h"
#include "imagedev/flopdrv.h"
#include "formats/basicdsk.h"
#include "includes/pk8020.h"
#include "machine/ram.h"

/* Address maps */
static ADDRESS_MAP_START(pk8020_mem, AS_PROGRAM, 8, pk8020_state )
ADDRESS_MAP_END

static ADDRESS_MAP_START( pk8020_io , AS_IO, 8, pk8020_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	ADDRESS_MAP_UNMAP_HIGH
ADDRESS_MAP_END

/* Input ports */
static INPUT_PORTS_START( pk8020 )
	PORT_START("LINE0")
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("@") PORT_CODE(KEYCODE_F9)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("A") PORT_CODE(KEYCODE_A)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("B") PORT_CODE(KEYCODE_B)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("C") PORT_CODE(KEYCODE_C)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("D") PORT_CODE(KEYCODE_D)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("E") PORT_CODE(KEYCODE_E)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F") PORT_CODE(KEYCODE_F)
		PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("G") PORT_CODE(KEYCODE_G)
	PORT_START("LINE1")
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("H") PORT_CODE(KEYCODE_H)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("I") PORT_CODE(KEYCODE_I)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("J") PORT_CODE(KEYCODE_J)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("K") PORT_CODE(KEYCODE_K)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("L") PORT_CODE(KEYCODE_L)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("M") PORT_CODE(KEYCODE_M)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("N") PORT_CODE(KEYCODE_N)
		PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("O") PORT_CODE(KEYCODE_O)
	PORT_START("LINE2")
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("P") PORT_CODE(KEYCODE_P)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Q") PORT_CODE(KEYCODE_Q)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("R") PORT_CODE(KEYCODE_R)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("S") PORT_CODE(KEYCODE_S)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("T") PORT_CODE(KEYCODE_T)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("U") PORT_CODE(KEYCODE_U)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("V") PORT_CODE(KEYCODE_V)
		PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("W") PORT_CODE(KEYCODE_W)
	PORT_START("LINE3")
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("X") PORT_CODE(KEYCODE_X)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Y") PORT_CODE(KEYCODE_Y)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Z") PORT_CODE(KEYCODE_Z)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("[") PORT_CODE(KEYCODE_OPENBRACE)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("\\") PORT_CODE(KEYCODE_BACKSLASH)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("]") PORT_CODE(KEYCODE_CLOSEBRACE)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("^") PORT_CODE(KEYCODE_TILDE)
		PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("_") PORT_CODE(KEYCODE_EQUALS)
	PORT_START("LINE4")
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0") PORT_CODE(KEYCODE_0)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1") PORT_CODE(KEYCODE_1)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2") PORT_CODE(KEYCODE_2)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3") PORT_CODE(KEYCODE_3)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4") PORT_CODE(KEYCODE_4)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5") PORT_CODE(KEYCODE_5)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6") PORT_CODE(KEYCODE_6)
		PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7") PORT_CODE(KEYCODE_7)
	PORT_START("LINE5")
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8") PORT_CODE(KEYCODE_8)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9") PORT_CODE(KEYCODE_9)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(":") PORT_CODE(KEYCODE_QUOTE)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(";") PORT_CODE(KEYCODE_COLON)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(",") PORT_CODE(KEYCODE_COMMA)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("-") PORT_CODE(KEYCODE_MINUS)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(".") PORT_CODE(KEYCODE_STOP)
		PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("?") PORT_CODE(KEYCODE_SLASH)
	PORT_START("LINE6")
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Enter") PORT_CODE(KEYCODE_ENTER)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("STRN") PORT_CODE(KEYCODE_MINUS_PAD)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("STOP") PORT_CODE(KEYCODE_F12)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("IZ") PORT_CODE(KEYCODE_ASTERISK)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("VZ") PORT_CODE(KEYCODE_SLASH_PAD)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Backspace") PORT_CODE(KEYCODE_BACKSPACE)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Tab") PORT_CODE(KEYCODE_TAB)
		PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Space") PORT_CODE(KEYCODE_SPACE)
	PORT_START("LINE7")
		// All keys in this line are reversed logic
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Left Shift") PORT_CODE(KEYCODE_LSHIFT)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Alf") PORT_CODE(KEYCODE_F11)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Graf") PORT_CODE(KEYCODE_F10)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Prf") PORT_CODE(KEYCODE_ESC)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Sel") PORT_CODE(KEYCODE_RCONTROL)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Upr") PORT_CODE(KEYCODE_LCONTROL)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("OO") PORT_CODE(KEYCODE_LALT)
		PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Right Shift") PORT_CODE(KEYCODE_RSHIFT)
	PORT_START("LINE8")
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Num 0") PORT_CODE(KEYCODE_0_PAD)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Num 1") PORT_CODE(KEYCODE_1_PAD)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Num 2") PORT_CODE(KEYCODE_2_PAD)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Num 3") PORT_CODE(KEYCODE_3_PAD)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Num 4") PORT_CODE(KEYCODE_4_PAD)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Num 5") PORT_CODE(KEYCODE_5_PAD)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Num 6") PORT_CODE(KEYCODE_6_PAD)
		PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Num 7") PORT_CODE(KEYCODE_7_PAD)
	PORT_START("LINE9")
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Num 8") PORT_CODE(KEYCODE_8_PAD)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Num 9") PORT_CODE(KEYCODE_9_PAD)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Num .") PORT_CODE(KEYCODE_DEL_PAD)
		PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_START("LINE10")
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F1") PORT_CODE(KEYCODE_F1)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F2") PORT_CODE(KEYCODE_F2)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F3") PORT_CODE(KEYCODE_F3)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F4") PORT_CODE(KEYCODE_F4)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F5") PORT_CODE(KEYCODE_F5)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_START("LINE11")
		PORT_BIT(0xFF, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_START("LINE12")
		PORT_BIT(0xFF, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_START("LINE13")
		PORT_BIT(0xFF, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_START("LINE14")
		PORT_BIT(0xFF, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_START("LINE15")
		PORT_BIT(0xFF, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END

/* Machine driver */
static const cassette_interface pk8020_cassette_interface =
{
	cassette_default_formats,
	NULL,
	(cassette_state)(CASSETTE_PLAY),
	NULL,
	NULL
};

static LEGACY_FLOPPY_OPTIONS_START(pk8020)
	LEGACY_FLOPPY_OPTION(pk8020, "kdi", "PK8020 disk image", basicdsk_identify_default, basicdsk_construct_default, NULL,
		HEADS([2])
		TRACKS([80])
		SECTORS([5])
		SECTOR_LENGTH([1024])
		FIRST_SECTOR_ID([1]))
LEGACY_FLOPPY_OPTIONS_END

static const floppy_interface pk8020_floppy_interface =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	FLOPPY_STANDARD_5_25_DSHD,
	LEGACY_FLOPPY_OPTIONS_NAME(pk8020),
	NULL,
	NULL
};

static const wd17xx_interface pk8020_wd17xx_interface =
{
	DEVCB_LINE_VCC,
	DEVCB_NULL,
	DEVCB_NULL,
	{ FLOPPY_0, FLOPPY_1, FLOPPY_2, FLOPPY_3 }
};

/* F4 Character Displayer */
static const gfx_layout pk8020_charlayout =
{
	8, 16,					/* 8 x 16 characters */
	512,					/* 512 characters */
	1,					/* 1 bits per pixel */
	{ 0 },					/* no bitplanes */
	/* x offsets */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	/* y offsets */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	8*16					/* every char takes 16 bytes */
};

static GFXDECODE_START( pk8020 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, pk8020_charlayout, 0, 4 )
GFXDECODE_END


/* Machine driver */
static MACHINE_CONFIG_START( pk8020, pk8020_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", I8080, XTAL_20MHz / 8)
	MCFG_CPU_PROGRAM_MAP(pk8020_mem)
	MCFG_CPU_IO_MAP(pk8020_io)
	MCFG_CPU_VBLANK_INT("screen", pk8020_interrupt)


	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_SIZE(512, 256)
	MCFG_SCREEN_VISIBLE_AREA(0, 512-1, 0, 256-1)
	MCFG_SCREEN_UPDATE_DRIVER(pk8020_state, screen_update_pk8020)

	MCFG_GFXDECODE(pk8020)
	MCFG_PALETTE_LENGTH(16)


	MCFG_I8255_ADD( "ppi8255_1", pk8020_ppi8255_interface_1 )
	MCFG_I8255_ADD( "ppi8255_2", pk8020_ppi8255_interface_2 )
	MCFG_I8255_ADD( "ppi8255_3", pk8020_ppi8255_interface_3 )
	MCFG_PIT8253_ADD( "pit8253", pk8020_pit8253_intf )
	MCFG_PIC8259_ADD( "pic8259", pk8020_pic8259_config )
	MCFG_I8251_ADD( "rs232", default_i8251_interface)
	MCFG_I8251_ADD( "lan", default_i8251_interface)

	MCFG_FD1793_ADD( "wd1793", pk8020_wd17xx_interface )

	/* audio hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD(SPEAKER_TAG, SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
	MCFG_SOUND_WAVE_ADD(WAVE_TAG, CASSETTE_TAG)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MCFG_CASSETTE_ADD( CASSETTE_TAG, pk8020_cassette_interface )

	MCFG_LEGACY_FLOPPY_4_DRIVES_ADD(pk8020_floppy_interface)

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("258K")	//64 + 4*48 + 2
	MCFG_RAM_DEFAULT_VALUE(0x00)
MACHINE_CONFIG_END

/* ROM definition */

ROM_START( korvet )
	ROM_REGION( 0x16000, "maincpu", ROMREGION_ERASEFF )
	ROM_DEFAULT_BIOS("v11")
	ROM_SYSTEM_BIOS(0, "v11", "v1.1")
	ROMX_LOAD( "korvet11.rom", 0x10000, 0x6000, CRC(81bdc2af) SHA1(c3484c3f1f3d252475979283c073286b8661d2b9), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "v20", "v2.0")
	ROMX_LOAD( "korvet20.rom", 0x10000, 0x6000, CRC(d6c36a45) SHA1(dba67e63457251814ad5c0fe6bb6d584eea5c7d2), ROM_BIOS(2))
	ROM_SYSTEM_BIOS(2, "cpm", "cpm")
	ROMX_LOAD( "cpm.rom",      0x10000, 0x4000, CRC(7a38d7f6) SHA1(fec6623291a38990b003e818683cd5edfb494c36), ROM_BIOS(3))
	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "korvet2.fnt", 0x0000, 0x2000, CRC(fb1cd3d4) SHA1(58f1d6e393253b1e8b497ce0880b6eff6d85b42a))
ROM_END

ROM_START( neiva )
	ROM_REGION( 0x16000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "neiva_d22.bin", 0x10000, 0x2000, CRC(9cc28f67) SHA1(68f390e846e1290df68419d522088d5325682945))
	ROM_LOAD( "neiva_d23.bin", 0x12000, 0x2000, CRC(31b53dc4) SHA1(607f2a2d8b1de469125c6c02b9ffc65649b753a2))
	ROM_LOAD( "neiva_d24.bin", 0x14000, 0x2000, CRC(d05c80df) SHA1(1ec2fa9983be5579abff7247fc9b98fe50661bd9))
	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "neiva_d21.bin", 0x0000,  0x2000, CRC(fb1cd3d4) SHA1(58f1d6e393253b1e8b497ce0880b6eff6d85b42a))
ROM_END

ROM_START( kontur )
	ROM_REGION( 0x16000, "maincpu", ROMREGION_ERASEFF )
	ROM_SYSTEM_BIOS(0, "v1", "v1")
	ROMX_LOAD( "kontur.rom",  0x10000, 0x2000, CRC(92cd441e) SHA1(9a0f9079256cefc6169ae4ba2114841d1f380480), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "v2", "v2")
	ROMX_LOAD( "kontur2.rom", 0x10000, 0x2000, CRC(5256d101) SHA1(22022a3c6882dbc5ea28d7815f00c182bbaef9e1), ROM_BIOS(2))
	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "kontur.fnt", 0x0000, 0x2000, CRC(14d33790) SHA1(6d5fcb214805c5fc44ef98a97219158ff7826ac0))
ROM_END

ROM_START( bk8t )
	ROM_REGION( 0x16000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "kor1.bin", 0x10000, 0x2000, CRC(f1e16ddc) SHA1(e3a10c9ce3f333928eb0d5f9b84e159e41fae6ca))
	ROM_LOAD( "kor2.bin", 0x12000, 0x2000, CRC(d4431d97) SHA1(08f79785846369d410a4183f0d60b856d6d70199))
	ROM_LOAD( "kor3.bin", 0x14000, 0x2000, CRC(74781903) SHA1(caaa638afe80eb83fc30b07dd6d1e40b66ddc6d1))
	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "kor4.bin", 0x0000,  0x2000, CRC(d164bada) SHA1(c334e50fd31b1f42c7668b89772487971a6875cb))
ROM_END

/* Driver */

/*    YEAR  NAME    PARENT  COMPAT  MACHINE     INPUT       INIT     COMPANY                  FULLNAME   FLAGS */
COMP( 1987, korvet,  0, 	 0,	pk8020, 	pk8020, driver_device,	0,       "<unknown>", "PK8020 Korvet",	 0)
COMP( 1987, neiva,   korvet,	 0,	pk8020, 	pk8020, driver_device,	0,       "<unknown>", "PK8020 Neiva",	 0)
COMP( 1987, kontur,  korvet,	 0,	pk8020, 	pk8020, driver_device,	0,       "<unknown>", "PK8020 Kontur",	 0)
COMP( 1987, bk8t,	 korvet,	 0,	pk8020, 	pk8020, driver_device,	0,       "<unknown>", "BK-8T",	 0)
