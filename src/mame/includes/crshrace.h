#include "cpu/z80/z80.h"
#include "video/bufsprite.h"
#include "video/vsystem_spr.h"

class crshrace_state : public driver_device
{
public:
	crshrace_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_videoram1(*this, "videoram1"),
		m_videoram2(*this, "videoram2"),
		m_audiocpu(*this, "audiocpu"),
		m_k053936(*this, "k053936"),
		m_spriteram(*this, "spriteram"),
		m_spriteram2(*this, "spriteram2"),
		m_spr(*this, "vsystem_spr")
	{ }

	/* memory pointers */
	required_shared_ptr<UINT16> m_videoram1;
	required_shared_ptr<UINT16> m_videoram2;
	required_device<z80_device> m_audiocpu;
	required_device<k053936_device> m_k053936;
	required_device<buffered_spriteram16_device> m_spriteram;
	required_device<buffered_spriteram16_device> m_spriteram2;
	required_device<vsystem_spr_device> m_spr;

	//      UINT16 *  m_paletteram;   // currently this uses generic palette handling

	/* video-related */
	tilemap_t   *m_tilemap1;
	tilemap_t   *m_tilemap2;
	int       m_roz_bank;
	int       m_gfxctrl;
	int       m_flipscreen;

	/* misc */
	int m_pending_command;

	/* devices */
	DECLARE_READ16_MEMBER(extrarom1_r);
	DECLARE_READ16_MEMBER(extrarom2_r);
	DECLARE_WRITE8_MEMBER(crshrace_sh_bankswitch_w);
	DECLARE_WRITE16_MEMBER(sound_command_w);
	DECLARE_WRITE8_MEMBER(pending_command_clear_w);
	DECLARE_WRITE16_MEMBER(crshrace_videoram1_w);
	DECLARE_WRITE16_MEMBER(crshrace_videoram2_w);
	DECLARE_WRITE16_MEMBER(crshrace_roz_bank_w);
	DECLARE_WRITE16_MEMBER(crshrace_gfxctrl_w);
	DECLARE_CUSTOM_INPUT_MEMBER(country_sndpending_r);
	DECLARE_DRIVER_INIT(crshrace2);
	DECLARE_DRIVER_INIT(crshrace);
	TILE_GET_INFO_MEMBER(get_tile_info1);
	TILE_GET_INFO_MEMBER(get_tile_info2);
	virtual void machine_start();
	virtual void machine_reset();
	virtual void video_start();
	UINT32 screen_update_crshrace(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void screen_eof_crshrace(screen_device &screen, bool state);
};
