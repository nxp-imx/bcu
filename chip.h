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

#ifndef CHIP_H
#define CHIP_H

#include "port.h"

struct device {
	char* name;
	int type;
	struct device* parent;
	struct device* child;
	int (*free)(void* p); // if there is any variable inside the device need to be free
};


struct i2c_device {
	struct device device;
	int (*i2c_read)(void*, unsigned char*, int); //Read one byte, give ack/nack
	int (*i2c_write)(void*, unsigned char); //Write one byte, check ack/nack
	int (*i2c_start)(void*);
	int (*i2c_stop)(void*); //sending Start/Stop Condition
};

#define MAX_PAC193X_CHANNELS 4
struct pac193x_reg_data {
	double	vbus[MAX_PAC193X_CHANNELS];
	double	vsense[MAX_PAC193X_CHANNELS];

	char	num_enabled_channels;
};

struct power_device {
	struct device device;
	int (*power_get_group)(void*);
	int (*power_get_sensor)(void*);
	int (*power_get_res)(void*);
	int (*power_set_snapshot)(void*);
	int (*power_get_data)(void*, struct pac193x_reg_data*);
	int (*switch_sensor)(void *, int i);
};

struct gpio_device {
	struct device device;
	int (*gpio_read)(void*, unsigned char*);
	int (*gpio_write)(void*, unsigned char);
	int (*gpio_toggle)(void*);//you dont have to implement unless it is used in monitor
	int (*gpio_get_output)(void*, unsigned char*);//you dont have to implement unless it is used in monitor
	unsigned char pin_bitmask; // specify which pins are output
	int opendrain;
};

struct eeprom_device {
	struct device device;
	int (*eeprom_read)(void*, unsigned char*);
	int (*eeprom_write)(void*, unsigned char*);
	int (*eeprom_check_board)(void*);//you dont have to implement unless it is used in monitor
};

struct name_and_init_func {
	char* name;
	void* (*create_funcptr)(char*, void*);
};

///////////////////////////////////////////////////////////////////////
struct at24cxx {
	struct eeprom_device eeprom_device;
	// int channel; //indicate which i2c channel to choose
	int addr;
};
int at24cxx_read(void* at24cxx, unsigned char* data_buffer);
int at24cxx_write(void* at24cxx, unsigned char* data_buffer);
int at24cxx_check_board(void* at24cxx);
void* at24cxx_create(char* chip_specification, void* parent);
///////////////////////////////////////////////////////////////////////
struct pca9548 {
	struct i2c_device i2c_device;
	int channel; //indicate which i2c channel to choose
	int addr;
};
int pca9548_read(void* pca9548, unsigned char* data_buffer, int is_nack);
int pca9548_write(void* pca9548, unsigned char data);
int pca9548_start(void* pca9548);
int pca9548_stop(void* pca9548);
void* pca9548_create(char* chip_specification, void* parent);

int pca9548_set_channel(struct pca9548* pca9548);
///////////////////////////////////////////////////////////////////////
struct ft4232h {
	struct i2c_device i2c_device;
	struct ftdi_info ftdi_info;
	int channel;
	unsigned char dir_bitmask;//define direction of the pins, 1 is outputm, 0 is input
	unsigned char val_bitmask; //define the output value of the bitmask, the value of each bit is only valid when the coresponding direction bit is 1
};
int ft4232h_i2c_read(void* ft4232h, unsigned char* data_buffer, int is_nack);
int ft4232h_i2c_write(void* ft4232h, unsigned char data);
int ft4232h_i2c_start(void* f2232h);
int ft4232h_i2c_stop(void* ft4232h);
void* ft4232h_i2c_create(char* chip_specification, void* parent);
int ft4232h_i2c_init(struct ft4232h* ft);
int ft4232h_i2c_free(void* ft4232h);
///////////////////////////////////////////////////////////////////////

struct ft4232h_gpio {
	struct gpio_device gpio_device;
	struct ftdi_info ftdi_info;
	int channel;
	unsigned char pin_bitmask;
};
int ft4232h_gpio_read(void* ft4232h, unsigned char* bit_value_buffer);
int ft4232h_gpio_write(void* ft4232h, unsigned char bit_value);
int ft4232h_gpio_toggle(void* ft4232h);
void* ft4232h_gpio_create(char* chip_specification, void* parent);
int ft4232h_gpio_free(void* ft4232h);
///////////////////////////////////////////////////////////////////////
struct pac1934 {
	struct power_device power_device;
	int group;
	int sensor;
	int addr;
	int rs1;
	int sensor2;
	int rs2;
	int cur_sensor;
	int cur_rs;
};
int pac1934_switch(void *pac1934, int i);
int get_pac1934_group(void* pac1934);
int get_pac1934_sensor(void* pac1934);
int get_pac1934_res(void* pac1934);
int pac1934_snapshot(void* pac1934);
int pac1934_get_data(void* pac1934, struct pac193x_reg_data* pac_reg);
void* pac1934_create(char* chip_specification, void* parent);
////////////////////////////////////////////////////////////////////////
struct pca6416a {
	struct gpio_device gpio_device;
	int addr;
	int port;
	unsigned char pin_bitmask; //specify which pin to use
};
int pca6416a_read(void* pca6416a, unsigned char* bit_value_buffer);
int pca6416a_write(void* pca6416a, unsigned char bit_value);
int pca6416a_toggle(void* pca6416a);
void* pca6416a_create(char* chip_specification, void* parent);
int pca6416a_set_direction(struct pca6416a* pca, unsigned char value);
int pca6416a_set_output(struct pca6416a* pca, unsigned char value);
int pca6416a_get_output(void* pca6416a, unsigned char* current_output);
#endif //CHIP_H
