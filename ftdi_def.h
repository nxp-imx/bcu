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
#ifndef FTDI_DEF_H
#define FTDI_DEF_H

/*MPSSE Control Commands*/
#define MPSSE_CMD_SET_DATA_BITS_LOWBYTE		0x80
#define MPSSE_CMD_SET_DATA_BITS_HIGHBYTE	0x82
#define MPSSE_CMD_GET_DATA_BITS_LOWBYTE		0x81
#define MPSSE_CMD_GET_DATA_BITS_HIGHBYTE	0x83

#define MPSSE_CMD_SEND_IMMEDIATE			0x87
#define MPSSE_CMD_ENABLE_3PHASE_CLOCKING	0x8C
#define MPSSE_CMD_DISABLE_3PHASE_CLOCKING	0x8D
#define MPSSE_CMD_DISABLE_CLOCK_DIVIDE_BY_5	0x8A
#define MPSSE_CMD_DISABLE_ADAPTIVE_CLOCKING 0x97
#define MPSSE_CMD_SET_CLOCK_DIVISOR			0x86
#define MPSEE_CMD_DISABLE_LOOPBACK			0x85

/*MPSSE Data Commands - bit mode - MSB first */
#define MPSSE_CMD_DATA_OUT_BITS_POS_EDGE		0x12
#define MPSSE_CMD_DATA_OUT_BITS_NEG_EDGE		0x13
#define MPSSE_CMD_DATA_IN_BITS_POS_EDGE			0x22
#define MPSSE_CMD_DATA_IN_BITS_NEG_EDGE			0x26
#define MPSSE_CMD_DATA_BITS_IN_POS_OUT_NEG_EDGE	0x33
#define MPSSE_CMD_DATA_BITS_IN_NEG_OUT_POS_EDGE	0x36

/*MPSSE Data Commands - byte mode - MSB first */
#define MPSSE_CMD_DATA_OUT_BYTES_POS_EDGE			0x10
#define MPSSE_CMD_DATA_OUT_BYTES_NEG_EDGE			0x11
#define MPSSE_CMD_DATA_IN_BYTES_POS_EDGE			0x20
#define MPSSE_CMD_DATA_IN_BYTES_NEG_EDGE			0x24
#define MPSSE_CMD_DATA_BYTES_IN_POS_OUT_NEG_EDGE	0x31
#define MPSSE_CMD_DATA_BYTES_IN_NEG_OUT_POS_EDGE	0x34

/*SCL & SDA directions*/
#define DIRECTION_SCLIN_SDAIN				0x00
#define DIRECTION_SCLOUT_SDAIN				0x01
#define DIRECTION_SCLIN_SDAOUT				0x02
#define DIRECTION_SCLOUT_SDAOUT				0x03

/*SCL & SDA values*/
#define VALUE_SCLLOW_SDALOW					0x00
#define VALUE_SCLHIGH_SDALOW				0x01
#define VALUE_SCLLOW_SDAHIGH				0x02
#define VALUE_SCLHIGH_SDAHIGH				0x03

/*Data size in bits*/
#define DATA_SIZE_8BITS						0x07
#define DATA_SIZE_1BIT						0x00

#define CLOCK_DIVISOR_100K 0x12B
#define CLOCK_DIVISOR_200K 0x95
#define CLOCK_DIVISOR_400K 0x4A

#endif