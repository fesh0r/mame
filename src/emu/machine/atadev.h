/***************************************************************************

    atadev.h

    ATA Device implementation.

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#pragma once

#ifndef __ATADEV_H__
#define __ATADEV_H__

#include "emu.h"

// ======================> ata_device_interface

class ata_device_interface
{
public:
	ata_device_interface(const machine_config &mconfig, device_t &device);
	virtual ~ata_device_interface() {}

	virtual UINT16 read_dma() = 0;
	virtual DECLARE_READ16_MEMBER(read_cs0) = 0;
	virtual DECLARE_READ16_MEMBER(read_cs1) = 0;

	virtual void write_dma(UINT16 data) = 0;
	virtual DECLARE_WRITE16_MEMBER(write_cs0) = 0;
	virtual DECLARE_WRITE16_MEMBER(write_cs1) = 0;
	virtual DECLARE_WRITE_LINE_MEMBER(write_dmack) = 0;
	virtual DECLARE_WRITE_LINE_MEMBER(write_csel) = 0;
	virtual DECLARE_WRITE_LINE_MEMBER(write_dasp) = 0;

	virtual UINT8 *identify_device_buffer() = 0;

	UINT8           m_master_password_enable;
	UINT8           m_user_password_enable;
	const UINT8 *   m_master_password;
	const UINT8 *   m_user_password;

	devcb2_write_line m_irq_handler;
	devcb2_write_line m_dmarq_handler;
};

#endif
