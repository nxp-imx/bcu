/*
* Copyright 2019 NXP.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of the NXP Semiconductor nor the names of its
* contributors may be used to endorse or promote products derived from this
* software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*/
#ifndef PORT_H
#define PORT_H

#ifdef _WIN32
#include <windows.h>
#include "ftd2xx.h"
#endif

#ifdef _WIN32
typedef PVOID FT_HANDLE;
typedef int(__stdcall* pFT_open)(int, FT_HANDLE*);
typedef int(__stdcall* pFT_open_ex)(PVOID, DWORD, FT_HANDLE*);
typedef int(__stdcall* pFT_set_bitmode)(FT_HANDLE, int, int);
typedef int(__stdcall* pFT_write)(FT_HANDLE, LPVOID, DWORD, LPDWORD);
typedef int(__stdcall* pFT_read)(FT_HANDLE, LPVOID, DWORD, LPDWORD);
typedef int(__stdcall* pFT_close)(FT_HANDLE);
typedef int(__stdcall* pFT_create_device_info_list)(LPDWORD);
typedef int(__stdcall* pFT_get_bitmode)(FT_HANDLE, PUCHAR);
typedef int(__stdcall* pFT_get_device_info_detail)(DWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, PCHAR, PCHAR, FT_HANDLE*);
typedef int(__stdcall* pFT_set_timeouts)(FT_HANDLE, DWORD, DWORD);
typedef int(__stdcall* pFT_purge)(FT_HANDLE, DWORD);
typedef int(__stdcall* pFT_EEPROM_erase)(FT_HANDLE);
typedef int(__stdcall* pFT_EEPROM_read)(FT_HANDLE, PFT_PROGRAM_DATA);
typedef int(__stdcall* pFT_EEPROM_program)(FT_HANDLE, PFT_PROGRAM_DATA);
typedef int(__stdcall* pFT_EE_uasize)(FT_HANDLE, LPDWORD);
typedef int(__stdcall* pFT_EE_uaread)(FT_HANDLE, PUCHAR, DWORD, LPDWORD);
typedef int(__stdcall* pFT_EE_uawrite)(FT_HANDLE, PUCHAR, DWORD);

#define FT_PURGE_RX 1
#define FT_PURGE_TX 2
#endif

/*is there any difference between ST_handle and fdti_context, does the difference matters?*/
struct ftdi_info {
#ifdef _WIN32

	FT_HANDLE ftdi;

	pFT_open FT_open;
	pFT_set_bitmode FT_set_bitmode;
	pFT_write FT_write;
	pFT_read FT_read;
	pFT_close FT_close;
	pFT_create_device_info_list FT_create_device_info_list;
	pFT_get_device_info_detail FT_get_device_info_detail;
	pFT_get_bitmode FT_get_bitmode;
	pFT_open_ex FT_open_ex;
	pFT_set_timeouts FT_set_timeouts;
	pFT_purge FT_purge;
	pFT_EEPROM_erase FT_EEPROM_erase;
	pFT_EEPROM_read FT_EEPROM_read;
	pFT_EEPROM_program FT_EEPROM_program;
	pFT_EE_uasize FT_EE_uasize;
	pFT_EE_uaread FT_EE_uaread;
	pFT_EE_uawrite FT_EE_uawrite;

#else
	struct ftdi_context* ftdi;
#endif

	int isinit;
};

enum BITMODE
{
	BM_RESET = 0x00,
	BM_BITBANG = 0x01,
	BM_MPSSE = 0x02,
	BM_MCU = 0x08,
	BM_OPTO = 0x10,
	BM_CBUS = 0x20,
	BM_SYNCFF = 0x40,
};

int ft_init(struct ftdi_info* ftdi);
int ft_open_channel(struct ftdi_info* fi, int channel);
int ft_open_channel_by_id(struct ftdi_info* fi, int channel, char* id);
int ft_close(struct ftdi_info* ftdi);
int ft_set_bitmode(struct ftdi_info* ftdi, int mask, int mode);
int ft_write(struct ftdi_info* ftdi, unsigned char* buffer, int size);
int ft_read(struct ftdi_info* ftdi, unsigned char* buffer, int size);
int ft_read_pins(struct ftdi_info* ftdi, unsigned char* pins);
int ft_clear_buffer(struct ftdi_info* ftdi);
void ft_list_devices();
void msleep(int duration);
int ft_erase_eeprom(struct ftdi_info* ftdi);
int ft_write_eeprom(struct ftdi_info* ftdi, unsigned int startaddr, unsigned char* buffer, int size, unsigned char* sn_buf);
int ft_read_eeprom(struct ftdi_info* ftdi, unsigned int startaddr, unsigned char* data_buf, int data_size, unsigned char* sn_buf);

#endif