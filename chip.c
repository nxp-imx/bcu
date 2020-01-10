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
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#include "parser.h"
#include "chip.h"
#include "port.h"
#include "ftdi_def.h"

#define MAX_FTDI_BUFFER_SIZE 512

extern char GV_LOCATION_ID[];

struct name_and_init_func chip_list[] =
{
	"pca9548", pca9548_create,
	"ft4232h_i2c", ft4232h_i2c_create,
	"ft4232h_gpio", ft4232h_gpio_create,
	"pac1934", pac1934_create,
	"pca6416a", pca6416a_create
};
int num_of_chips = sizeof(chip_list) / sizeof(struct name_and_init_func);

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
	pca->i2c_device.i2c_write = pca9548_write;
	pca->i2c_device.i2c_start = pca9548_start;
	pca->i2c_device.i2c_stop = pca9548_stop;
	pca->channel = extract_parameter_value(chip_specification, "channel");
	pca->addr = extract_parameter_value(chip_specification, "addr");
	pca9548_set_channel(pca);
	//printf("pca9548 created!\n");
	return pca;
}

int pca9548_read(void* pca9548, unsigned char* data_buffer, int is_nack)
{
	struct pca9548* pca = pca9548;
	struct i2c_device* parent = (void*)pca->i2c_device.device.parent;
	parent->i2c_read(parent, data_buffer, is_nack);
	return 0;
}

int pca9548_write(void* pca9548, unsigned char data)
{
	struct pca9548* pca = pca9548;
	struct i2c_device* parent = (void*)pca->i2c_device.device.parent;
	return 	parent->i2c_write(parent, data);
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
	status = parent->i2c_write(parent, addr_plus_write);
	if (!status)
	{
		printf("oh no! no ack received!\n");
	}
	parent->i2c_write(parent, change_channel_cmd);
	parent->i2c_stop(parent);

	return 0;
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

	ft_init(&(ft->ftdi_info));

	int status;
	if (strlen(GV_LOCATION_ID) == 0) {
		status = ft_open_channel(&ft->ftdi_info, ft->channel);
	}
	else {
		status = ft_open_channel_by_id(&ft->ftdi_info, ft->channel, GV_LOCATION_ID);
	}

	if (status != 0)
	{
		printf("failed to open ftdi device!\n");
#ifdef __linux__
		printf("***please make sure you run bcu with sudo\n");
#endif		
		free(ft);
		return NULL;
	}
	ft4232h_i2c_init(ft);
	//printf("ft4232h created!\n");
	return ft;
}

int ft4232h_i2c_read(void* ft4232h, unsigned char* data_buffer, int is_nack)
{
	struct ft4232h* ft = ft4232h;
	unsigned char buffer[MAX_FTDI_BUFFER_SIZE];
	unsigned char in_buffer[MAX_FTDI_BUFFER_SIZE];
	int i = 0, ftStatus = 0;
#ifdef __linux__
	ft_clear_buffer(&ft->ftdi_info);
#endif

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
	ftStatus = ft_write(&ft->ftdi_info, buffer, i); //Send off the commands
	if (ftStatus < 0) return ftStatus;
	ftStatus = ft_read(&ft->ftdi_info, in_buffer, 1); //Read one byte
	if (ftStatus < 0) return ftStatus;
	*data_buffer = in_buffer[0];

	return 0;

}

int ft4232h_i2c_write(void* ft4232h, unsigned char data)
{
	struct ft4232h* ft = ft4232h;
	unsigned char buffer[MAX_FTDI_BUFFER_SIZE];
	unsigned char in_buffer[MAX_FTDI_BUFFER_SIZE];
	int i = 0, ftStatus = 0;
#ifdef __linux__ 
	ft_clear_buffer(&ft->ftdi_info);
#endif

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
	buffer[i++] = VALUE_SCLLOW_SDALOW | ft->val_bitmask; /*Value*/
	buffer[i++] = DIRECTION_SCLOUT_SDAOUT | ft->dir_bitmask; /*Direction*/
	ftStatus = ft_write(&ft->ftdi_info, buffer, i); //Send off the commands
	if (ftStatus < 0) return ftStatus;
	ftStatus = ft_read(&ft->ftdi_info, in_buffer, 1);
	if (ftStatus < 0) return ftStatus;

	if (((in_buffer[0] & 0x1) != 0)) //Check ACK bit 0 on data byte read out
	{
		printf("can't get the ACK bit after write\n");
		return -1; /*Error, can't get the ACK bit */
	}

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

	ftStatus = ft_write(&ft->ftdi_info, buffer, i); //Send off the commands
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
	ftStatus = ft_write(&ft->ftdi_info, buffer, i); //Send off the commands
	if (ftStatus < 0)
		return ftStatus;

	return 0;
}

int ft4232h_i2c_init(struct ft4232h* ft)
{
	unsigned char buffer[MAX_FTDI_BUFFER_SIZE];
	int i = 0, ftStatus = 0;

	ft_set_bitmode(&ft->ftdi_info, 0, 0); //resetting the controller
	ft_set_bitmode(&ft->ftdi_info, 0, BM_MPSSE);//set as MPSSE

	buffer[i++] = MPSSE_CMD_DISABLE_CLOCK_DIVIDE_BY_5; //Ensure disable clock divide by 5 for 60Mhz master clock
	buffer[i++] = MPSSE_CMD_DISABLE_ADAPTIVE_CLOCKING; //Ensure turn off adaptive clocking
	buffer[i++] = MPSSE_CMD_DISABLE_3PHASE_CLOCKING; //Disable 3 phase data clock, used by I2C to allow data on one clock edges
	ftStatus = ft_write(&ft->ftdi_info, buffer, i);
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

	ftStatus = ft_write(&ft->ftdi_info, buffer, i);
	if (ftStatus < 0)
		return ftStatus;
	i = 0; //Clear output buffer
	msleep(20); //Delay for a while
	//Turn off loop back in case
	buffer[i++] = MPSEE_CMD_DISABLE_LOOPBACK; //Command to turn off loop back of TDI/TDO connection
	ftStatus = ft_write(&ft->ftdi_info, buffer, i);
	if (ftStatus < 0)
		return ftStatus;
	msleep(30); //Delay for a while

	//printf("I2C initialization finished!\n");
	return 0;
}

int ft4232h_i2c_free(void* ft4232h)
{
	struct ft4232h* ft = ft4232h;
	int ftStatus = 0;

	ftStatus = ft_close(&ft->ftdi_info);
	if (ftStatus < 0)
		return ftStatus;

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
	//ft->gpio_device.gpio_read=ft4232h_gpio_read;
	ft->gpio_device.gpio_write = ft4232h_gpio_write;
	//ft->gpio_device.gpio_toggle=ft4232h_gpio_toggle;
	ft->gpio_device.device.free = ft4232h_gpio_free;
	ft->gpio_device.pin_bitmask = extract_parameter_value(chip_specification, "pin_bitmask");
	ft->channel = extract_parameter_value(chip_specification, "channel");

	ft_init(&(ft->ftdi_info));
	int status;
	if (strlen(GV_LOCATION_ID) == 0) {
		status = ft_open_channel(&ft->ftdi_info, ft->channel);
	}
	else {
		status = ft_open_channel_by_id(&ft->ftdi_info, ft->channel, GV_LOCATION_ID);
	}


	if (status != 0)
	{
		printf("failed to open ftdi device!\n");
#ifdef __linux__
		printf("***please make sure you run bcu with sudo\n");
#endif			

		free(ft);
		return NULL;
	}

	return ft;
}


int ft4232h_gpio_write(void* ft4232h, unsigned char bit_value)
{
	struct ft4232h_gpio* ft = ft4232h;
	int mask = 0xFF;
	if (ft_set_bitmode(&ft->ftdi_info, mask, BM_BITBANG) < 0)
		printf("failed to set bitmode\n");

	unsigned char current_output;
	ft_read_pins(&ft->ftdi_info, &current_output);
	//printf("previous output: %x\n", current_output);

	unsigned char data;
	data = (current_output & (~ft->gpio_device.pin_bitmask)) | (bit_value & ft->gpio_device.pin_bitmask);

	if (ft_write(&ft->ftdi_info, &data, 1) < 0)
		printf("failed to write\n");

	return 0;
}


int ft4232h_gpio_read(void* ft4232h, unsigned char* bit_value_buffer)
{
	printf("read is not yet implemented!\n");
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
	ft_close(&ft->ftdi_info);
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
	pac->power_device.power_get_current = pac1934_get_current;
	pac->power_device.power_get_voltage = pac1934_get_voltage;
	pac->sensor = extract_parameter_value(chip_specification, "sensor");
	pac->addr = extract_parameter_value(chip_specification, "addr");
	//printf("pac1934 created!\n");
	return pac;
}

int pac1934_get_voltage(void* pac1934, float* voltage)
{
	struct pac1934* pac = pac1934;
	struct i2c_device* parent = (void*)pac->power_device.device.parent;
	char addr_plus_write = (pac->addr) << 1;
	char addr_plus_read = (pac->addr << 1) + 1;

	//first refresh the powerister
	parent->i2c_start(parent);
	parent->i2c_write(parent, addr_plus_write);
	parent->i2c_write(parent, 0x00); //refresh;
	parent->i2c_stop(parent);
	msleep(1);
	//read the data
	parent->i2c_start(parent);
	parent->i2c_write(parent, addr_plus_write);
	parent->i2c_write(parent, 0x07 + pac->sensor - 1); //get voltage;
	parent->i2c_start(parent);
	parent->i2c_write(parent, addr_plus_read);

	unsigned char data[2];
	parent->i2c_read(parent, &data[0], 0);
	parent->i2c_read(parent, &data[1], 1); //last bit should be ack

	parent->i2c_stop(parent);
	//printf("data in register: %02x%02x\n", data[0], data[1] );
	*voltage = ((float)(((data[0] << 8) + data[1]) * 32)) / (65535);
	return 0;
}

int pac1934_get_current(void* pac1934, float* current)
{
	struct pac1934* pac = pac1934;
	struct i2c_device* parent = (void*)pac->power_device.device.parent;
	char addr_plus_write = (pac->addr) << 1;
	char addr_plus_read = (pac->addr << 1) + 1;

	//first refresh the powerister
	parent->i2c_start(parent);
	parent->i2c_write(parent, addr_plus_write);
	parent->i2c_write(parent, 0x00); //refresh;
	parent->i2c_stop(parent);
	msleep(1);
	//read the data
	parent->i2c_start(parent);
	parent->i2c_write(parent, addr_plus_write);
	parent->i2c_write(parent, 0x0B + pac->sensor - 1); //get voltage;
	parent->i2c_start(parent);
	parent->i2c_write(parent, addr_plus_read);

	unsigned char data[2];
	parent->i2c_read(parent, &data[0], 0);
	parent->i2c_read(parent, &data[1], 1); //last bit should be ack

	parent->i2c_stop(parent);
	//printf("data in register: %02x%02x\n", data[0], data[1] );
	*current = ((float)(((data[0] << 8) + data[1]) * 1)) / (65535);
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

	pca->addr = extract_parameter_value(chip_specification, "addr");
	pca->port = extract_parameter_value(chip_specification, "port");

	pca6416a_set_direction(pca);
	//printf("pca6416a created!\n");
	return pca;
}

int pca6416a_write(void* pca6416a, unsigned char bit_value)
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
	bSucceed = parent->i2c_write(parent, addr_plus_write);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char output_data = (current_output[0] & (~pca->gpio_device.pin_bitmask)) | (bit_value & pca->gpio_device.pin_bitmask);
	//read current input register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_data);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int pca6416a_read(void* pca6416a, unsigned char* bit_value_buffer)
{
	struct pca6416a* pca = pca6416a;
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;
	unsigned char input_cmd = (pca->port) + 0x00; //x00h is the input command
	int bSucceed = 0;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_cmd);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, bit_value_buffer, 1);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	//mask away unwanted value;
	*bit_value_buffer = (*bit_value_buffer) & (~pca->gpio_device.pin_bitmask);

	return 0;
}

int pca6416a_set_direction(struct pca6416a* pca)
{
	struct i2c_device* parent = (void*)pca->gpio_device.device.parent;
	unsigned char addr_plus_write = (pca->addr << 1) + 0;
	unsigned char addr_plus_read = (pca->addr << 1) + 1;

	unsigned char configure_cmd = (pca->port) + 0x06; //x06h is the io configuration command
	unsigned char current_config;
	int bSucceed = 0;

	//read current configuration register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, configure_cmd);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, &current_config, 1);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char input_bitmask = (~(pca->gpio_device.pin_bitmask)) & current_config;

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, configure_cmd);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, input_bitmask);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	return 0;
}

int pca6416a_toggle(void* pca6416a)
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
	bSucceed = parent->i2c_write(parent, addr_plus_write);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;

	unsigned char output_data = current_output[0] ^ pca->gpio_device.pin_bitmask;
	//read current input register
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_data);
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

	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_write);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, output_cmd);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_start(parent);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_write(parent, addr_plus_read);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_read(parent, current_output, 1);
	if (bSucceed) return bSucceed;
	bSucceed = parent->i2c_stop(parent);
	if (bSucceed) return bSucceed;
	*current_output = (*current_output) & pca->gpio_device.pin_bitmask;
	return 0;
}