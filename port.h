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
#endif

#ifdef _WIN32

#define FT_OPEN_BY_SERIAL_NUMBER	1
#define FT_OPEN_BY_DESCRIPTION		2
#define FT_OPEN_BY_LOCATION		4

#define FT_OPEN_MASK (FT_OPEN_BY_SERIAL_NUMBER | \
                      FT_OPEN_BY_DESCRIPTION | \
                      FT_OPEN_BY_LOCATION)

typedef struct ft_program_data {

	DWORD Signature1;			// Header - must be 0x00000000
	DWORD Signature2;			// Header - must be 0xffffffff
	DWORD Version;				// Header - FT_PROGRAM_DATA version
	//			0 = original
	//			1 = FT2232 extensions
	//			2 = FT232R extensions
	//			3 = FT2232H extensions
	//			4 = FT4232H extensions
	//			5 = FT232H extensions

	WORD VendorId;				// 0x0403
	WORD ProductId;				// 0x6001
	char* Manufacturer;			// "FTDI"
	char* ManufacturerId;		// "FT"
	char* Description;			// "USB HS Serial Converter"
	char* SerialNumber;			// "FT000001" if fixed, or NULL
	WORD MaxPower;				// 0 < MaxPower <= 500
	WORD PnP;					// 0 = disabled, 1 = enabled
	WORD SelfPowered;			// 0 = bus powered, 1 = self powered
	WORD RemoteWakeup;			// 0 = not capable, 1 = capable
	//
	// Rev4 (FT232B) extensions
	//
	UCHAR Rev4;					// non-zero if Rev4 chip, zero otherwise
	UCHAR IsoIn;				// non-zero if in endpoint is isochronous
	UCHAR IsoOut;				// non-zero if out endpoint is isochronous
	UCHAR PullDownEnable;		// non-zero if pull down enabled
	UCHAR SerNumEnable;			// non-zero if serial number to be used
	UCHAR USBVersionEnable;		// non-zero if chip uses USBVersion
	WORD USBVersion;			// BCD (0x0200 => USB2)
	//
	// Rev 5 (FT2232) extensions
	//
	UCHAR Rev5;					// non-zero if Rev5 chip, zero otherwise
	UCHAR IsoInA;				// non-zero if in endpoint is isochronous
	UCHAR IsoInB;				// non-zero if in endpoint is isochronous
	UCHAR IsoOutA;				// non-zero if out endpoint is isochronous
	UCHAR IsoOutB;				// non-zero if out endpoint is isochronous
	UCHAR PullDownEnable5;		// non-zero if pull down enabled
	UCHAR SerNumEnable5;		// non-zero if serial number to be used
	UCHAR USBVersionEnable5;	// non-zero if chip uses USBVersion
	WORD USBVersion5;			// BCD (0x0200 => USB2)
	UCHAR AIsHighCurrent;		// non-zero if interface is high current
	UCHAR BIsHighCurrent;		// non-zero if interface is high current
	UCHAR IFAIsFifo;			// non-zero if interface is 245 FIFO
	UCHAR IFAIsFifoTar;			// non-zero if interface is 245 FIFO CPU target
	UCHAR IFAIsFastSer;			// non-zero if interface is Fast serial
	UCHAR AIsVCP;				// non-zero if interface is to use VCP drivers
	UCHAR IFBIsFifo;			// non-zero if interface is 245 FIFO
	UCHAR IFBIsFifoTar;			// non-zero if interface is 245 FIFO CPU target
	UCHAR IFBIsFastSer;			// non-zero if interface is Fast serial
	UCHAR BIsVCP;				// non-zero if interface is to use VCP drivers
	//
	// Rev 6 (FT232R) extensions
	//
	UCHAR UseExtOsc;			// Use External Oscillator
	UCHAR HighDriveIOs;			// High Drive I/Os
	UCHAR EndpointSize;			// Endpoint size
	UCHAR PullDownEnableR;		// non-zero if pull down enabled
	UCHAR SerNumEnableR;		// non-zero if serial number to be used
	UCHAR InvertTXD;			// non-zero if invert TXD
	UCHAR InvertRXD;			// non-zero if invert RXD
	UCHAR InvertRTS;			// non-zero if invert RTS
	UCHAR InvertCTS;			// non-zero if invert CTS
	UCHAR InvertDTR;			// non-zero if invert DTR
	UCHAR InvertDSR;			// non-zero if invert DSR
	UCHAR InvertDCD;			// non-zero if invert DCD
	UCHAR InvertRI;				// non-zero if invert RI
	UCHAR Cbus0;				// Cbus Mux control
	UCHAR Cbus1;				// Cbus Mux control
	UCHAR Cbus2;				// Cbus Mux control
	UCHAR Cbus3;				// Cbus Mux control
	UCHAR Cbus4;				// Cbus Mux control
	UCHAR RIsD2XX;				// non-zero if using D2XX driver
	//
	// Rev 7 (FT2232H) Extensions
	//
	UCHAR PullDownEnable7;		// non-zero if pull down enabled
	UCHAR SerNumEnable7;		// non-zero if serial number to be used
	UCHAR ALSlowSlew;			// non-zero if AL pins have slow slew
	UCHAR ALSchmittInput;		// non-zero if AL pins are Schmitt input
	UCHAR ALDriveCurrent;		// valid values are 4mA, 8mA, 12mA, 16mA
	UCHAR AHSlowSlew;			// non-zero if AH pins have slow slew
	UCHAR AHSchmittInput;		// non-zero if AH pins are Schmitt input
	UCHAR AHDriveCurrent;		// valid values are 4mA, 8mA, 12mA, 16mA
	UCHAR BLSlowSlew;			// non-zero if BL pins have slow slew
	UCHAR BLSchmittInput;		// non-zero if BL pins are Schmitt input
	UCHAR BLDriveCurrent;		// valid values are 4mA, 8mA, 12mA, 16mA
	UCHAR BHSlowSlew;			// non-zero if BH pins have slow slew
	UCHAR BHSchmittInput;		// non-zero if BH pins are Schmitt input
	UCHAR BHDriveCurrent;		// valid values are 4mA, 8mA, 12mA, 16mA
	UCHAR IFAIsFifo7;			// non-zero if interface is 245 FIFO
	UCHAR IFAIsFifoTar7;		// non-zero if interface is 245 FIFO CPU target
	UCHAR IFAIsFastSer7;		// non-zero if interface is Fast serial
	UCHAR AIsVCP7;				// non-zero if interface is to use VCP drivers
	UCHAR IFBIsFifo7;			// non-zero if interface is 245 FIFO
	UCHAR IFBIsFifoTar7;		// non-zero if interface is 245 FIFO CPU target
	UCHAR IFBIsFastSer7;		// non-zero if interface is Fast serial
	UCHAR BIsVCP7;				// non-zero if interface is to use VCP drivers
	UCHAR PowerSaveEnable;		// non-zero if using BCBUS7 to save power for self-powered designs
	//
	// Rev 8 (FT4232H) Extensions
	//
	UCHAR PullDownEnable8;		// non-zero if pull down enabled
	UCHAR SerNumEnable8;		// non-zero if serial number to be used
	UCHAR ASlowSlew;			// non-zero if A pins have slow slew
	UCHAR ASchmittInput;		// non-zero if A pins are Schmitt input
	UCHAR ADriveCurrent;		// valid values are 4mA, 8mA, 12mA, 16mA
	UCHAR BSlowSlew;			// non-zero if B pins have slow slew
	UCHAR BSchmittInput;		// non-zero if B pins are Schmitt input
	UCHAR BDriveCurrent;		// valid values are 4mA, 8mA, 12mA, 16mA
	UCHAR CSlowSlew;			// non-zero if C pins have slow slew
	UCHAR CSchmittInput;		// non-zero if C pins are Schmitt input
	UCHAR CDriveCurrent;		// valid values are 4mA, 8mA, 12mA, 16mA
	UCHAR DSlowSlew;			// non-zero if D pins have slow slew
	UCHAR DSchmittInput;		// non-zero if D pins are Schmitt input
	UCHAR DDriveCurrent;		// valid values are 4mA, 8mA, 12mA, 16mA
	UCHAR ARIIsTXDEN;			// non-zero if port A uses RI as RS485 TXDEN
	UCHAR BRIIsTXDEN;			// non-zero if port B uses RI as RS485 TXDEN
	UCHAR CRIIsTXDEN;			// non-zero if port C uses RI as RS485 TXDEN
	UCHAR DRIIsTXDEN;			// non-zero if port D uses RI as RS485 TXDEN
	UCHAR AIsVCP8;				// non-zero if interface is to use VCP drivers
	UCHAR BIsVCP8;				// non-zero if interface is to use VCP drivers
	UCHAR CIsVCP8;				// non-zero if interface is to use VCP drivers
	UCHAR DIsVCP8;				// non-zero if interface is to use VCP drivers
	//
	// Rev 9 (FT232H) Extensions
	//
	UCHAR PullDownEnableH;		// non-zero if pull down enabled
	UCHAR SerNumEnableH;		// non-zero if serial number to be used
	UCHAR ACSlowSlewH;			// non-zero if AC pins have slow slew
	UCHAR ACSchmittInputH;		// non-zero if AC pins are Schmitt input
	UCHAR ACDriveCurrentH;		// valid values are 4mA, 8mA, 12mA, 16mA
	UCHAR ADSlowSlewH;			// non-zero if AD pins have slow slew
	UCHAR ADSchmittInputH;		// non-zero if AD pins are Schmitt input
	UCHAR ADDriveCurrentH;		// valid values are 4mA, 8mA, 12mA, 16mA
	UCHAR Cbus0H;				// Cbus Mux control
	UCHAR Cbus1H;				// Cbus Mux control
	UCHAR Cbus2H;				// Cbus Mux control
	UCHAR Cbus3H;				// Cbus Mux control
	UCHAR Cbus4H;				// Cbus Mux control
	UCHAR Cbus5H;				// Cbus Mux control
	UCHAR Cbus6H;				// Cbus Mux control
	UCHAR Cbus7H;				// Cbus Mux control
	UCHAR Cbus8H;				// Cbus Mux control
	UCHAR Cbus9H;				// Cbus Mux control
	UCHAR IsFifoH;				// non-zero if interface is 245 FIFO
	UCHAR IsFifoTarH;			// non-zero if interface is 245 FIFO CPU target
	UCHAR IsFastSerH;			// non-zero if interface is Fast serial
	UCHAR IsFT1248H;			// non-zero if interface is FT1248
	UCHAR FT1248CpolH;			// FT1248 clock polarity - clock idle high (1) or clock idle low (0)
	UCHAR FT1248LsbH;			// FT1248 data is LSB (1) or MSB (0)
	UCHAR FT1248FlowControlH;	// FT1248 flow control enable
	UCHAR IsVCPH;				// non-zero if interface is to use VCP drivers
	UCHAR PowerSaveEnableH;		// non-zero if using ACBUS7 to save power for self-powered designs

} FT_PROGRAM_DATA, * PFT_PROGRAM_DATA;

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

#define MAX_NUMBER_OF_USB_DEVICES 127
#define MAX_LOCATION_ID_LENGTH 21
#define MAX_USB_LAYERS 7
#define LIST_DEVICE_MODE_PRINT		0
#define LIST_DEVICE_MODE_OUTPUT		1

int ft_init(struct ftdi_info* ftdi);
int ft_open_channel(struct ftdi_info* fi, int channel);
int ft_open_channel_by_id(struct ftdi_info* fi, int channel, char* id);
int ft_close(struct ftdi_info* ftdi);
int ft_set_bitmode(struct ftdi_info* ftdi, int mask, int mode);
int ft_write(struct ftdi_info* ftdi, unsigned char* buffer, int size);
int ft_read(struct ftdi_info* ftdi, unsigned char* buffer, int size);
int ft_read_pins(struct ftdi_info* ftdi, unsigned char* pins);
int ft_clear_buffer(struct ftdi_info* ftdi);
void ft_list_devices(char location_str[][MAX_LOCATION_ID_LENGTH], int *board_num, int mode);
void msleep(int duration);
int ft_erase_eeprom(struct ftdi_info* ftdi);
int ft_write_eeprom(struct ftdi_info* ftdi, unsigned int startaddr, unsigned char* buffer, int size, unsigned char* sn_buf);
int ft_read_eeprom(struct ftdi_info* ftdi, unsigned int startaddr, unsigned char* data_buf, int data_size, unsigned char* sn_buf);

extern char GV_LOCATION_ID[MAX_LOCATION_ID_LENGTH];

#endif