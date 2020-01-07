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
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#endif

#ifdef linux
#include <unistd.h>
#include <ftdi.h>
#include <libusb.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "port.h"

#define MAX_NUMBER_OF_USB_DEVICES 127
#define MAX_LOCATION_ID_LENGTH 21
#define MAX_USB_LAYERS 7
char GV_LOCATION_ID[MAX_LOCATION_ID_LENGTH] = "";

int ft_init(struct ftdi_info* ftdi)
{
#ifdef _WIN32
	HINSTANCE hGetProcIDDLL = LoadLibrary("ftd2xx.dll");

	if (!hGetProcIDDLL) {
		printf("loading process failed\n");
		return -1;
	}
	ftdi->FT_open = (pFT_open)GetProcAddress(hGetProcIDDLL, "FT_Open");
	ftdi->FT_open_ex = (pFT_open_ex)GetProcAddress(hGetProcIDDLL, "FT_OpenEx");

	if (!ftdi->FT_open) {
		printf("ft_init: load failed\n");
		return -1;
	}
	ftdi->FT_set_bitmode = (pFT_set_bitmode)GetProcAddress(hGetProcIDDLL, "FT_SetBitMode");
	ftdi->FT_write = (pFT_write)GetProcAddress(hGetProcIDDLL, "FT_Write");
	ftdi->FT_read = (pFT_read)GetProcAddress(hGetProcIDDLL, "FT_Read");
	ftdi->FT_close = (pFT_close)GetProcAddress(hGetProcIDDLL, "FT_Close");
	ftdi->FT_create_device_info_list = (pFT_create_device_info_list)GetProcAddress(hGetProcIDDLL, "FT_CreateDeviceInfoList");
	ftdi->FT_get_device_info_detail = (pFT_get_device_info_detail)GetProcAddress(hGetProcIDDLL, "FT_GetDeviceInfoDetail");
	ftdi->FT_get_bitmode = (pFT_get_bitmode)GetProcAddress(hGetProcIDDLL, "FT_GetBitMode");
	ftdi->FT_set_timeouts = (pFT_set_timeouts)GetProcAddress(hGetProcIDDLL, "FT_SetTimeouts");

#endif
	return 0;
}

enum d2xx_open_method
{
	FT_OPEN_BY_SERIAL_NUMBER = 1,
	FT_OPEN_BY_DESCRIPTION = 2,
	FT_OPEN_BY_LOCATION = 4
};

/*issue: need to consider index  number, vender id, and product id*/
int ft_open_channel(struct ftdi_info* fi, int channel)
{
#ifdef _WIN32
	//char channel_serial_number
	//if(channel==0)
	//	ftdi-FT_open_ex();

	int status;
	//printf("opening channel %d",channel);

	if (channel == 0)
		status = fi->FT_open_ex("A", FT_OPEN_BY_SERIAL_NUMBER, &fi->ftdi);
	else if (channel == 1)
		status = fi->FT_open_ex("B", FT_OPEN_BY_SERIAL_NUMBER, &fi->ftdi);
	else if (channel == 2)
		status = fi->FT_open_ex("C", FT_OPEN_BY_SERIAL_NUMBER, &fi->ftdi);
	else if (channel == 3)
		status = fi->FT_open_ex("D", FT_OPEN_BY_SERIAL_NUMBER, &fi->ftdi);
	else
	{
		status = -1;
		printf("open channel can only range from 0 to 3");

	}
	fi->FT_set_timeouts(fi->ftdi, 10, 10);
	//status = fi->FT_open_ex(0x192, FT_OPEN_BY_LOCATION, &fi->ftdi);
	return status;

	//return fi->FT_open(1, &(fi->ftdi));

#else
	/*
	fi->ftdi = ftdi_new();
	int status=ftdi_set_interface(fi->ftdi, channel+1);
	status=ftdi_usb_open_desc_index(fi->ftdi, VENDOR_ID, PRODUCT_ID,NULL,NULL, 0);
	//status=ftdi_usb_open_bus_addr(ftdi->ftdi, 2,34);
	//printf("ftdi open status:%d\n", status);
	*/
	int status = ft_open_channel_by_id(fi, channel, NULL);
	return status;
#endif
}

int ft_close(struct ftdi_info* fi)
{
#ifdef _WIN32
	int num = fi->FT_close(fi->ftdi);
	//printf("%d\n",num);
	return num;
#else
	ftdi_usb_close(fi->ftdi);
	ftdi_free(fi->ftdi);
	return 0;
#endif
}

int ft_set_bitmode(struct ftdi_info* ftdi, int mask, int mode)
{
#ifdef _WIN32
	return ftdi->FT_set_bitmode(ftdi->ftdi, mask, mode);
#else
	return ftdi_set_bitmode(ftdi->ftdi, mask, mode);
#endif
}

/*write 'size' byte of data store in 'buffer' to the ftdi chip*/
int ft_write(struct ftdi_info* ftdi, unsigned char* buffer, int size)
{
#ifdef _WIN32
	int bytes_written;
	int status = ftdi->FT_write(ftdi->ftdi, buffer, size, &bytes_written);
	if (bytes_written <= 0)
		return -1;
	else
		return status;
#else
	return ftdi_write_data(ftdi->ftdi, (const unsigned char*)buffer, size);
#endif
}

int ft_read(struct ftdi_info* ftdi, unsigned char* buffer, int size)
{
#ifdef _WIN32
	int bytes_read;
	int status = ftdi->FT_read(ftdi->ftdi, buffer, size, &bytes_read);
	if (bytes_read <= 0)
		return -1;
	else
		return status;
#else
	return ftdi_read_data(ftdi->ftdi, (unsigned char*)buffer, size);
#endif
}

int ft_clear_buffer(struct ftdi_info* ftdi)
{
#ifdef _WIN32
	printf("clear buffer is not yet implemented at Windows\n");
	return -1;
#else
	int num1 = 0;
	int num2 = 0;
	//num1=ftdi_usb_purge_rx_buffer(ftdi->ftdi);
	num2 = ftdi_usb_purge_tx_buffer(ftdi->ftdi);
	if (num1 != 0 || num2 != 0)
	{
		printf("clear buffer failed!\n");
	}
	return 0;
#endif
}

void msleep(int duration)
{
	if (duration < 1)
		return;

#ifdef _WIN32
	Sleep(duration);
#endif
#ifdef __linux__
	usleep(duration * 1000);
#endif
}

void ft_list_devices()
{
#ifdef _WIN32
	struct ftdi_info ft;
	ft_init(&ft);
	int ftStatus;
	FT_HANDLE ftHandleTemp;
	DWORD numDevs;
	DWORD Flags;
	DWORD ID;
	DWORD Type;
	DWORD loc_id;
	char SerialNumber[16];
	char Description[64]; // create the device information list
	DWORD board_table[MAX_NUMBER_OF_USB_DEVICES][5]; //used for recording the location id of the board
	char location_id_str[MAX_NUMBER_OF_USB_DEVICES][MAX_LOCATION_ID_LENGTH];
	int detected_boards = 0;

	ftStatus = ft.FT_create_device_info_list(&numDevs);

	if (ftStatus != 0)
	{
		//printf("number of channel connected through FTDI device found: %d\n",numDevs);
		return;
	}
	if (numDevs > 0)
	{
		for (unsigned int i = 0; i < numDevs; i++)
		{
			ftStatus = ft.FT_get_device_info_detail(i, &Flags, &Type, &ID, &loc_id, SerialNumber, Description, &ftHandleTemp);
			if (ftStatus != 0)
			{
				return;
				//printf("Dev %d:\n", i);
				//printf("  Flags=0x%x\n",Flags);
				//printf("  Type=0x%x\n",Type);
				//printf("  ID=0x%x\n",ID);
				//printf("loc_id=0x%x\n",loc_id);
				//printf("board[%d] location_id=%x\n",i,loc_id);
				//printf("  SerialNumber=%s\n",SerialNumber);
				//printf("  Description=%s\n",Description);
				//printf("  ftHandle=0x%x\n",ftHandleTemp);
			}
			int found = 0;
			for (int k = 0; k < detected_boards; k++)
			{
				if (abs(loc_id - board_table[k][4]) < 4)
				{
					found = 1;
					if (strcmp("A", SerialNumber) == 0) {
						board_table[k][0] = loc_id;
						sprintf(location_id_str[k], "%x", loc_id);
					}
					else if (strcmp("B", SerialNumber) == 0)
						board_table[k][1] = loc_id;
					else if (strcmp("C", SerialNumber) == 0)
						board_table[k][2] = loc_id;
					else if (strcmp("D", SerialNumber) == 0)
						board_table[k][3] = loc_id;
					else
						printf("detected channel information are not found!\n");
				}
			}
			if (!found)
			{
				//printf("new board!\n");
				board_table[detected_boards][4] = loc_id;
				if (strcmp("A", SerialNumber) == 0) {
					board_table[detected_boards][0] = loc_id;
					sprintf(location_id_str[detected_boards], "%x", loc_id);
				}
				else if (strcmp("B", SerialNumber) == 0)
					board_table[detected_boards][1] = loc_id;
				else if (strcmp("C", SerialNumber) == 0)
					board_table[detected_boards][2] = loc_id;
				else if (strcmp("D", SerialNumber) == 0)
					board_table[detected_boards][3] = loc_id;
				else
					printf("detected channel information are not found!\n");
				detected_boards++;
			}
		}
		printf("number of boards connected through FTDI device found: %d\n", detected_boards);

		for (int j = 0; j < detected_boards; j++)
		{
			printf("board [%d] location_id=%s\n", j, location_id_str[j]);
			//printf("id: %s\n", location_id_str[j]);
			//printf("A: %x\n", board_table[j][0]);
			//printf("B: %x\n", board_table[j][1]);
			//printf("C: %x\n", board_table[j][2]);
			//printf("D: %x\n", board_table[j][3]);
			//printf("X: %x\n", board_table[j][4]);
		}
	}
	return;
#else
	int ret, i;
	struct ftdi_context* ftdi;
	struct ftdi_device_list* devlist, * curdev;
	char manufacturer[128], description[128];
	int retval = 0;

	if ((ftdi = ftdi_new()) == 0)
	{
		fprintf(stderr, "ftdi_new failed\n");
		//return -1;
	}

	if ((ret = ftdi_usb_find_all(ftdi, &devlist, 0, 0)) < 0)
	{
		fprintf(stderr, "ftdi_usb_find_all failed: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		retval = -1;
		goto do_deinit;
	}

	printf("number of boards connected through FTDI device found: %d\n", ret);

	char location_id[MAX_NUMBER_OF_USB_DEVICES][MAX_LOCATION_ID_LENGTH];
	struct libusb_device* dev_ptrs[MAX_NUMBER_OF_USB_DEVICES];
	memset(location_id, 0, sizeof(location_id));

	i = 0;
	for (curdev = devlist; curdev != NULL; i++)
	{
		dev_ptrs[i] = curdev->dev;
		int max_layers = 8;
		uint8_t port_number[8];
		for (int j = 0; j < max_layers; j++)
		{
			port_number[j] = 0;
		}

		int number_of_usb_device = libusb_get_port_numbers(curdev->dev, port_number, sizeof(port_number));

		int bus = libusb_get_bus_number(curdev->dev);
		char busstr[5];
		sprintf(busstr, "%d", bus);
		strcat(location_id[i], busstr);
		strcat(location_id[i], "-");

		for (int j = 0; j < number_of_usb_device; j++)
		{
			char numstr[3];
			sprintf(numstr, "%d", port_number[j]);
			strcat(location_id[i], numstr);
			if ((j + 1) < number_of_usb_device)
			{
				strcat(location_id[i], ".");
			}
		}
		printf("board[%d] location_id=%s\n", i, location_id[i]);

		/*
		if ((ret = ftdi_usb_get_strings(ftdi, curdev->dev, manufacturer, 128, description, 128, NULL, 0)) < 0)
		{
			fprintf(stderr, "ftdi_usb_get_strings failed: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
			retval = -1;
			goto done;
		}
		printf("Manufacturer: %s, Description: %s\n\n", manufacturer, description);
		*/
		curdev = curdev->next;
	}
done:
	ftdi_list_free(&devlist);
do_deinit:
	ftdi_free(ftdi);

	return;
#endif

}

int ft_read_pins(struct ftdi_info* ftdi, unsigned char* pins)
{
#ifdef _WIN32
	return ftdi->FT_get_bitmode(ftdi->ftdi, pins);
#else
	return ftdi_read_pins(ftdi->ftdi, pins);
#endif
}

int ft_open_channel_by_id(struct ftdi_info* fi, int channel, char* id)
{
#ifdef _WIN32
	struct ftdi_info ft;
	ft_init(&ft);
	int ftStatus;
	FT_HANDLE ftHandleTemp;
	DWORD numDevs;
	DWORD Flags;
	DWORD ID;
	DWORD Type;
	DWORD loc_id;
	char SerialNumber[16];
	char Description[64]; // create the device information list
	INT64 board_table[MAX_NUMBER_OF_USB_DEVICES][5]; //used for recording the location id of the board
	char location_id_str[MAX_NUMBER_OF_USB_DEVICES][MAX_LOCATION_ID_LENGTH];
	int detected_boards = 0;

	ftStatus = ft.FT_create_device_info_list(&numDevs);

	if (ftStatus != 0)
	{
		printf("failed to obtain device info list\n");
	}
	if (numDevs > 0)
	{
		for (unsigned int i = 0; i < numDevs; i++)
		{
			ftStatus = ft.FT_get_device_info_detail(i, &Flags, &Type, &ID, &loc_id, SerialNumber, Description, &ftHandleTemp);
			if (ftStatus != 0)
			{
				printf("fail to get device information\n");
				return -1;
			}
			int found = 0;
			for (int k = 0; k < detected_boards; k++)
			{
				if (abs(loc_id - (int)board_table[k][4]) < 4)
				{
					found = 1;
					if (strcmp("A", SerialNumber) == 0) {
						board_table[k][0] = loc_id;
						sprintf(location_id_str[k], "%x", loc_id);
					}

					else if (strcmp("B", SerialNumber) == 0)
						board_table[k][1] = loc_id;
					else if (strcmp("C", SerialNumber) == 0)
						board_table[k][2] = loc_id;
					else if (strcmp("D", SerialNumber) == 0)
						board_table[k][3] = loc_id;
					else
						printf("detected channel information are not found!\n");
				}
			}
			if (!found)
			{
				board_table[detected_boards][4] = loc_id;
				if (strcmp("A", SerialNumber) == 0)
				{
					sprintf(location_id_str[detected_boards], "%x", loc_id);
					board_table[detected_boards][0] = loc_id;
				}
				else if (strcmp("B", SerialNumber) == 0)
					board_table[detected_boards][1] = loc_id;
				else if (strcmp("C", SerialNumber) == 0)
					board_table[detected_boards][2] = loc_id;
				else if (strcmp("D", SerialNumber) == 0)
					board_table[detected_boards][3] = loc_id;
				else
					printf("detected channel information are not found!\n");
				detected_boards++;
			}
		}
	}
	for (int j = 0; j < detected_boards; j++)
	{
		if (strcmp(location_id_str[j], id) == 0)
		{
			int status;
			//printf("opening channel %d",channel);

			if (channel == 0)
				status = fi->FT_open_ex((PVOID)board_table[j][0], FT_OPEN_BY_LOCATION, &fi->ftdi);
			else if (channel == 1)
				status = fi->FT_open_ex((PVOID)board_table[j][1], FT_OPEN_BY_LOCATION, &fi->ftdi);
			else if (channel == 2)
				status = fi->FT_open_ex((PVOID)board_table[j][2], FT_OPEN_BY_LOCATION, &fi->ftdi);
			else if (channel == 3)
				status = fi->FT_open_ex((PVOID)board_table[j][3], FT_OPEN_BY_LOCATION, &fi->ftdi);
			else
			{
				status = -1;
				printf("open channel can only range from 0 to 3\n");
			}

			//status = fi->FT_open_ex(0x192, FT_OPEN_BY_LOCATION, &fi->ftdi);
			return status;
		}
	}

	printf("entered location id can not be found\n");
	return -1;

#else

	int number_of_ftdi, i;
	fi->ftdi = ftdi_new();

	struct ftdi_device_list* devlist, * curdev;
	if (fi->ftdi == NULL)
	{
		printf("ftdi_new failed!\n");
		return -1;
	}

	if ((number_of_ftdi = ftdi_usb_find_all(fi->ftdi, &devlist, 0, 0)) < 0)
	{
		printf("ftdi_usb_find_all failed!\n");
		return -1;
	}

	if (number_of_ftdi == 0)
	{
		printf("no ftdi device found! please make sure the device is connnected to the computer\n");
		ftdi_free(fi->ftdi);
		ftdi_list_free(&devlist);
		return -1;
	}
	char location_id[MAX_NUMBER_OF_USB_DEVICES][MAX_LOCATION_ID_LENGTH];
	struct libusb_device* dev_ptrs[MAX_NUMBER_OF_USB_DEVICES];
	memset(location_id, 0, sizeof(location_id));

	i = 0;
	int found = 0;
	for (curdev = devlist; curdev != NULL; i++)
	{
		dev_ptrs[i] = curdev->dev;
		int max_layers = MAX_USB_LAYERS;
		uint8_t port_number[MAX_USB_LAYERS];
		int path_length = libusb_get_port_numbers(curdev->dev, port_number, sizeof(port_number));

		int bus = libusb_get_bus_number(curdev->dev);
		char busstr[5];
		sprintf(busstr, "%d", bus);
		strcat(location_id[i], busstr);
		strcat(location_id[i], "-");

		for (int j = 0; j < path_length; j++)
		{
			char numstr[4];
			sprintf(numstr, "%d", port_number[j]);
			strcat(location_id[i], numstr);
			if ((j + 1) < path_length)
			{
				strcat(location_id[i], ".");
			}
		}
		if (id == NULL || strcmp(location_id[i], id) == 0)
		{
			printf("found device specified\n");
			found = 1;
			int status = ftdi_set_interface(fi->ftdi, channel + 1);
			status = ftdi_usb_open_dev(fi->ftdi, curdev->dev);
			break;
		}

		curdev = curdev->next;
	}

	ftdi_list_free(&devlist);

	if (!found) {
		printf("entered location id not found\n");
		ftdi_free(fi->ftdi);

		return -1;
	}
	else
		return 0;

#endif
}
