/*
* Copyright 2019-2021 NXP.
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
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#include "bcu_parser.h"
#include "chip.h"
#include "port.h"
#include "ftdi_def.h"

#define MAX_FTDI_BUFFER_SIZE 1024

extern char GV_LOCATION_ID[];

struct name_and_init_func chip_list[] =
{
	"pca9548", pca9548_create,
	"ft4232h_i2c", ft4232h_i2c_create,
	"ft4232h_gpio", ft4232h_gpio_create,
	"ft4232h_eeprom", ft4232h_eeprom_create,
	"pac1934", pac1934_create,
	"pca6408a", pca6408a_create,
	"pca6416a", pca6416a_create,
	"pca9655e", pca6416a_create,
	"pcal6524h", pcal6524h_create,
	"pca9670", pca9670_create,
	"adp5585", adp5585_create,
	"at24cxx", at24cxx_create,
	"pct2075", pct2075_create,
};
int num_of_chips = sizeof(chip_list) / sizeof(struct name_and_init_func);

//////////////////////////PCT2075//////////////////////////////////
void* pct2075_create(char* chip_specification, void* parent)
{
	struct pct2075* temp = (struct pct2075*)malloc(sizeof(struct pct2075));

	if (temp == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}
	temp->temp_dev.device.parent = parent;
	temp->temp_dev.temp_read = temp_read;
	temp->temp_dev.temp_enable = temp_enable;
	temp->addr = extract_parameter_value(chip_specification, "addr");
	//printf("AT24CXX created!\n");
	return temp;
}

float temp_read(void* pct2075)
{
	struct pct2075* temp = pct2075;
	struct i2c_device* parent = (void*)temp->temp_dev.device.parent;
	char addr_plus_write = (temp->addr) << 1;
	char addr_plus_read = (temp->addr << 1) + 1;
	int status, i = 0;
	unsigned char data_buffer[2] = {0};
	short data = 0;

	parent->i2c_start(parent);
	status = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_TEMP);
	if (status)
	{
		// printf("oh no! no ack received!\n");
		return -10;
	}
	parent->i2c_write(parent, 0x00, I2C_TYPE_TEMP);
	parent->i2c_start(parent);
	parent->i2c_write(parent, addr_plus_read, I2C_TYPE_TEMP);
	parent->i2c_read(parent, &data_buffer[0], 0, I2C_TYPE_TEMP);
	parent->i2c_read(parent, &data_buffer[1], 1, I2C_TYPE_TEMP);
	parent->i2c_stop(parent);

	data = (data_buffer[0] << 8 | data_buffer[1]) >> 5;

	if (data >= 1024)
		return (float)((data - 2048) / 8.0);
	else
		return (float)((data) / 8.0);

	return 0;
}

int temp_enable(void* pct2075, int enable)
{
	struct pct2075* temp = pct2075;
	struct i2c_device* parent = (void*)temp->temp_dev.device.parent;
	char addr_plus_write = (temp->addr) << 1;
	int status, i = 0;
	unsigned char data_buffer[2] = {0};
	short data = 0;

	parent->i2c_start(parent);
	status = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_TEMP);
	if (status)
	{
		// printf("oh no! no ack received!\n");
		return -10;
	}
	parent->i2c_write(parent, 0x01, I2C_TYPE_TEMP);
	parent->i2c_write(parent, enable ? 0x1 : 0x0, I2C_TYPE_TEMP);
	parent->i2c_stop(parent);

	return 0;
}

//////////////////////////AT24CXX//////////////////////////////////
void* at24cxx_create(char* chip_specification, void* parent)
{
	struct at24cxx* at24 = (struct at24cxx*)malloc(sizeof(struct at24cxx));
	if (at24 == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}
	at24->eeprom_device.device.parent = parent;
	at24->eeprom_device.eeprom_erase = at24cxx_erase;
	at24->eeprom_device.eeprom_read = at24cxx_read;
	at24->eeprom_device.eeprom_write = at24cxx_write;
	at24->eeprom_device.eeprom_check_board = at24cxx_check_board;
	at24->addr = extract_parameter_value(chip_specification, "addr");
	at24->type = extract_parameter_value(chip_specification, "type");
	//printf("AT24CXX created!\n");
	return at24;
}

int at24cxx_erase(void* at24cxx)
{
	unsigned char buf[256];

	for (int i = 0; i < 256; i++)
		buf[i] = 0xFF;

	return at24cxx_write(at24cxx, buf, 0, 64, NULL);
}

int at24cxx_read(void* at24cxx, unsigned char* data_buffer, unsigned int startaddr, int size, unsigned char* sn_buf)
{
	struct at24cxx* at24 = at24cxx;
	struct i2c_device* parent = (void*)at24->eeprom_device.device.parent;
	char addr_plus_write = (at24->addr) << 1;
	char addr_plus_read = (at24->addr << 1) + 1;
	int status, i = 0;

	parent->i2c_start(parent);
	status = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_AT24);
	if (status)
	{
		// printf("oh no! no ack received!\n");
		return -10;
	}
	if (at24->type == EEPROM_TYPE_AT24C32) {
		status = parent->i2c_write(parent, 0, I2C_TYPE_AT24);
		if (status)
			return -10;
	}
	status = parent->i2c_write(parent, startaddr, I2C_TYPE_AT24);
	if (status)
		return -10;
	parent->i2c_start(parent);
	status = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_AT24);
	if (status)
		return -10;
	for (i = 0; i < size - 1; i++)
		parent->i2c_read(parent, &data_buffer[i], 0, I2C_TYPE_AT24);
	parent->i2c_read(parent, &data_buffer[i], 1, I2C_TYPE_AT24);
	parent->i2c_stop(parent);

	msleep(10);

	return 0;
}

int at24cxx_write(void* at24cxx, unsigned char* data_buffer, unsigned int startaddr, int size, unsigned char* sn_buf)
{
	struct at24cxx* at24 = at24cxx;
	struct i2c_device* parent = (void*)at24->eeprom_device.device.parent;
	char addr_plus_write = (at24->addr) << 1;
	char addr_plus_read = (at24->addr << 1) + 1;
	int status, i = 0;

	parent->i2c_start(parent);
	status = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_AT24);
	if (status)
	{
		// printf("oh no! no ack received!\n");
		return -10;
	}
	if (at24->type == EEPROM_TYPE_AT24C32)
		parent->i2c_write(parent, 0, I2C_TYPE_AT24);
	parent->i2c_write(parent, startaddr, I2C_TYPE_AT24);
	for (i = 0; i < size; i++)
	{
		if (i != 0 && (startaddr + i) % 8 == 0)//AT24C02每8字节换页
		{
			parent->i2c_stop(parent);
			msleep(10);
			parent->i2c_start(parent);
			parent->i2c_write(parent, addr_plus_write, I2C_TYPE_AT24);
			if (at24->type == EEPROM_TYPE_AT24C32)
				parent->i2c_write(parent, 0, I2C_TYPE_AT24);
			parent->i2c_write(parent, startaddr + i, I2C_TYPE_AT24);
		}
		parent->i2c_write(parent, data_buffer[i], I2C_TYPE_AT24);
	}
	parent->i2c_stop(parent);

	msleep(10);

	return 0;
}

int at24cxx_check_board(void* at24cxx)
{
	struct at24cxx* at24 = at24cxx;
	struct i2c_device* parent = (void*)at24->eeprom_device.device.parent;
	char addr_plus_write = (at24->addr) << 1;
	char addr_plus_read = (at24->addr << 1) + 1;

	//refresh the powerister
	parent->i2c_start(parent);
	if(parent->i2c_write(parent, addr_plus_write, I2C_TYPE_AT24))
	{
		parent->i2c_stop(parent);
		// printf("pac 1934 failure get ack\n");
		return -1;
	}
	else
	{
		parent->i2c_stop(parent);
		return 0;
	}
}

//////////////////////////PCA9548//////////////////////////////////
void* pca9548_create(char* chip_specification, void* parent)
{
	struct pca9548* pca = (struct pca9548*)malloc(sizeof(struct pca9548));
	if (pca == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}
	pca->i2c_device.device.parent = parent;
	pca->i2c_device.i2c_read = pca9548_read;
	pca->i2c_device.i2c_readn = pca9548_readn;
	pca->i2c_device.i2c_write = pca9548_write;
	pca->i2c_device.i2c_start = pca9548_start;
	pca->i2c_device.i2c_stop = pca9548_stop;
	pca->channel = extract_parameter_value(chip_specification, "channel");
	pca->addr = extract_parameter_value(chip_specification, "addr");
	pca9548_set_channel(pca);
	//printf("pca9548 created!\n");
	return pca;
}

int pca9548_read(void* pca9548, unsigned char* data_buffer, int is_nack, int type)
{
	struct pca9548* pca = pca9548;
	struct i2c_device* parent = (void*)pca->i2c_device.device.parent;
	parent->i2c_read(parent, data_buffer, is_nack, type);
	return 0;
}

int pca9548_readn(void* pca9548, unsigned char* data_buffer, int type, int len)
{
	struct pca9548* pca = pca9548;
	struct i2c_device* parent = (void*)pca->i2c_device.device.parent;
	parent->i2c_readn(parent, data_buffer, type, len);
	return 0;
}

int pca9548_write(void* pca9548, unsigned char data, int type)
{
	struct pca9548* pca = pca9548;
	struct i2c_device* parent = (void*)pca->i2c_device.device.parent;
	return 	parent->i2c_write(parent, data, type);
}

int pca9548_start(void* pca9548)
{
	struct pca9548* pca = pca9548;
	struct i2c_device* parent = (void*)pca->i2c_device.device.parent;
	parent->i2c_start(parent);
	return 0;
}

int pca9548_stop(void* pca9548)
{
	struct pca9548* pca = pca9548;
	struct i2c_device* parent = (void*)pca->i2c_device.device.parent;
	parent->i2c_stop(parent);
	return 0;
}

int pca9548_set_channel(struct pca9548* pca9548)
{
	//my observation is that we must wait for 20ms from last set_channel to guarantee sucessfully switch
	//msleep(20);
	struct pca9548* pca = pca9548;
	struct i2c_device* parent = (void*)pca->i2c_device.device.parent;
	unsigned char addr_plus_write = pca->addr << 1;
	unsigned char change_channel_cmd = 1 << (pca->channel);

	int status;
	parent->i2c_start(parent);
	status = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_PCA9548);
	if (status)
	{
		printf("oh no! no ack received!\n");
	}
	parent->i2c_write(parent, change_channel_cmd, I2C_TYPE_PCA9548);
	parent->i2c_stop(parent);

	return 0;
}

////////////////////////////////ft4232h_eeprom///////////////////////////////////
struct ftdi_info g_ftdi_info[MAX_FT_I2C_CHANNEL_NUMBER];

void* ft4232h_eeprom_create(char* chip_specification, void* parent)
{
	int status = 0;
	struct ft4232h_eeprom* ftee = malloc(sizeof(struct ft4232h_eeprom));
	if (ftee == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}

	ftee->ftdi_info = &g_ftdi_info[0];
#ifdef _WIN32
	if (ftee->ftdi_info->ftdi == NULL && g_ftdi_info[1].ftdi)
		ftee->ftdi_info = &g_ftdi_info[1];
#endif
	ftee->eeprom_device.device.parent = parent;
	ftee->eeprom_device.eeprom_read = ft4232h_eeprom_read;
	ftee->eeprom_device.eeprom_write = ft4232h_eeprom_write;
	ftee->eeprom_device.eeprom_erase = ft4232h_eeprom_erase;
	ft_init(ftee->ftdi_info);
	
	if (!ftee->ftdi_info->isinit)
	{
		if (strlen(GV_LOCATION_ID) == 0) {
#if defined(__linux__) || defined(__APPLE__)
			status = ft_open_channel(ftee->ftdi_info, 0);
#else
			status = ft_open_channel(ftee->ftdi_info, 2);
#endif
		}
		else {
#if defined(__linux__) || defined(__APPLE__)
			status = ft_open_channel_by_id(ftee->ftdi_info, 0, GV_LOCATION_ID);
#else
			status = ft_open_channel_by_id(ftee->ftdi_info, 2, GV_LOCATION_ID);
#endif
		}
	}

	if (status != 0)
	{
		printf("failed to open ftdi device, err = %d\n", status);
#if defined(__linux__) || defined(__APPLE__)
		printf("***please make sure you run bcu with sudo\n");
#endif
		free(ftee);
		return NULL;
	}

	return ftee;
}

int ft4232h_eeprom_read(void* ft_eeprom, unsigned char* data_buffer, unsigned int startaddr, int size, unsigned char* sn_buf)
{
	struct ft4232h_eeprom* eeprom = ft_eeprom;
	struct ftdi_info *ftdi = eeprom->ftdi_info;

	return ft_read_eeprom(ftdi, startaddr, data_buffer, size, sn_buf);
}

int ft4232h_eeprom_write(void* ft_eeprom, unsigned char* data_buffer, unsigned int startaddr, int size, unsigned char* sn_buf)
{
	struct ft4232h_eeprom* eeprom = ft_eeprom;
	struct ftdi_info *ftdi = eeprom->ftdi_info;

	return ft_write_eeprom(ftdi, startaddr, data_buffer, size, sn_buf);
}

int ft4232h_eeprom_erase(void* ft_eeprom)
{
	struct ft4232h_eeprom* eeprom = ft_eeprom;
	struct ftdi_info *ftdi = eeprom->ftdi_info;

	return ft_erase_eeprom(ftdi);
}

////////////////////////////////ft4232H///////////////////////////////////
void* ft4232h_i2c_create(char* chip_specification, void* parent)
{
	struct ft4232h* ft = malloc(sizeof(struct ft4232h));
	if (ft == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}
	ft->i2c_device.device.parent = parent;
	ft->i2c_device.i2c_read = ft4232h_i2c_read;
	ft->i2c_device.i2c_readn = ft4232h_i2c_readn;
	ft->i2c_device.i2c_write = ft4232h_i2c_write;
	ft->i2c_device.i2c_start = ft4232h_i2c_start;
	ft->i2c_device.i2c_stop = ft4232h_i2c_stop;
	ft->i2c_device.device.free = ft4232h_i2c_free;
	ft->channel = extract_parameter_value(chip_specification, "channel");
	if (extract_parameter_value(chip_specification, "dir_bitmask") == -1)
	{
		printf("set dir_bitmask as default value 0x00\n");
		ft->dir_bitmask = 0x00;//default should be zero if parameter is not entered;
	}
	else
		ft->dir_bitmask = extract_parameter_value(chip_specification, "dir_bitmask");
	if (extract_parameter_value(chip_specification, "val_bitmask") == -1)
	{
		printf("set val_bitmask as default value 0x00\n");
		ft->val_bitmask = 0x00;//default should be zero if parameter is not entered;
	}
	else
		ft->val_bitmask = extract_parameter_value(chip_specification, "val_bitmask");

	ft->ftdi_info = &g_ftdi_info[ft->channel];
	for (int i = 0; i < MAX_FT_I2C_CHANNEL_NUMBER; i++)
	{
		if (g_ftdi_info[i].isinit & 0xF0)
		{
			g_ftdi_info[i].isinit = 0x1;
			if (ft->ftdi_info->isinit & 0xF)
				ft->ftdi_info->isinit = 0x10;
		}
	}

	if (!(ft->ftdi_info->isinit & 0xF))
	{
		if (!(ft->ftdi_info->isinit & 0xF0))
		{
			ft_init(ft->ftdi_info);
			int status = 0;
			if (strlen(GV_LOCATION_ID) == 0) {
				status = ft_open_channel(ft->ftdi_info, ft->channel + 1);
			}
			else {
				status = ft_open_channel_by_id(ft->ftdi_info, ft->channel + 1, GV_LOCATION_ID);
			}

			if (status != 0)
			{
				printf("failed to open ftdi device!\n");
#if defined(__linux__) || defined(__APPLE__)
				printf("***please make sure you run bcu with sudo\n");
#endif		
				free(ft);
				return NULL;
			}
		}
		ft4232h_i2c_init(ft);
		ft->ftdi_info->isinit = 0x1;
	}
	//printf("ft4232h created!\n");
	return ft;
}

void ft4232h_i2c_remove_all(int enable_1_exit_handler)
{
	int i;
	for (i = 0; i < MAX_FT_I2C_CHANNEL_NUMBER; i++)
	{
#ifdef _WIN32
		if (i == 1)
#else
		if (!enable_1_exit_handler && i == 1)
#endif
			continue;

		if (g_ftdi_info[i].isinit)
		{
			ft_close(&g_ftdi_info[i]);
			g_ftdi_info[i].isinit = 0;
		}
			// ft4232h_i2c_free(&g_ft[i]);
	}
}

int ft4232h_i2c_read(void* ft4232h, unsigned char* data_buffer, int is_nack, int type)
{
	struct ft4232h* ft = ft4232h;
	unsigned char buffer[MAX_FTDI_BUFFER_SIZE];
	unsigned char in_buffer[MAX_FTDI_BUFFER_SIZE];
	int i = 0, ftStatus = 0;

#ifdef _WIN32
	if (type == I2C_TYPE_GPIO)
#endif
		ft_clear_buffer(ft->ftdi_info);

	buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE;
	buffer[i++] = VALUE_SCLLOW_SDALOW | ft->val_bitmask;
	buffer[i++] = DIRECTION_SCLOUT_SDAIN | ft->dir_bitmask;

	buffer[i++] = MPSSE_CMD_DATA_IN_BITS_POS_EDGE;
	buffer[i++] = DATA_SIZE_8BITS;

	buffer[i++] = MPSSE_CMD_SEND_IMMEDIATE;


	buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE;
	buffer[i++] = VALUE_SCLLOW_SDALOW | ft->val_bitmask;
	buffer[i++] = DIRECTION_SCLOUT_SDAOUT | ft->dir_bitmask;

	buffer[i++] = MPSSE_CMD_DATA_OUT_BITS_NEG_EDGE;
	buffer[i++] = 0;// 0 means send one bit
	if (is_nack)
		buffer[i++] = 0x80;// MSB zero means ACK
	else
		buffer[i++] = 0x00;// MSB zero means ACK
	ftStatus = ft_write(ft->ftdi_info, buffer, i); //Send off the commands
	if (ftStatus < 0) return ftStatus;
	ftStatus = ft_read(ft->ftdi_info, in_buffer, 1); //Read one byte
	if (ftStatus < 0) return ftStatus;
	*data_buffer = in_buffer[0];

#ifdef _WIN32
	if (type == I2C_TYPE_GPIO)
#endif
		ft_clear_buffer(ft->ftdi_info);

	return 0;

}

int ft4232h_i2c_readn(void* ft4232h, unsigned char* data_buffer, int type, int len)
{
	struct ft4232h* ft = ft4232h;
	unsigned char buffer[MAX_FTDI_BUFFER_SIZE] = {0};
	unsigned char in_buffer[MAX_FTDI_BUFFER_SIZE] = {0};
	int i = 0, ftStatus = 0, j;

// #ifdef _WIN32
// 	if (type == I2C_TYPE_GPIO)
// #endif
// 		ft_clear_buffer(ft->ftdi_info);

	for (j = 0; j < len; j++)
	{
		buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE;
		buffer[i++] = VALUE_SCLLOW_SDALOW | ft->val_bitmask;
		buffer[i++] = DIRECTION_SCLOUT_SDAIN | ft->dir_bitmask;

		buffer[i++] = MPSSE_CMD_DATA_IN_BITS_POS_EDGE;
		buffer[i++] = DATA_SIZE_8BITS;

		buffer[i++] = MPSSE_CMD_SEND_IMMEDIATE;

		buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE;
		buffer[i++] = VALUE_SCLLOW_SDALOW | ft->val_bitmask;
		buffer[i++] = DIRECTION_SCLOUT_SDAOUT | ft->dir_bitmask;

		buffer[i++] = MPSSE_CMD_DATA_OUT_BITS_NEG_EDGE;
		buffer[i++] = 0;// 0 means send one bit
		if (j < len - 1)
			buffer[i++] = 0x00;// MSB zero means ACK
		else
			buffer[i++] = 0xFF;// MSB zero means ACK
	}

	ftStatus = ft_write(ft->ftdi_info, buffer, i); //Send off the commands
	if (ftStatus < 0)
		return ftStatus;
	us_sleep(500);
	ftStatus = ft_read(ft->ftdi_info, in_buffer, len); //Read one byte
	if (ftStatus < 0)
		return ftStatus;
	memcpy(data_buffer, in_buffer, len);

#ifdef _WIN32
	if (type == I2C_TYPE_GPIO)
#endif
		ft_clear_buffer(ft->ftdi_info);

	return 0;

}

int ft4232h_i2c_write(void* ft4232h, unsigned char data, int type)
{
	struct ft4232h* ft = ft4232h;
	unsigned char buffer[MAX_FTDI_BUFFER_SIZE];
	unsigned char in_buffer[MAX_FTDI_BUFFER_SIZE];
	int i = 0, ftStatus = 0;

	buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE; //Command to set directions of lower 8 pins and force value on bits set as output
	buffer[i++] = VALUE_SCLLOW_SDALOW | ft->val_bitmask;
	buffer[i++] = DIRECTION_SCLOUT_SDAOUT | ft->dir_bitmask;


	/* Command to write 8bits */
	buffer[i++] = MPSSE_CMD_DATA_OUT_BITS_NEG_EDGE;/* MPSSE command */
	buffer[i++] = DATA_SIZE_8BITS;
	buffer[i++] = data;

	/* Set SDA to input mode before reading ACK bit */
	buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
	buffer[i++] = VALUE_SCLLOW_SDALOW | ft->val_bitmask; /*Value*/
	buffer[i++] = DIRECTION_SCLOUT_SDAIN | ft->dir_bitmask; /*Direction*/

	/* Command to get ACK bit */
	buffer[i++] = MPSSE_CMD_DATA_IN_BITS_POS_EDGE;/* MPSSE command */
	buffer[i++] = DATA_SIZE_1BIT; /*Read only one bit */

	/*Command MPSSE to send data to PC immediately */
	buffer[i++] = MPSSE_CMD_SEND_IMMEDIATE;

	/*set direction*/
	buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE;/* MPSSE command */
	buffer[i++] = VALUE_SCLLOW_SDAHIGH | ft->val_bitmask; /*Value*/
	buffer[i++] = DIRECTION_SCLOUT_SDAOUT | ft->dir_bitmask; /*Direction*/
	ftStatus = ft_write(ft->ftdi_info, buffer, i); //Send off the commands
	if (ftStatus < 0) return ftStatus;
	ftStatus = ft_read(ft->ftdi_info, in_buffer, 1);
	if (ftStatus < 0) return ftStatus;

	if (((in_buffer[0] & 0x1) != 0)) //Check ACK bit 0 on data byte read out
	{
		// printf("can't get the ACK bit after write\n");
		return -1; /*Error, can't get the ACK bit */
	}

#ifdef _WIN32
	if (type == I2C_TYPE_GPIO)
#endif
		ft_clear_buffer(ft->ftdi_info);

	return 0;
}

int ft4232h_i2c_start(void* ft4232h)
{
	struct ft4232h* ft = ft4232h;
	unsigned char buffer[MAX_FTDI_BUFFER_SIZE];
	int i = 0, ftStatus = 0;
	for (int j = 0; j < 40; j++)
	{
		buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE;
		buffer[i++] = VALUE_SCLHIGH_SDAHIGH | ft->val_bitmask;
		buffer[i++] = DIRECTION_SCLOUT_SDAOUT | ft->dir_bitmask;
	}

	for (int j = 0; j < 20; j++)
	{
		buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE;
		buffer[i++] = VALUE_SCLHIGH_SDALOW | ft->val_bitmask;
		buffer[i++] = DIRECTION_SCLOUT_SDAOUT | ft->dir_bitmask;
	}

	buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE;
	buffer[i++] = VALUE_SCLLOW_SDALOW | ft->val_bitmask;
	buffer[i++] = DIRECTION_SCLOUT_SDAOUT | ft->dir_bitmask;

	ftStatus = ft_write(ft->ftdi_info, buffer, i); //Send off the commands
	if (ftStatus < 0)
		return ftStatus;

	return 0;
}

int ft4232h_i2c_stop(void* ft4232h)
{
	struct ft4232h* ft = ft4232h;
	unsigned char buffer[MAX_FTDI_BUFFER_SIZE];
	int i = 0, ftStatus = 0;

	/* SCL low, SDA low */
	for (int j = 0; j < 25; j++)
	{
		buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE;
		buffer[i++] = VALUE_SCLLOW_SDALOW | ft->val_bitmask;
		buffer[i++] = DIRECTION_SCLOUT_SDAOUT | ft->dir_bitmask;
	}
	/* SCL high, SDA low */
	for (int j = 0; j < 25; j++)
	{
		buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE;
		buffer[i++] = VALUE_SCLHIGH_SDALOW | ft->val_bitmask;
		buffer[i++] = DIRECTION_SCLOUT_SDAOUT | ft->dir_bitmask;
	}
	/* SCL high, SDA high */
	for (int j = 0; j < 25; j++)
	{
		buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE;
		buffer[i++] = VALUE_SCLHIGH_SDAHIGH | ft->val_bitmask;
		buffer[i++] = DIRECTION_SCLOUT_SDAOUT | ft->dir_bitmask;
	}
	//buffer[i++]=0x80;
	//buffer[i++]=0x00;
	//buffer[i++]=0x10; /* Tristate the SCL & SDA pins */
	ftStatus = ft_write(ft->ftdi_info, buffer, i); //Send off the commands
	if (ftStatus < 0)
		return ftStatus;

	return 0;
}

int ft4232h_i2c_init(struct ft4232h* ft)
{
	unsigned char buffer[MAX_FTDI_BUFFER_SIZE];
	int i = 0, ftStatus = 0;

	ft_set_bitmode(ft->ftdi_info, 0, 0); //resetting the controller
	ft_set_bitmode(ft->ftdi_info, 0, BM_MPSSE);//set as MPSSE

	buffer[i++] = MPSSE_CMD_DISABLE_CLOCK_DIVIDE_BY_5; //Ensure disable clock divide by 5 for 60Mhz master clock
	buffer[i++] = MPSSE_CMD_DISABLE_ADAPTIVE_CLOCKING; //Ensure turn off adaptive clocking
	buffer[i++] = MPSSE_CMD_ENABLE_3PHASE_CLOCKING; //PAC 1934 need it. Enable 3 phase data clock, used by I2C to allow data on one clock edges
	ftStatus = ft_write(ft->ftdi_info, buffer, i);
	if (ftStatus < 0)
		return ftStatus;

	i = 0; //Clear output buffer
	buffer[i++] = MPSSE_CMD_SET_DATA_BITS_LOWBYTE; //Command to set directions of lower 8 pins and force value on bits set as output
	buffer[i++] = VALUE_SCLHIGH_SDAHIGH | ft->val_bitmask;
	buffer[i++] = DIRECTION_SCLOUT_SDAOUT | ft->dir_bitmask;
	// The SK clock frequency can be worked out by below algorithm with divide by 5 set as off
	// SK frequency = 60MHz /((1 + [(1 +0xValueH*256) OR 0xValueL])*2)
	buffer[i++] = MPSSE_CMD_SET_CLOCK_DIVISOR; //Command to set clock divisor
	buffer[i++] = (unsigned char)(CLOCK_DIVISOR_400K & '\xFF'); //Set 0xValueL of clock divisor
	buffer[i++] = (unsigned char)((CLOCK_DIVISOR_400K >> 8) & '\xFF'); //Set 0xValueH of clock divisor

	ftStatus = ft_write(ft->ftdi_info, buffer, i);
	if (ftStatus < 0)
		return ftStatus;
	i = 0; //Clear output buffer
	msleep(20); //Delay for a while
	//Turn off loop back in case
	buffer[i++] = MPSEE_CMD_DISABLE_LOOPBACK; //Command to turn off loop back of TDI/TDO connection
	ftStatus = ft_write(ft->ftdi_info, buffer, i);
	if (ftStatus < 0)
		return ftStatus;
	msleep(30); //Delay for a while

	//printf("I2C initialization finished!\n");
	return 0;
}

int ft4232h_i2c_free(void* ft4232h)
{
	struct ft4232h* ft = ft4232h;
	// int ftStatus = 0;

	// ftStatus = ft_close(ft->ftdi_info);
	// if (ftStatus < 0)
	// 	return ftStatus;

	ft->ftdi_info = NULL;

	return 0;
}

///////////////////////////////ft4232H_GPIO/////////////////////////////////


void* ft4232h_gpio_create(char* chip_specification, void* parent)
{

	struct ft4232h_gpio* ft = malloc(sizeof(struct ft4232h_gpio));
	if (ft == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}
	ft->gpio_device.device.parent = parent;
	ft->gpio_device.gpio_read=ft4232h_gpio_read;
	ft->gpio_device.gpio_write = ft4232h_gpio_write;
	//ft->gpio_device.gpio_toggle=ft4232h_gpio_toggle;
	ft->gpio_device.device.free = ft4232h_gpio_free;
	ft->gpio_device.pin_bitmask = extract_parameter_value(chip_specification, "pin_bitmask");
	ft->channel = extract_parameter_value(chip_specification, "channel");

	ft->ftdi_info = &g_ftdi_info[ft->channel];
	if (!ft->ftdi_info->isinit)
	{
		ft_init(ft->ftdi_info);
		int status = 0;
		if (strlen(GV_LOCATION_ID) == 0) {
			status = ft_open_channel(ft->ftdi_info, ft->channel + 1);
		}
		else {
			status = ft_open_channel_by_id(ft->ftdi_info, ft->channel + 1, GV_LOCATION_ID);
		}

		if (status != 0)
		{
			printf("failed to open ftdi device!\n");
#if defined(__linux__) || defined(__APPLE__)
			printf("***please make sure you run bcu with sudo\n");
#endif			

			free(ft);
			return NULL;
		}
		ft->ftdi_info->isinit = ft->ftdi_info->isinit << 4;
	}

	if (ft->ftdi_info->isinit & 0xF)
	{
		ft_set_bitmode(ft->ftdi_info, 0, 0);
		ft->ftdi_info->isinit = ft->ftdi_info->isinit << 4;
	}

	return ft;
}


int ft4232h_gpio_write(void* ft4232h, unsigned char bit_value)
{
	struct ft4232h_gpio* ft = ft4232h;
	int mask = 0xFF;
	if (ft_set_bitmode(ft->ftdi_info, mask, BM_BITBANG) < 0)
		printf("failed to set bitmode\n");

	unsigned char current_output;
	ft_read_pins(ft->ftdi_info, &current_output);
	//printf("previous output: %x\n", current_output);

	unsigned char data;
	data = (current_output & (~ft->gpio_device.pin_bitmask)) | (bit_value & ft->gpio_device.pin_bitmask);

	if (ft_write(ft->ftdi_info, &data, 1) < 0)
	{
		printf("failed to write\n");
		return -1;
	}

	return 0;
}


int ft4232h_gpio_read(void* ft4232h, unsigned char* bit_value_buffer)
{
	struct ft4232h_gpio* ft = ft4232h;
	unsigned char current_output;

	ft_read_pins(ft->ftdi_info, &current_output);

	bit_value_buffer[0] = current_output & ft->gpio_device.pin_bitmask;

	return 0;
}

int ft4232h_gpio_toggle(void* ft4232h)
{
	printf("toggle is not yet implemented!\n");
	return 0;
}

int ft4232h_gpio_free(void* ft4232h)
{
	struct ft4232h_gpio* ft = ft4232h;
	// ft_close(ft->ftdi_info);
	ft->ftdi_info = NULL;
	return 0;
}

////////////////////////////////PAC1934///////////////////////////////////
void* pac1934_create(char* chip_specification, void* parent)
{
	struct pac1934* pac = (struct pac1934*)malloc(sizeof(struct pac1934));
	if (pac == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}
	pac->power_device.device.parent = parent;
	pac->power_device.power_get_addr = get_pac1934_addr;
	pac->power_device.power_get_group = get_pac1934_group;
	pac->power_device.power_get_sensor = get_pac1934_sensor;
	pac->power_device.power_get_cur_res = get_pac1934_cur_res;
	pac->power_device.power_get_unused_res = get_pac1934_unused_res;
	pac->power_device.power_set_hwfilter = pac1934_set_hwfilter;
	pac->power_device.power_set_bipolar = pac1934_set_bipolar;
	pac->power_device.power_write_bipolar = pac1934_write_bipolar;
	pac->power_device.power_write_no_skip = pac1934_write_no_skip;
	pac->power_device.power_set_snapshot = pac1934_snapshot;
	pac->power_device.power_get_data = pac1934_get_data;
	pac->power_device.switch_sensor = pac1934_switch;
	pac->group = extract_parameter_value(chip_specification, "group");
	pac->sensor = extract_parameter_value(chip_specification, "sensor");
	pac->addr = extract_parameter_value(chip_specification, "addr");
	pac->rs1 = extract_parameter_value(chip_specification, "rsense1");
	pac->group2 = extract_parameter_value(chip_specification, "group2");
	pac->sensor2 = extract_parameter_value(chip_specification, "sensor2");
	pac->rs2 = extract_parameter_value(chip_specification, "rsense2");
	pac->cur_group = pac->group;
	pac->cur_sensor = pac->sensor;
	pac->cur_rs = pac->rs1;
	//printf("pac1934 created!\n");
	return pac;
}

int pac1934_switch(void *pac1934, int i)
{
	struct pac1934* pac = pac1934;

	if (i == 0)
	{
		pac->cur_group = pac->group;
		pac->cur_sensor = pac->sensor;
		pac->cur_rs = pac->rs1;
	}
	else if (i == 1)
	{
		if(pac->group2 != -1)
			pac->cur_group = pac->group2;
		if(pac->sensor2 != -1)
			pac->cur_sensor = pac->sensor2;
		pac->cur_rs =  pac->rs2;
	}
	else
		return -1;

	return 0;
}

int get_pac1934_addr(void* pac1934)
{
	struct pac1934* pac = pac1934;

	return pac->addr;
}

int get_pac1934_group(void* pac1934)
{
	struct pac1934* pac = pac1934;

	return pac->cur_group;
}

int get_pac1934_sensor(void* pac1934)
{
	struct pac1934* pac = pac1934;

	return pac->cur_sensor;
}

int get_pac1934_cur_res(void* pac1934)
{
	struct pac1934* pac = pac1934;

	return pac->cur_rs;
}

int get_pac1934_unused_res(void* pac1934)
{
	struct pac1934* pac = pac1934;

	if (pac->rs1 == pac->cur_rs)
		return pac->rs2;
	else
		return pac->rs1;
}

void pac1934_set_hwfilter(void* pac1934, int onoff)
{
	struct pac1934* pac = pac1934;

	pac->hwfilter = onoff;
}

void pac1934_set_bipolar(void* pac1934, int value)
{
	struct pac1934* pac = pac1934;

	pac->bipolar = value;
}

int pac1934_write_bipolar(void* pac1934, int value)
{
	struct pac1934* pac = pac1934;
	struct i2c_device* parent = (void*)pac->power_device.device.parent;
	char addr_plus_write = (pac->addr) << 1;
	char addr_plus_read = (pac->addr << 1) + 1;

	parent->i2c_start(parent);
	if(parent->i2c_write(parent, addr_plus_write, I2C_TYPE_PAC1934))
	{
		printf("pac1934_write_bipolar: pac 1934 failure get ack, addr: 0x%x\n", pac->addr);
		return -1;
	};
	parent->i2c_write(parent, PAC1934_REG_NEG_PWR_ADDR, I2C_TYPE_PAC1934);
	if (value)
		parent->i2c_write(parent, 0xF0, I2C_TYPE_PAC1934);
	else
		parent->i2c_write(parent, 0x00, I2C_TYPE_PAC1934);
	parent->i2c_stop(parent);

	pac->bipolar = value;

	return 0;
}

int pac1934_write_no_skip(void* pac1934, int value)
{
	struct pac1934* pac = pac1934;
	struct i2c_device* parent = (void*)pac->power_device.device.parent;
	char addr_plus_write = (pac->addr) << 1;
	char addr_plus_read = (pac->addr << 1) + 1;

	parent->i2c_start(parent);
	if(parent->i2c_write(parent, addr_plus_write, I2C_TYPE_PAC1934))
	{
		printf("pac1934_write_no_skip: pac 1934 failure get ack, addr: 0x%x\n", pac->addr);
		return -1;
	};
	parent->i2c_write(parent, PAC1934_REG_CHANNEL_DIS_AND_SMBUS_ADDR, I2C_TYPE_PAC1934);
	if (value)
		parent->i2c_write(parent, 0x02, I2C_TYPE_PAC1934);
	else
		parent->i2c_write(parent, 0x00, I2C_TYPE_PAC1934);
	parent->i2c_stop(parent);

	pac->no_skip = value;

	return 0;
}

int pac1934_snapshot(void* pac1934)
{
	struct pac1934* pac = pac1934;
	struct i2c_device* parent = (void*)pac->power_device.device.parent;
	char addr_plus_write = (pac->addr) << 1;
	char addr_plus_read = (pac->addr << 1) + 1;

	//refresh the powerister
	parent->i2c_start(parent);
	if(parent->i2c_write(parent, addr_plus_write, I2C_TYPE_PAC1934))
	{
		printf("pac1934_snapshot: pac 1934 failure get ack, addr: 0x%x\n", pac->addr);
		return -1;
	};
	parent->i2c_write(parent, 0x00, I2C_TYPE_PAC1934); //refresh;
	parent->i2c_stop(parent);

	return 0;
}

double pac1934_reg2current(unsigned int reg, unsigned char complement)
{
	if (complement) { /* 2 complement representing values between -100 mV and 100 mV */
		if (reg >= 32768) {
			return ((double)reg - 65536) / 32768 * 100000;
		}
		else {
			return ((double)reg) / 32768 * 100000;
		}
	}
	else
		return ((double)reg) / 65536 * 100000;
}

int pac1934_get_data(void* pac1934, struct pac193x_reg_data* pac_reg)
{
	struct pac1934* pac = pac1934;
	struct i2c_device* parent = (void*)pac->power_device.device.parent;
	char addr_plus_write = (pac->addr) << 1;
	char addr_plus_read = (pac->addr << 1) + 1;
	// unsigned char ctrl[12];
	int k;
	#define DATA_LEN 17
	unsigned char data[DATA_LEN] = {0};
	unsigned char start_read_reg_base;

	int status = 0;

	if (pac->hwfilter)
		start_read_reg_base = PAC1934_REG_VBUS1_AVG_ADDR;
	else
		start_read_reg_base = PAC1934_REG_VBUS1_ADDR;

	parent->i2c_start(parent);
	status = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_PAC1934);
	if (status < 0)
		return status;
	parent->i2c_write(parent, start_read_reg_base, I2C_TYPE_PAC1934); //start get data;
	parent->i2c_start(parent);
	parent->i2c_write(parent, addr_plus_read, I2C_TYPE_PAC1934);
	// for(k = 0; k < DATA_LEN - 1; k++)
	// 	parent->i2c_read(parent, &data[k], 0, I2C_TYPE_PAC1934);
	// parent->i2c_read(parent, &data[DATA_LEN - 1], 1, I2C_TYPE_PAC1934);
	parent->i2c_readn(parent, data, I2C_TYPE_PAC1934, DATA_LEN);
	parent->i2c_stop(parent);

	// for(k=0; k<12; k++)
	// 	printf("ctrl[%d]=0x%x\n", k, ctrl[k]);
	// for(k=0; k<76; k++)
	// 	printf("data[%d]=0x%x\n", k, data[k]);

	// printf("voltage %d: %f\n", 1, ((double)(((data[0] << 8) + data[1]) * 32)) / (65535));
	// printf("voltage %d: %f\n", 2, ((double)(((data[2] << 8) + data[3]) * 32)) / (65535));
	// printf("voltage %d: %f\n", 3, ((double)(((data[4] << 8) + data[5]) * 32)) / (65535));
	// printf("voltage %d: %f\n", 4, ((double)(((data[6] << 8) + data[7]) * 32)) / (65535));
	pac_reg->vbus[0] = ((double)(((data[0] << 8) + data[1]) * 32)) / (65535);
	pac_reg->vbus[1] = ((double)(((data[2] << 8) + data[3]) * 32)) / (65535);
	pac_reg->vbus[2] = ((double)(((data[4] << 8) + data[5]) * 32)) / (65535);
	pac_reg->vbus[3] = ((double)(((data[6] << 8) + data[7]) * 32)) / (65535);

	// printf("current %d: %f\n", 1, (((double)(((data[8] << 8) + data[9]) * 1)) / (65535)) * 100000);
	// printf("current %d: %f\n", 2, (((double)(((data[10] << 8) + data[11]) * 1)) / (65535)) * 100000);
	// printf("current %d: %f\n", 3, (((double)(((data[12] << 8) + data[13]) * 1)) / (65535)) * 100000);
	// printf("current %d: %f\n", 4, (((double)(((data[14] << 8) + data[15]) * 1)) / (65535)) * 100000);
	pac_reg->vsense[0] = pac1934_reg2current((data[8] << 8) + data[9], pac->bipolar);
	pac_reg->vsense[1] = pac1934_reg2current((data[10] << 8) + data[11], pac->bipolar);
	pac_reg->vsense[2] = pac1934_reg2current((data[12] << 8) + data[13], pac->bipolar);
	pac_reg->vsense[3] = pac1934_reg2current((data[14] << 8) + data[15], pac->bipolar);

	return 0;
}

////////////////////////PCA6408A//////////////////////////////////

void* pca6408a_create(char* chip_specification, void* parent)
{
	struct pca6408a* pca = (struct pca6408a*)malloc(sizeof(struct pca6408a));
	if (pca == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}
	pca->gpio_device.device.parent = parent;
	pca->gpio_device.gpio_read = pca6408a_read;
	pca->gpio_device.gpio_write = pca6408a_write;
	pca->gpio_device.gpio_toggle = pca6408a_toggle;
	pca->gpio_device.gpio_get_output = pca6408a_get_output;
	pca->gpio_device.pin_bitmask = extract_parameter_value(chip_specification, "pin_bitmask");
	pca->gpio_device.opendrain = extract_parameter_value(chip_specification, "opendrain");
	pca->gpio_device.active_level = extract_parameter_value(chip_specification, "active_level");

	pca->addr = extract_parameter_value(chip_specification, "addr");

	return pca;
}

int pca6408a_write(void* pca6408a, unsigned char bit_value)
{
	struct pca6408a* pca = pca6408a;

	if (pca->gpio_device.opendrain > 0) {
		pca6408a_set_output(pca, ~pca->gpio_device.pin_bitmask);
		return pca6408a_set_direction(pca, bit_value);
	}

	pca6408a_set_direction(pca, ~pca->gpio_device.pin_bitmask);
	return pca6408a_set_output(pca, bit_value);
}
int pca6408a_set_output(struct pca6408a* pca6408a, unsigned char bit_value)
{
	struct pca6408a* pca = pca6408a;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;

	unsigned char output_cmd = 0x01; //x01h is the output command for port 0
	unsigned char current_output[1];
	int bSucceed = 0;

	//read current output register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char output_data = (current_output[0] & (~pca->gpio_device.pin_bitmask)) | (bit_value & pca->gpio_device.pin_bitmask);
	//read current input register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_data, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int pca6408a_get_direction(void* pca6408a, unsigned char* bit_value_buffer)
{
	struct pca6408a* pca = pca6408a;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	unsigned char input_cmd = 0x03; //x03h is the io configuration command
	int bSucceed = 0;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, bit_value_buffer, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	if (pca->gpio_device.opendrain <= 0)
		(*bit_value_buffer) = ~(*bit_value_buffer);

	//mask away unwanted value;
	*bit_value_buffer = (*bit_value_buffer) & (pca->gpio_device.pin_bitmask);
	return 0;
}

int pca6408a_read(void* pca6408a, unsigned char* bit_value_buffer)
{
	struct pca6408a* pca = pca6408a;
	if (pca->gpio_device.opendrain > 0)
		return pca6408a_get_direction(pca, bit_value_buffer);
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	unsigned char input_cmd = 0x00; //x00h is the input command
	int bSucceed = 0;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, bit_value_buffer, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	//mask away unwanted value;
	*bit_value_buffer = (*bit_value_buffer) & (pca->gpio_device.pin_bitmask);
	return 0;
}

int pca6408a_set_direction(struct pca6408a* pca, unsigned char value)
{
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;

	unsigned char configure_cmd = 0x03; //x03h is the io configuration command
	unsigned char current_config[1];
	int bSucceed = 0;

	//read current configuration register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, configure_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_config, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char input_bitmask = (value | (~pca->gpio_device.pin_bitmask)) & (current_config[0] | pca->gpio_device.pin_bitmask);

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, configure_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_bitmask, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int pca6408a_toggle(void* pca6408a)
{
	struct pca6408a* pca = pca6408a;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;

	//TODO: NEED SET DIRECTIONS FOR THE PINS FIRST HERE

	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;

	unsigned char output_cmd = 0x01; //x01h is the output command for port 0
	unsigned char current_output[1];
	int bSucceed = 0;

	//read current output register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char output_data = current_output[0] ^ pca->gpio_device.pin_bitmask;
	//read current input register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_data, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int pca6408a_get_output(void* pca6408a, unsigned char* current_output)
{
	struct pca6408a* pca = pca6408a;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	unsigned char output_cmd = 0x01; //x01h is the output command for port 0
	int bSucceed = 0;

	if (pca->gpio_device.opendrain > 0)
		output_cmd = 0x3;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	*current_output = (*current_output) & pca->gpio_device.pin_bitmask;

	return 0;
}

////////////////////////PCA6416A//////////////////////////////////

void* pca6416a_create(char* chip_specification, void* parent)
{
	struct pca6416a* pca = (struct pca6416a*)malloc(sizeof(struct pca6416a));
	if (pca == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}
	pca->gpio_device.device.parent = parent;
	pca->gpio_device.gpio_read = pca6416a_read;
	pca->gpio_device.gpio_write = pca6416a_write;
	pca->gpio_device.gpio_toggle = pca6416a_toggle;
	pca->gpio_device.gpio_get_output = pca6416a_get_output;
	pca->gpio_device.pin_bitmask = extract_parameter_value(chip_specification, "pin_bitmask");
	pca->gpio_device.opendrain = extract_parameter_value(chip_specification, "opendrain");
	pca->gpio_device.active_level = extract_parameter_value(chip_specification, "active_level");

	pca->addr = extract_parameter_value(chip_specification, "addr");
	pca->port = extract_parameter_value(chip_specification, "port");

	return pca;
}

int pca6416a_write(void* pca6416a, unsigned char bit_value)
{
	struct pca6416a* pca = pca6416a;

	if (pca->gpio_device.opendrain > 0) {
		pca6416a_set_output(pca, ~pca->gpio_device.pin_bitmask);
		return pca6416a_set_direction(pca, bit_value);
	}

	pca6416a_set_direction(pca, ~pca->gpio_device.pin_bitmask);
	return pca6416a_set_output(pca, bit_value);
}
int pca6416a_set_output(struct pca6416a* pca6416a, unsigned char bit_value)
{
	struct pca6416a* pca = pca6416a;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;

	unsigned char output_cmd = (pca->port) + 0x02; //x02h is the output command for port 0
	unsigned char current_output[1];
	int bSucceed = 0;

	//read current output register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char output_data = (current_output[0] & (~pca->gpio_device.pin_bitmask)) | (bit_value & pca->gpio_device.pin_bitmask);
	//read current input register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_data, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int pca6416a_get_direction(void* pca6416a, unsigned char* bit_value_buffer)
{
	struct pca6416a* pca = pca6416a;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	unsigned char input_cmd = (pca->port) + 0x06; //x06h is the io configuration command
	int bSucceed = 0;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, bit_value_buffer, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	if (pca->gpio_device.opendrain <= 0)
		(*bit_value_buffer) = ~(*bit_value_buffer);

	//mask away unwanted value;
	*bit_value_buffer = (*bit_value_buffer) & (pca->gpio_device.pin_bitmask);
	return 0;
}

int pca6416a_read(void* pca6416a, unsigned char* bit_value_buffer)
{
	struct pca6416a* pca = pca6416a;
	if (pca->gpio_device.opendrain > 0)
		return pca6416a_get_direction(pca, bit_value_buffer);
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	unsigned char input_cmd = (pca->port) + 0x00; //x00h is the input command
	int bSucceed = 0;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, bit_value_buffer, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	//mask away unwanted value;
	*bit_value_buffer = (*bit_value_buffer) & (pca->gpio_device.pin_bitmask);
	return 0;
}

int pca6416a_set_direction(struct pca6416a* pca, unsigned char value)
{
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;

	unsigned char configure_cmd = (pca->port) + 0x06; //x06h is the io configuration command
	unsigned char current_config[1];
	int bSucceed = 0;

	//read current configuration register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, configure_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_config, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char input_bitmask = (value | (~pca->gpio_device.pin_bitmask)) & (current_config[0] | pca->gpio_device.pin_bitmask);

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, configure_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_bitmask, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int pca6416a_toggle(void* pca6416a)
{
	struct pca6416a* pca = pca6416a;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;

	//TODO: NEED SET DIRECTIONS FOR THE PINS FIRST HERE

	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;

	unsigned char output_cmd = (pca->port) + 0x02; //x02h is the output command for port 0
	unsigned char current_output[1];
	int bSucceed = 0;

	//read current output register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char output_data = current_output[0] ^ pca->gpio_device.pin_bitmask;
	//read current input register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_data, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int pca6416a_get_output(void* pca6416a, unsigned char* current_output)
{
	struct pca6416a* pca = pca6416a;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	unsigned char output_cmd = (pca->port) + 0x02; //x02h is the output command for port 0
	int bSucceed = 0;

	if (pca->gpio_device.opendrain > 0)
                output_cmd = (pca->port) + 0x6;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	*current_output = (*current_output) & pca->gpio_device.pin_bitmask;

	return 0;
}

////////////////////////PCAL6524H//////////////////////////////////

void* pcal6524h_create(char* chip_specification, void* parent)
{
	struct pcal6524h* pca = (struct pcal6524h*)malloc(sizeof(struct pcal6524h));
	if (pca == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}
	pca->gpio_device.device.parent = parent;
	pca->gpio_device.gpio_read = pcal6524h_read;
	pca->gpio_device.gpio_write = pcal6524h_write;
	pca->gpio_device.gpio_toggle = pcal6524h_toggle;
	pca->gpio_device.gpio_get_output = pcal6524h_get_output;
	pca->gpio_device.pin_bitmask = extract_parameter_value(chip_specification, "pin_bitmask");
	pca->gpio_device.opendrain = extract_parameter_value(chip_specification, "opendrain");
	pca->gpio_device.active_level = extract_parameter_value(chip_specification, "active_level");

	pca->addr = extract_parameter_value(chip_specification, "addr");
	pca->port = extract_parameter_value(chip_specification, "port");

	return pca;
}

int pcal6524h_write(void* pcal6524h, unsigned char bit_value)
{
	struct pcal6524h* pca = pcal6524h;

	if (pca->gpio_device.opendrain > 0){
		pcal6524h_set_output(pca, ~pca->gpio_device.pin_bitmask);
		return pcal6524h_set_direction(pca, bit_value);
	}

	pcal6524h_set_direction(pca, ~pca->gpio_device.pin_bitmask);
	return pcal6524h_set_output(pca, bit_value);
}
int pcal6524h_set_output(struct pcal6524h* pcal6524h, unsigned char bit_value)
{
	struct pcal6524h* pca = pcal6524h;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;

	unsigned char output_cmd = (pca->port) + 0x04; //x04h is the output command for port 0
	unsigned char current_output[1];
	int bSucceed = 0;

	//read current output register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char output_data = (current_output[0] & (~pca->gpio_device.pin_bitmask)) | (bit_value & pca->gpio_device.pin_bitmask);
	//read current input register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_data, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int pcal6524h_get_direction(void* pcal6524h, unsigned char* bit_value_buffer)
{
	struct pcal6524h* pca = pcal6524h;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	unsigned char input_cmd = (pca->port) + 0x0C; //x0Ch is the io configuration command
	int bSucceed = 0;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, bit_value_buffer, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	if (pca->gpio_device.opendrain <= 0)
		(*bit_value_buffer) = ~(*bit_value_buffer);

	//mask away unwanted value;
	*bit_value_buffer = (*bit_value_buffer) & (pca->gpio_device.pin_bitmask);
	return 0;
}

int pcal6524h_read(void* pcal6524h, unsigned char* bit_value_buffer)
{
	struct pcal6524h* pca = pcal6524h;
	if (pca->gpio_device.opendrain > 0)
		return pcal6524h_get_direction(pca, bit_value_buffer);
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	unsigned char input_cmd = (pca->port) + 0x00; //x00h is the input command
	int bSucceed = 0;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, bit_value_buffer, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	//mask away unwanted value;
	*bit_value_buffer = (*bit_value_buffer) & (pca->gpio_device.pin_bitmask);
	return 0;
}

int pcal6524h_set_direction(struct pcal6524h* pca, unsigned char value)
{
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;

	unsigned char configure_cmd = (pca->port) + 0x0C; //x0Ch is the io configuration command
	unsigned char current_config;
	int bSucceed = 0;

	//read current configuration register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, configure_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, &current_config, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char input_bitmask = (value | (~pca->gpio_device.pin_bitmask)) & (current_config | pca->gpio_device.pin_bitmask);

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, configure_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_bitmask, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int pcal6524h_toggle(void* pcal6524h)
{
	struct pcal6524h* pca = pcal6524h;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;

	unsigned char output_cmd = (pca->port) + 0x04; //x04h is the output command for port 0
	unsigned char current_output[1];
	int bSucceed = 0;

	//read current output register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char output_data = current_output[0] ^ pca->gpio_device.pin_bitmask;
	//read current input register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_data, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int pcal6524h_get_output(void* pcal6524h, unsigned char* current_output)
{
	struct pcal6524h* pca = pcal6524h;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	unsigned char output_cmd = (pca->port) + 0x04; //x04h is the output command for port 0
	int bSucceed = 0;

	if (pca->gpio_device.opendrain > 0)
		output_cmd = (pca->port) + 0xC;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	*current_output = (*current_output) & pca->gpio_device.pin_bitmask;

	return 0;
}

////////////////////////ADP5585//////////////////////////////////
void* adp5585_create(char* chip_specification, void* parent)
{
	struct adp5585* adp = (struct adp5585*)malloc(sizeof(struct adp5585));
	if (adp == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}
	adp->gpio_device.device.parent = parent;
	adp->gpio_device.gpio_read = adp5585_read;
	adp->gpio_device.gpio_write = adp5585_write;
	adp->gpio_device.gpio_toggle = adp5585_toggle;
	adp->gpio_device.gpio_get_output = adp5585_get_output;
	adp->gpio_device.pin_bitmask = extract_parameter_value(chip_specification, "pin_bitmask");
	adp->gpio_device.opendrain = extract_parameter_value(chip_specification, "opendrain");
	adp->gpio_device.active_level = extract_parameter_value(chip_specification, "active_level");

	adp->addr = extract_parameter_value(chip_specification, "addr");
	adp->port = extract_parameter_value(chip_specification, "port");

	if (adp->gpio_device.opendrain <= 0)
		adp5585_set_direction(adp, adp->gpio_device.pin_bitmask);
	else
		adp5585_set_output(adp, ~adp->gpio_device.pin_bitmask);

	return adp;
}

int adp5585_write(void* adp5585, unsigned char bit_value)
{
	struct adp5585* adp = adp5585;
	if (adp->gpio_device.opendrain > 0)
		return adp5585_set_direction(adp, bit_value);

	return adp5585_set_output(adp, bit_value);
}
int adp5585_set_output(struct adp5585* adp5585, unsigned char bit_value)
{
	struct adp5585* adp = adp5585;
	struct i2c_device* parent = (void*)adp->gpio_device.device.parent;
	unsigned char addr_plus_write = (adp->addr << 1) + 0;
	unsigned char addr_plus_read = (adp->addr << 1) + 1;

	unsigned char output_cmd = (adp->port) + 0x23; //x23h is the output command for port a
	unsigned char current_output[1];
	int bSucceed = 0;

	//read current output register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char output_data = (current_output[0] & (~adp->gpio_device.pin_bitmask)) | (bit_value & adp->gpio_device.pin_bitmask);
	//read current input register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_data, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int adp5585_read(void* adp5585, unsigned char* bit_value_buffer)
{
	struct adp5585* adp = adp5585;
	struct i2c_device* parent = (void*)adp->gpio_device.device.parent;
	unsigned char addr_plus_write = (adp->addr << 1) + 0;
	unsigned char addr_plus_read = (adp->addr << 1) + 1;
	unsigned char input_cmd = (adp->port) + 0x23; //x15h is the input status reg, will only be used when INPUT
	int bSucceed = 0;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, bit_value_buffer, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	//mask away unwanted value;
	*bit_value_buffer = (*bit_value_buffer) & (adp->gpio_device.pin_bitmask);

	return 0;
}

int adp5585_set_direction(struct adp5585* adp, unsigned char value)
{
	struct i2c_device* parent = (void*)adp->gpio_device.device.parent;
	unsigned char addr_plus_write = (adp->addr << 1) + 0;
	unsigned char addr_plus_read = (adp->addr << 1) + 1;

	unsigned char configure_cmd = (adp->port) + 0x27; //x27h is the Direction_A reg
	unsigned char current_config;
	int bSucceed = 0;

	//read current configuration register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, configure_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, &current_config, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char input_bitmask = (value | (~adp->gpio_device.pin_bitmask)) & (current_config | adp->gpio_device.pin_bitmask);

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, configure_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_bitmask, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int adp5585_toggle(void* adp5585)
{
	printf("adp5585_toggle: TBD\n");

	return 0;
}

int adp5585_get_output(void* adp5585, unsigned char* current_output)
{
	struct adp5585* adp = adp5585;
	struct i2c_device* parent = (void*)adp->gpio_device.device.parent;
	unsigned char addr_plus_write = (adp->addr << 1) + 0;
	unsigned char addr_plus_read = (adp->addr << 1) + 1;
	unsigned char output_cmd = (adp->port) + 0X23; //x23h is the output command for port A
	int bSucceed = 0;

	if (adp->gpio_device.opendrain > 0)
		output_cmd = (adp->port) + 0x27;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	*current_output = (*current_output) & adp->gpio_device.pin_bitmask;

	return 0;
}

////////////////////////PCA9670//////////////////////////////////

void* pca9670_create(char* chip_specification, void* parent)
{
	struct pca9670* pca = (struct pca9670*)malloc(sizeof(struct pca9670));
	if (pca == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}
	pca->gpio_device.device.parent = parent;
	pca->gpio_device.gpio_read = pca9670_read;
	pca->gpio_device.gpio_write = pca9670_write;
	pca->gpio_device.gpio_get_output = pca9670_get_output;
	pca->gpio_device.pin_bitmask = extract_parameter_value(chip_specification, "pin_bitmask");
	pca->gpio_device.opendrain = extract_parameter_value(chip_specification, "opendrain");
	pca->gpio_device.active_level = extract_parameter_value(chip_specification, "active_level");

	pca->addr = extract_parameter_value(chip_specification, "addr");
	pca->port = extract_parameter_value(chip_specification, "port");

	return pca;
}

int pca9670_write(void* pca9670, unsigned char bit_value)
{
	struct pca9670* pca = pca9670;

	return pca9670_set_output(pca, bit_value);
}

int pca9670_set_output(struct pca9670* pca9670, unsigned char bit_value)
{
	struct pca9670* pca = pca9670;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	unsigned char current_output[1];
	int bSucceed = 0;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char output_data = (current_output[0] & (~pca->gpio_device.pin_bitmask)) | (bit_value & pca->gpio_device.pin_bitmask);
	//read current input register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_data, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int pca9670_read(void* pca9670, unsigned char* bit_value_buffer)
{
	struct pca9670* pca = pca9670;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	int bSucceed = 0;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, bit_value_buffer, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	//mask away unwanted value;
	*bit_value_buffer = (*bit_value_buffer) & (pca->gpio_device.pin_bitmask);
	return 0;
}

int pca9670_get_output(void* pca9670, unsigned char* current_output)
{
	struct pca9670* pca = pca9670;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	int bSucceed = 0;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1, I2C_TYPE_GPIO);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	*current_output = (*current_output) & pca->gpio_device.pin_bitmask;

	return 0;
}
