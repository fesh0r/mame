/***************************************************************************

  Toaplan Slap Fight hardware

  Functions to emulate the video hardware of the machine

***************************************************************************/

#include "emu.h"
#include "includes/slapfght.h"


/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

TILE_GET_INFO_MEMBER(slapfght_state::get_pf_tile_info)
{
	/* For Performan only */
	int tile = m_slapfight_videoram[tile_index] | ((m_slapfight_colorram[tile_index] & 0x03) << 8);
	int color = (m_slapfight_colorram[tile_index] >> 3) & 0x0f;

	SET_TILE_INFO_MEMBER(0, tile, color, 0);
}

TILE_GET_INFO_MEMBER(slapfght_state::get_pf1_tile_info)
{
	int tile = m_slapfight_videoram[tile_index] | ((m_slapfight_colorram[tile_index] & 0x0f) << 8);
	int color = (m_slapfight_colorram[tile_index] & 0xf0) >> 4;

	SET_TILE_INFO_MEMBER(1, tile, color, 0);
}

TILE_GET_INFO_MEMBER(slapfght_state::get_fix_tile_info)
{
	int tile = m_slapfight_fixvideoram[tile_index] | ((m_slapfight_fixcolorram[tile_index] & 0x03) << 8);
	int color = (m_slapfight_fixcolorram[tile_index] & 0xfc) >> 2;

	SET_TILE_INFO_MEMBER(0, tile, color, 0);
}



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START_MEMBER(slapfght_state, perfrman)
{
	m_pf1_tilemap = &machine().tilemap().create(m_gfxdecode, tilemap_get_info_delegate(FUNC(slapfght_state::get_pf_tile_info), this), TILEMAP_SCAN_ROWS, 8, 8, 64, 32);

	m_pf1_tilemap->set_transparent_pen(0);
}

VIDEO_START_MEMBER(slapfght_state, slapfight)
{
	m_pf1_tilemap = &machine().tilemap().create(m_gfxdecode, tilemap_get_info_delegate(FUNC(slapfght_state::get_pf1_tile_info), this), TILEMAP_SCAN_ROWS, 8, 8, 64, 32);
	m_fix_tilemap = &machine().tilemap().create(m_gfxdecode, tilemap_get_info_delegate(FUNC(slapfght_state::get_fix_tile_info), this), TILEMAP_SCAN_ROWS, 8, 8, 64, 32);

	m_fix_tilemap->set_transparent_pen(0);
}



/***************************************************************************

  Memory handlers

***************************************************************************/

WRITE8_MEMBER(slapfght_state::slapfight_videoram_w)
{
	m_slapfight_videoram[offset] = data;
	m_pf1_tilemap->mark_tile_dirty(offset);
}

WRITE8_MEMBER(slapfght_state::slapfight_colorram_w)
{
	m_slapfight_colorram[offset] = data;
	m_pf1_tilemap->mark_tile_dirty(offset);
}

WRITE8_MEMBER(slapfght_state::slapfight_fixram_w)
{
	m_slapfight_fixvideoram[offset] = data;
	m_fix_tilemap->mark_tile_dirty(offset);
}

WRITE8_MEMBER(slapfght_state::slapfight_fixcol_w)
{
	m_slapfight_fixcolorram[offset] = data;
	m_fix_tilemap->mark_tile_dirty(offset);
}

WRITE8_MEMBER(slapfght_state::flipscreen_w)
{
	m_flipscreen = offset ? 0 : 1;
}

WRITE8_MEMBER(slapfght_state::palette_bank_w)
{
	m_palette_bank = offset;
}



/***************************************************************************

  Render the Screen

***************************************************************************/

void slapfght_state::draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect, int priority_to_display)
{
	UINT8 *src = m_spriteram->buffer();

	for (int offs = 0; offs < m_spriteram->bytes(); offs += 4)
	{
		int sx, sy;

		if ((src[offs + 2] & 0x80) == priority_to_display)
		{
			if (m_flipscreen)
			{
				sx = 265 - src[offs + 1];
				sy = 239 - src[offs + 3];
				sy &= 0xff;
			}
			else
			{
				sx = src[offs + 1] + 3;
				sy = src[offs + 3] - 1;
			}
			
			m_gfxdecode->gfx(1)->transpen(bitmap, cliprect,
				src[offs],
				((src[offs + 2] >> 1) & 3) | ((src[offs + 2] << 2) & 4) | (m_palette_bank << 3),
				m_flipscreen, m_flipscreen,
				sx, sy, 0
			);
		}
	}
}


UINT32 slapfght_state::screen_update_perfrman(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	m_pf1_tilemap->set_flip(m_flipscreen ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
	m_pf1_tilemap->set_scrolly(0, 0);
	if (m_flipscreen)
		m_pf1_tilemap->set_scrollx(0, 264);
	else
		m_pf1_tilemap->set_scrollx(0, -16);

	m_pf1_tilemap->draw(screen, bitmap, cliprect, TILEMAP_DRAW_OPAQUE, 0);
	draw_sprites(bitmap,cliprect, 0);
	m_pf1_tilemap->draw(screen, bitmap, cliprect, 0, 0);
	draw_sprites(bitmap,cliprect, 0x80);

	return 0;
}


UINT32 slapfght_state::screen_update_slapfight(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	UINT8 *src = m_spriteram->buffer();

	machine().tilemap().set_flip_all(m_flipscreen ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
	if (m_flipscreen)
	{
		m_fix_tilemap->set_scrollx(0, 296);
		m_pf1_tilemap->set_scrollx(0, (*m_slapfight_scrollx_lo + 256 * *m_slapfight_scrollx_hi) + 296);
		m_pf1_tilemap->set_scrolly(0, (*m_slapfight_scrolly) + 15);
		m_fix_tilemap->set_scrolly(0, -1); /* Glitch in Tiger Heli otherwise */
	}
	else
	{
		m_fix_tilemap->set_scrollx(0, 0);
		m_pf1_tilemap->set_scrollx(0, (*m_slapfight_scrollx_lo + 256 * *m_slapfight_scrollx_hi));
		m_pf1_tilemap->set_scrolly(0, (*m_slapfight_scrolly) - 1);
		m_fix_tilemap->set_scrolly(0, -1); /* Glitch in Tiger Heli otherwise */
	}

	m_pf1_tilemap->draw(screen, bitmap, cliprect, 0, 0);

	/* Draw the sprites */
	for (int offs = 0; offs < m_spriteram->bytes(); offs += 4)
	{
		if (m_flipscreen)
			m_gfxdecode->gfx(2)->transpen(bitmap, cliprect,
				src[offs] + ((src[offs + 2] & 0xc0) << 2),
				(src[offs + 2] & 0x1e) >> 1,
				1, 1,
				288 - (src[offs + 1] + ((src[offs + 2] & 0x01) << 8)) + 18, 240 - src[offs + 3], 0
			);
		else
			m_gfxdecode->gfx(2)->transpen(bitmap, cliprect,
				src[offs] + ((src[offs + 2] & 0xc0) << 2),
				(src[offs + 2] & 0x1e) >> 1,
				0, 0,
				(src[offs + 1] + ((src[offs + 2] & 0x01) << 8)) - 13, src[offs + 3], 0
			);
	}

	m_fix_tilemap->draw(screen, bitmap, cliprect, 0, 0);

	return 0;
}
