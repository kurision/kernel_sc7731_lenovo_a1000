/* drivers/video/sc8825/lcd_ili9806e_holit0561_mipi.c
 *
 * Support for ili9806e_holit0561 mipi LCD device
 *
 * Copyright (C) 2010 Spreadtrum
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include "../sprdfb_panel.h"
#include <linux/io.h>//add by liuwei
#include <soc/sprd/hardware.h>//add by liuwei
//#define LCD_Delay(ms)  uDelay(ms*1000)

#define MAX_DATA   48
//+modify by liuwei
typedef struct LCM_Init_Code_tag
{
    unsigned int tag;
    unsigned char data[MAX_DATA];
} LCM_Init_Code;
typedef struct LCM_force_cmd_code_tag
{
    unsigned int datatype;
    LCM_Init_Code real_cmd_code;
} LCM_Force_Cmd_Code;

#define LCM_TAG_SHIFT 24
#define LCM_TAG_MASK  ((1 << 24) -1)
#define LCM_SEND(len) ((1 << LCM_TAG_SHIFT)| len)
#define LCM_SLEEP(ms) ((2 << LCM_TAG_SHIFT)| ms)
//#define ARRAY_SIZE(array) ( sizeof(array) / sizeof(array[0]))
#define LCM_TAG_SEND  (1<< 0)
#define LCM_TAG_SLEEP (1 << 1)


static u8 lcd_ili9806e_holit0561_cmd_1[]= {0xFF,0xFF,0x98,0x06,0x04,0x01};//ChangetoPage1
static u8 lcd_ili9806e_holit0561_cmd_2[]= {0x08,0x10};//outputSDA
static u8 lcd_ili9806e_holit0561_cmd_3[]= {0x21,0x01};//DE=1Active
static u8 lcd_ili9806e_holit0561_cmd_4[]= {0x30,0x02};//480X800
static u8 lcd_ili9806e_holit0561_cmd_5[]= {0x31,0x00};//2-dot
static u8 lcd_ili9806e_holit0561_cmd_6[]= {0x40,0x15};//BT
static u8 lcd_ili9806e_holit0561_cmd_7[]= {0x41,0x33};//DDVDH/DDVDL
static u8 lcd_ili9806e_holit0561_cmd_8[]= {0x42,0x02};//VGH/VGL
static u8 lcd_ili9806e_holit0561_cmd_9[]= {0x43,0x09};//VGH_CP_OFF
static u8 lcd_ili9806e_holit0561_cmd_10[]={0x44,0x09};//VGL_CP_OFF
static u8 lcd_ili9806e_holit0561_cmd_11[]={0x50,0x78};//
static u8 lcd_ili9806e_holit0561_cmd_12[]={0x51,0x78};//
static u8 lcd_ili9806e_holit0561_cmd_13[]={0x52,0x00};//flickerMSB
static u8 lcd_ili9806e_holit0561_cmd_14[]={0x53,0x66};//flickerLSB
static u8 lcd_ili9806e_holit0561_cmd_15[]={0x57,0x50};//flickerLSB
static u8 lcd_ili9806e_holit0561_cmd_16[]={0x60,0x07};//SDTI
static u8 lcd_ili9806e_holit0561_cmd_17[]={0x61,0x00};//CRTI
static u8 lcd_ili9806e_holit0561_cmd_18[]={0x62,0x07};//EQTI
static u8 lcd_ili9806e_holit0561_cmd_19[]={0x63,0x00};//PCTI
static u8 lcd_ili9806e_holit0561_cmd_20[]={0xA0,0x23};//Gamma0
static u8 lcd_ili9806e_holit0561_cmd_21[]={0xA1,0x1A};//Gamma4
static u8 lcd_ili9806e_holit0561_cmd_22[]={0xA2,0x20};//Gamma8
static u8 lcd_ili9806e_holit0561_cmd_23[]={0xA3,0x18};//Gamma16
static u8 lcd_ili9806e_holit0561_cmd_24[]={0xA4,0x0A};//Gamma24
static u8 lcd_ili9806e_holit0561_cmd_25[]={0xA5,0x1F};//Gamma52
static u8 lcd_ili9806e_holit0561_cmd_26[]={0xA6,0x0F};//Gamma80
static u8 lcd_ili9806e_holit0561_cmd_27[]={0xA7,0x0F};//Gamma108
static u8 lcd_ili9806e_holit0561_cmd_28[]={0xA8,0x00};//Gamma147
static u8 lcd_ili9806e_holit0561_cmd_29[]={0xA9,0x02};//Gamma175
static u8 lcd_ili9806e_holit0561_cmd_30[]={0xAA,0x00};//Gamma203
static u8 lcd_ili9806e_holit0561_cmd_31[]={0xAB,0x03};//Gamma231
static u8 lcd_ili9806e_holit0561_cmd_32[]={0xAC,0x17};//Gamma239
static u8 lcd_ili9806e_holit0561_cmd_33[]={0xAD,0x2F};//Gamma247
static u8 lcd_ili9806e_holit0561_cmd_34[]={0xAE,0x26};//Gamma251
static u8 lcd_ili9806e_holit0561_cmd_35[]={0xAF,0x00};//Gamma255
static u8 lcd_ili9806e_holit0561_cmd_36[]={0xC0,0x05};//Gamma0
static u8 lcd_ili9806e_holit0561_cmd_37[]={0xC1,0x18};//Gamma4
static u8 lcd_ili9806e_holit0561_cmd_38[]={0xC2,0x21};//Gamma8
static u8 lcd_ili9806e_holit0561_cmd_39[]={0xC3,0x09};//Gamma16
static u8 lcd_ili9806e_holit0561_cmd_40[]={0xC4,0x06};//Gamma24
static u8 lcd_ili9806e_holit0561_cmd_41[]={0xC5,0x0C};//Gamma52
static u8 lcd_ili9806e_holit0561_cmd_42[]={0xC6,0x04};//Gamma80
static u8 lcd_ili9806e_holit0561_cmd_43[]={0xC7,0x02};//Gamma108
static u8 lcd_ili9806e_holit0561_cmd_44[]={0xC8,0x08};//Gamma147
static u8 lcd_ili9806e_holit0561_cmd_45[]={0xC9,0x0C};//Gamma175
static u8 lcd_ili9806e_holit0561_cmd_46[]={0xCA,0x0D};//Gamma203
static u8 lcd_ili9806e_holit0561_cmd_47[]={0xCB,0x06};//Gamma231
static u8 lcd_ili9806e_holit0561_cmd_48[]={0xCC,0x07};//Gamma239
static u8 lcd_ili9806e_holit0561_cmd_49[]={0xCD,0x27};//Gamma247
static u8 lcd_ili9806e_holit0561_cmd_50[]={0xCE,0x28};//Gamma251
static u8 lcd_ili9806e_holit0561_cmd_51[]={0xCF,0x00};//Gamma255
static u8 lcd_ili9806e_holit0561_cmd_52[]={0xFF,0xFF,0x98,0x06,0x04,0x06};//ChangetoPage6
static u8 lcd_ili9806e_holit0561_cmd_53[]={0x00,0x21};
static u8 lcd_ili9806e_holit0561_cmd_54[]={0x01,0x06};
static u8 lcd_ili9806e_holit0561_cmd_55[]={0x02,0x00};
static u8 lcd_ili9806e_holit0561_cmd_56[]={0x03,0x00};
static u8 lcd_ili9806e_holit0561_cmd_57[]={0x04,0x01};
static u8 lcd_ili9806e_holit0561_cmd_58[]={0x05,0x05};
static u8 lcd_ili9806e_holit0561_cmd_59[]={0x06,0x98};
static u8 lcd_ili9806e_holit0561_cmd_60[]={0x07,0x02};
static u8 lcd_ili9806e_holit0561_cmd_61[]={0x08,0x05};
static u8 lcd_ili9806e_holit0561_cmd_62[]={0x09,0x00};
static u8 lcd_ili9806e_holit0561_cmd_63[]={0x0A,0x00};
static u8 lcd_ili9806e_holit0561_cmd_64[]={0x0B,0x00};
static u8 lcd_ili9806e_holit0561_cmd_65[]={0x0C,0x01};
static u8 lcd_ili9806e_holit0561_cmd_66[]={0x0D,0x01};
static u8 lcd_ili9806e_holit0561_cmd_67[]={0x0E,0x00};
static u8 lcd_ili9806e_holit0561_cmd_68[]={0x0F,0x00};
static u8 lcd_ili9806e_holit0561_cmd_69[]={0x10,0x77};
static u8 lcd_ili9806e_holit0561_cmd_70[]={0x11,0xF4};
static u8 lcd_ili9806e_holit0561_cmd_71[]={0x12,0x00};
static u8 lcd_ili9806e_holit0561_cmd_72[]={0x13,0x00};
static u8 lcd_ili9806e_holit0561_cmd_73[]={0x14,0x00};
static u8 lcd_ili9806e_holit0561_cmd_74[]={0x15,0xC0};
static u8 lcd_ili9806e_holit0561_cmd_75[]={0x16,0x08};
static u8 lcd_ili9806e_holit0561_cmd_76[]={0x17,0x00};
static u8 lcd_ili9806e_holit0561_cmd_77[]={0x18,0x00};
static u8 lcd_ili9806e_holit0561_cmd_78[]={0x19,0x00};
static u8 lcd_ili9806e_holit0561_cmd_79[]={0x1A,0x00};
static u8 lcd_ili9806e_holit0561_cmd_80[]={0x1B,0x00};
static u8 lcd_ili9806e_holit0561_cmd_81[]={0x1C,0x00};
static u8 lcd_ili9806e_holit0561_cmd_82[]={0x1D,0x00};
static u8 lcd_ili9806e_holit0561_cmd_83[]={0x20,0x01};
static u8 lcd_ili9806e_holit0561_cmd_84[]={0x21,0x23};
static u8 lcd_ili9806e_holit0561_cmd_85[]={0x22,0x45};
static u8 lcd_ili9806e_holit0561_cmd_86[]={0x23,0x67};
static u8 lcd_ili9806e_holit0561_cmd_87[]={0x24,0x01};
static u8 lcd_ili9806e_holit0561_cmd_88[]={0x25,0x23};
static u8 lcd_ili9806e_holit0561_cmd_89[]={0x26,0x45};
static u8 lcd_ili9806e_holit0561_cmd_90[]={0x27,0x67};
static u8 lcd_ili9806e_holit0561_cmd_91[]={0x30,0x03};
static u8 lcd_ili9806e_holit0561_cmd_92[]={0x31,0x22};
static u8 lcd_ili9806e_holit0561_cmd_93[]={0x32,0x11};
static u8 lcd_ili9806e_holit0561_cmd_94[]={0x33,0x00};
static u8 lcd_ili9806e_holit0561_cmd_95[]={0x34,0x86};
static u8 lcd_ili9806e_holit0561_cmd_96[]={0x35,0x68};
static u8 lcd_ili9806e_holit0561_cmd_97[]={0x36,0x22};
static u8 lcd_ili9806e_holit0561_cmd_98[]={0x37,0x22};
static u8 lcd_ili9806e_holit0561_cmd_99[]={0x38,0xDA};
static u8 lcd_ili9806e_holit0561_cmd_100[]={0x39,0xCB};
static u8 lcd_ili9806e_holit0561_cmd_101[]={0x3A,0xBC};
static u8 lcd_ili9806e_holit0561_cmd_102[]={0x3B,0xAD};
static u8 lcd_ili9806e_holit0561_cmd_103[]={0x3C,0x22};
static u8 lcd_ili9806e_holit0561_cmd_104[]={0x3D,0x22};
static u8 lcd_ili9806e_holit0561_cmd_105[]={0x3E,0x22};
static u8 lcd_ili9806e_holit0561_cmd_106[]={0x3F,0x22};
static u8 lcd_ili9806e_holit0561_cmd_107[]={0x40,0x22};
static u8 lcd_ili9806e_holit0561_cmd_108[]={0x52,0x10};
static u8 lcd_ili9806e_holit0561_cmd_109[]={0x53,0x10};
static u8 lcd_ili9806e_holit0561_cmd_110[]={0xFF,0xFF,0x98,0x06,0x04,0x07};//ChangetoPage7
static u8 lcd_ili9806e_holit0561_cmd_111[]={0x17,0x22};//VREG1/2OUTOut
static u8 lcd_ili9806e_holit0561_cmd_112[]={0x02,0x77};
static u8 lcd_ili9806e_holit0561_cmd_113[]={0xE1,0x79};
static u8 lcd_ili9806e_holit0561_cmd_114[]={0xFF,0xFF,0x98,0x06,0x04,0x00};//ChangetoPage0
static u8 lcd_ili9806e_holit0561_cmd_115[]={0x11,0x00};//Sleep-Out
static u8 lcd_ili9806e_holit0561_cmd_116[]={0x29,0x00};//DisplayOn

static sprdfb_dev_dsi_cmds lcd_ili9806e_holit0561_cmds_init[] =
{
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_1), .p_cmd=lcd_ili9806e_holit0561_cmd_1,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_2), .p_cmd=lcd_ili9806e_holit0561_cmd_2,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_3), .p_cmd=lcd_ili9806e_holit0561_cmd_3,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_4), .p_cmd=lcd_ili9806e_holit0561_cmd_4,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_5), .p_cmd=lcd_ili9806e_holit0561_cmd_5,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_6), .p_cmd=lcd_ili9806e_holit0561_cmd_6,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_7), .p_cmd=lcd_ili9806e_holit0561_cmd_7,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_8), .p_cmd=lcd_ili9806e_holit0561_cmd_8,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_9), .p_cmd=lcd_ili9806e_holit0561_cmd_9,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_10), .p_cmd=lcd_ili9806e_holit0561_cmd_10,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_11), .p_cmd=lcd_ili9806e_holit0561_cmd_11,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_12), .p_cmd=lcd_ili9806e_holit0561_cmd_12,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_13), .p_cmd=lcd_ili9806e_holit0561_cmd_13,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_14), .p_cmd=lcd_ili9806e_holit0561_cmd_14,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_15), .p_cmd=lcd_ili9806e_holit0561_cmd_15,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_16), .p_cmd=lcd_ili9806e_holit0561_cmd_16,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_17), .p_cmd=lcd_ili9806e_holit0561_cmd_17,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_18), .p_cmd=lcd_ili9806e_holit0561_cmd_18,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_19), .p_cmd=lcd_ili9806e_holit0561_cmd_19,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_20), .p_cmd=lcd_ili9806e_holit0561_cmd_20,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_21), .p_cmd=lcd_ili9806e_holit0561_cmd_21,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_22), .p_cmd=lcd_ili9806e_holit0561_cmd_22,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_23), .p_cmd=lcd_ili9806e_holit0561_cmd_23,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_24), .p_cmd=lcd_ili9806e_holit0561_cmd_24,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_25), .p_cmd=lcd_ili9806e_holit0561_cmd_25,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_26), .p_cmd=lcd_ili9806e_holit0561_cmd_26,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_27), .p_cmd=lcd_ili9806e_holit0561_cmd_27,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_28), .p_cmd=lcd_ili9806e_holit0561_cmd_28,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_29), .p_cmd=lcd_ili9806e_holit0561_cmd_29,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_30), .p_cmd=lcd_ili9806e_holit0561_cmd_30,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_31), .p_cmd=lcd_ili9806e_holit0561_cmd_31,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_32), .p_cmd=lcd_ili9806e_holit0561_cmd_32,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_33), .p_cmd=lcd_ili9806e_holit0561_cmd_33,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_34), .p_cmd=lcd_ili9806e_holit0561_cmd_34,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_35), .p_cmd=lcd_ili9806e_holit0561_cmd_35,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_36), .p_cmd=lcd_ili9806e_holit0561_cmd_36,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_37), .p_cmd=lcd_ili9806e_holit0561_cmd_37,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_38), .p_cmd=lcd_ili9806e_holit0561_cmd_38,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_39), .p_cmd=lcd_ili9806e_holit0561_cmd_39,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_40), .p_cmd=lcd_ili9806e_holit0561_cmd_40,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_41), .p_cmd=lcd_ili9806e_holit0561_cmd_41,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_42), .p_cmd=lcd_ili9806e_holit0561_cmd_42,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_43), .p_cmd=lcd_ili9806e_holit0561_cmd_43,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_44), .p_cmd=lcd_ili9806e_holit0561_cmd_44,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_45), .p_cmd=lcd_ili9806e_holit0561_cmd_45,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_46), .p_cmd=lcd_ili9806e_holit0561_cmd_46,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_47), .p_cmd=lcd_ili9806e_holit0561_cmd_47,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_48), .p_cmd=lcd_ili9806e_holit0561_cmd_48,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_49), .p_cmd=lcd_ili9806e_holit0561_cmd_49,},  
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_50), .p_cmd=lcd_ili9806e_holit0561_cmd_50,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_51), .p_cmd=lcd_ili9806e_holit0561_cmd_51,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_52), .p_cmd=lcd_ili9806e_holit0561_cmd_52,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_53), .p_cmd=lcd_ili9806e_holit0561_cmd_53,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_54), .p_cmd=lcd_ili9806e_holit0561_cmd_54,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_55), .p_cmd=lcd_ili9806e_holit0561_cmd_55,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_56), .p_cmd=lcd_ili9806e_holit0561_cmd_56,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_57), .p_cmd=lcd_ili9806e_holit0561_cmd_57,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_58), .p_cmd=lcd_ili9806e_holit0561_cmd_58,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_59), .p_cmd=lcd_ili9806e_holit0561_cmd_59,}, 
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_60), .p_cmd=lcd_ili9806e_holit0561_cmd_60,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_61), .p_cmd=lcd_ili9806e_holit0561_cmd_61,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_62), .p_cmd=lcd_ili9806e_holit0561_cmd_62,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_63), .p_cmd=lcd_ili9806e_holit0561_cmd_63,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_64), .p_cmd=lcd_ili9806e_holit0561_cmd_64,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_65), .p_cmd=lcd_ili9806e_holit0561_cmd_65,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_66), .p_cmd=lcd_ili9806e_holit0561_cmd_66,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_67), .p_cmd=lcd_ili9806e_holit0561_cmd_67,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_68), .p_cmd=lcd_ili9806e_holit0561_cmd_68,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_69), .p_cmd=lcd_ili9806e_holit0561_cmd_69,}, 
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_70), .p_cmd=lcd_ili9806e_holit0561_cmd_70,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_71), .p_cmd=lcd_ili9806e_holit0561_cmd_71,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_72), .p_cmd=lcd_ili9806e_holit0561_cmd_72,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_73), .p_cmd=lcd_ili9806e_holit0561_cmd_73,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_74), .p_cmd=lcd_ili9806e_holit0561_cmd_74,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_75), .p_cmd=lcd_ili9806e_holit0561_cmd_75,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_76), .p_cmd=lcd_ili9806e_holit0561_cmd_76,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_77), .p_cmd=lcd_ili9806e_holit0561_cmd_77,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_78), .p_cmd=lcd_ili9806e_holit0561_cmd_78,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_79), .p_cmd=lcd_ili9806e_holit0561_cmd_79,}, 
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_80), .p_cmd=lcd_ili9806e_holit0561_cmd_80,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_81), .p_cmd=lcd_ili9806e_holit0561_cmd_81,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_82), .p_cmd=lcd_ili9806e_holit0561_cmd_82,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_83), .p_cmd=lcd_ili9806e_holit0561_cmd_83,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_84), .p_cmd=lcd_ili9806e_holit0561_cmd_84,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_85), .p_cmd=lcd_ili9806e_holit0561_cmd_85,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_86), .p_cmd=lcd_ili9806e_holit0561_cmd_86,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_87), .p_cmd=lcd_ili9806e_holit0561_cmd_87,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_88), .p_cmd=lcd_ili9806e_holit0561_cmd_88,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_89), .p_cmd=lcd_ili9806e_holit0561_cmd_89,}, 
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_90), .p_cmd=lcd_ili9806e_holit0561_cmd_90,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_91), .p_cmd=lcd_ili9806e_holit0561_cmd_91,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_92), .p_cmd=lcd_ili9806e_holit0561_cmd_92,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_93), .p_cmd=lcd_ili9806e_holit0561_cmd_93,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_94), .p_cmd=lcd_ili9806e_holit0561_cmd_94,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_95), .p_cmd=lcd_ili9806e_holit0561_cmd_95,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_96), .p_cmd=lcd_ili9806e_holit0561_cmd_96,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_97), .p_cmd=lcd_ili9806e_holit0561_cmd_97,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_98), .p_cmd=lcd_ili9806e_holit0561_cmd_98,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_99), .p_cmd=lcd_ili9806e_holit0561_cmd_99,}, 
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_100), .p_cmd=lcd_ili9806e_holit0561_cmd_100,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_101), .p_cmd=lcd_ili9806e_holit0561_cmd_101,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_102), .p_cmd=lcd_ili9806e_holit0561_cmd_102,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_103), .p_cmd=lcd_ili9806e_holit0561_cmd_103,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_104), .p_cmd=lcd_ili9806e_holit0561_cmd_104,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_105), .p_cmd=lcd_ili9806e_holit0561_cmd_105,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_106), .p_cmd=lcd_ili9806e_holit0561_cmd_106,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_107), .p_cmd=lcd_ili9806e_holit0561_cmd_107,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_108), .p_cmd=lcd_ili9806e_holit0561_cmd_108,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_109), .p_cmd=lcd_ili9806e_holit0561_cmd_109,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_110), .p_cmd=lcd_ili9806e_holit0561_cmd_110,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_111), .p_cmd=lcd_ili9806e_holit0561_cmd_111,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_112), .p_cmd=lcd_ili9806e_holit0561_cmd_112,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_113), .p_cmd=lcd_ili9806e_holit0561_cmd_113,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_114), .p_cmd=lcd_ili9806e_holit0561_cmd_114,},

    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_115), .p_cmd=lcd_ili9806e_holit0561_cmd_115,}, 
    { .type = DSI_CMD_DEALY,	.len = 120, },
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_cmd_116), .p_cmd=lcd_ili9806e_holit0561_cmd_116,},
    { .type = DSI_CMD_DEALY,	.len = 20, },

    { .type = DSI_CMD_END,},

};


static sprdfb_dev_dsi_cmds disp_on = { .type = DSI_CMD_DCS,	.len = 1, .cmd={0x29},};

static sprdfb_dev_dsi_cmds sleep_in[] =
{
    { .type = DSI_CMD_DCS,	.len = 1, .cmd={0x28},},
    { .type = DSI_CMD_DEALY,	.len = 10, },
    { .type = DSI_CMD_DCS,	.len = 1, .cmd={0x10},},
    { .type = DSI_CMD_DEALY,	.len = 120, },
    { .type = DSI_CMD_END,},

};

static sprdfb_dev_dsi_cmds sleep_out[] =
{
    { .type = DSI_CMD_DCS,	.len = 1, .cmd={0x11},},
    { .type = DSI_CMD_DEALY,	.len = 120, },
    { .type = DSI_CMD_DCS,	.len = 1, .cmd={0x29},},
    { .type = DSI_CMD_DEALY,	.len = 20, },
    { .type = DSI_CMD_END,},
};

extern unsigned int lcd_debug_panel_on_cmds( sprdfb_dev_dsi_cmds**cmds, unsigned char** buf) ;

static int32_t ili9806e_holit0561_mipi_init(struct panel_spec *self)
{
    sprdfb_dev_dsi_cmds *dbg_cmds = NULL;
    unsigned char* dbg_data= NULL ;
    unsigned int size = 0;

    mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
    mipi_common_cmd_t mipi_common_cmd = self->info.mipi->ops->mipi_common_cmd;
    mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

    printk(KERN_DEBUG "[kernel]:ili9806e_holit0561_init\n");

    mipi_set_cmd_mode();
    mipi_eotp_set(1,0);

    if( ( size = lcd_debug_panel_on_cmds(&dbg_cmds, &dbg_data) ) > 0 )
    {
        mipi_common_cmd(dbg_cmds);
        kfree(dbg_cmds);
        kfree(dbg_data);
        printk(KERN_INFO "[kernel]:>>>>>>>load sdcard config param success<<<<<<<<< \n");
    }
    else
    {
        mipi_common_cmd(lcd_ili9806e_holit0561_cmds_init);
        printk("[kernel]:>>>>>>>>load local config param success<<<<<<<<<<<<< \n");
    }
    mipi_eotp_set(1,1);
    return 0;
}

static uint32_t ili9806e_holit0561_readid(struct panel_spec *self)
{
    return 0x18;
}

static int32_t ili9806e_holit0561_enter_sleep(struct panel_spec *self, uint8_t is_sleep)
{
    sprdfb_dev_dsi_cmds *sleep_in_or_out = NULL;

    mipi_common_cmd_t mipi_common_cmd = self->info.mipi->ops->mipi_common_cmd;
    mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

    printk(KERN_DEBUG "ili9806e_holit0561_enter_sleep, is_sleep = %d\n", is_sleep);

    if(is_sleep)
    {
        sleep_in_or_out = sleep_in;
    }
    else
    {
        sleep_in_or_out = sleep_out;
    }

    mipi_eotp_set(1,0);
    mipi_common_cmd(sleep_in_or_out);

    return 0;
}

#ifdef CONFIG_FB_ESD_SUPPORT

static uint32_t ili9806e_holit0561_readpowermode(struct panel_spec *self)
{
	  static LCM_Force_Cmd_Code rd_prep_code_1[]={
	    {0x39, {LCM_SEND(8), {0x6, 0, 0xF0,0x55,0xAA,0x52,0x08,0x01}}},
	    {0x37, {LCM_SEND(2), {0x5, 0}}},
    };

    int32_t i = 0;
    uint32_t j =0;
    LCM_Force_Cmd_Code * rd_prepare = rd_prep_code_1;
    uint8_t read_data[1] = {0};
    int32_t read_rtn = 0;
    unsigned int tag = 0;
    uint32_t reg_val_1 = 0;
    uint32_t reg_val_2 = 0;

    mipi_force_write_t mipi_force_write = self->info.mipi->ops->mipi_force_write;
    mipi_force_read_t mipi_force_read = self->info.mipi->ops->mipi_force_read;
    mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;
    mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;

    printk("lcd_ili9806e_holit0561_mipi read power mode!\n");
    mipi_set_cmd_mode();
    mipi_eotp_set(1,0);

    for(j = 0; j < 4; j++)
    {
        rd_prepare = rd_prep_code_1;
        for(i = 0; i < ARRAY_SIZE(rd_prep_code_1); i++)
        {
            tag = (rd_prepare->real_cmd_code.tag >> 24);
            if(tag & LCM_TAG_SEND)
            {
                mipi_force_write(rd_prepare->datatype, rd_prepare->real_cmd_code.data, 
                                 (rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
            }
            else if(tag & LCM_TAG_SLEEP)
            {
                msleep((rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
            }
            rd_prepare++;
        }

        read_rtn = mipi_force_read(0x0A, 1,(uint8_t *)read_data);
        printk("lcd_ili9806e_holit0561 mipi read power mode 0x0A value is 0x%x! , read result(%d)\n",
               read_data[0], read_rtn);
        mipi_eotp_set(1,1);
        if((0 == read_data[0])  && (0 == read_rtn))
        {
            printk("lcd_ili9806e_holit0561_mipi read power mode success!\n");
            return 0x9c;
        }
    }
    return 0x00;
}


static uint32_t ili9806e_holit0561_check_esd(struct panel_spec *self)
{
    uint32_t power_mode;

    printk("ili9806e_holit0561_check_esd!\n");
    mipi_set_lp_mode_t mipi_set_data_lp_mode = self->info.mipi->ops->mipi_set_data_lp_mode;
    mipi_set_hs_mode_t mipi_set_data_hs_mode = self->info.mipi->ops->mipi_set_data_hs_mode;

    mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
    mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
    uint16_t work_mode = self->info.mipi->work_mode;

    if(SPRDFB_MIPI_MODE_CMD==work_mode)
    {
        mipi_set_lp_mode();
    }
    else
    {
        mipi_set_data_lp_mode();
    }
    power_mode = ili9806e_holit0561_readpowermode(self);
    //power_mode = 0x0;
    if(SPRDFB_MIPI_MODE_CMD==work_mode)
    {
        mipi_set_hs_mode();
    }
    else
    {
        mipi_set_data_hs_mode();
    }
    if(power_mode == 0x9c)
    {
        printk("ili9806e_holit0561_check_esd OK!\n");
        return 1;
    }
    else
    {
        printk("ili9806e_holit0561_check_esd fail!(0x%x)\n", power_mode);
        return 0;
    }
}

#endif//-modify by liuwei

static u8 lcd_ili9806e_holit0561_driving_mode_cmd_1[]= {0xFF,0xFF,0x98,0x06,0x04,0x01};
static u8 lcd_ili9806e_holit0561_driving_mode_cmd_2[]= {0x31,0x00};
static u8 lcd_ili9806e_holit0561_driving_mode_cmd_3[]= {0xFF,0xFF,0x98,0x06,0x04,0x00};
static sprdfb_dev_dsi_cmds lcd_driving_mode_control[] =
{
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_driving_mode_cmd_1), .p_cmd=lcd_ili9806e_holit0561_driving_mode_cmd_1,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_driving_mode_cmd_2), .p_cmd=lcd_ili9806e_holit0561_driving_mode_cmd_2,},
    { .type = DSI_CMD_DCS,	.len = sizeof(lcd_ili9806e_holit0561_driving_mode_cmd_3), .p_cmd=lcd_ili9806e_holit0561_driving_mode_cmd_3,},
};
static void ili9806e_holit0561_driving_mode_set(struct panel_spec *self,bool enable)
{
    sprdfb_dev_dsi_cmds *init = lcd_driving_mode_control;
    mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
    mipi_common_cmd_t mipi_common_cmd = self->info.mipi->ops->mipi_common_cmd;
    mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

    if(enable)//dot mode
        lcd_driving_mode_control[1].p_cmd[1] = 0x02;
    else //column mode
        lcd_driving_mode_control[1].p_cmd[1] = 0x00;

    mipi_set_cmd_mode();
    mipi_eotp_set(0,1);

    mipi_common_cmd(init);
    mipi_eotp_set(1,1);

    printk(KERN_INFO "[kernel]:ili9806e_holit0561_driving_mode_set ok.dot enable=%d.\n",enable);
}
static struct panel_operations lcd_ili9806e_holit0561_mipi_operations =
{
    .panel_init = ili9806e_holit0561_mipi_init,
    .panel_readid = ili9806e_holit0561_readid,
    .panel_enter_sleep = ili9806e_holit0561_enter_sleep,
#ifdef CONFIG_FB_ESD_SUPPORT
    .panel_esd_check = ili9806e_holit0561_check_esd,
#endif
    //.lcd_set_driving_mode = ili9806e_holit0561_driving_mode_set,
};

static struct timing_rgb lcd_ili9806e_holit0561_mipi_timing =
{   
    .hfp = 20,  /* unit: pixel */
    .hbp = 20,
    .hsync = 20,
    .vfp = 11, /*unit: line*/
    .vbp = 6,
    .vsync = 4,
};

static struct info_mipi lcd_ili9806e_holit0561_mipi_info =
{
    .work_mode  = SPRDFB_MIPI_MODE_VIDEO,
    .video_bus_width = 24, /*18,16*/
    .lan_number = 2,
    .phy_feq = 320*1000,
    .h_sync_pol = SPRDFB_POLARITY_POS,
    .v_sync_pol = SPRDFB_POLARITY_POS,
    .de_pol = SPRDFB_POLARITY_POS,
    .te_pol = SPRDFB_POLARITY_POS,
    .color_mode_pol = SPRDFB_POLARITY_NEG,
    .shut_down_pol = SPRDFB_POLARITY_NEG,
    .timing = &lcd_ili9806e_holit0561_mipi_timing,
    .ops = NULL,
};

struct panel_spec lcd_ili9806e_holit0561_mipi_spec =
{
    .width  = 480,
    .height = 800,
    .fps    = 60,
    .type   = LCD_MODE_DSI,
    .direction    = LCD_DIRECT_NORMAL,
    .is_clean_lcd = true,
    .suspend_mode = SEND_SLEEP_CMD ,
    .info = {
        .mipi = &lcd_ili9806e_holit0561_mipi_info
    },
    .ops = &lcd_ili9806e_holit0561_mipi_operations,
    .mipi_clk_level = 0,
};

struct panel_cfg lcd_ili9806e_holit0561_mipi =
{
    /* this panel can only be main lcd */
    .dev_id = SPRDFB_MAINLCD_ID,
    .lcd_id = 0x18,
    .lcd_name = "lcd_ili9806e_holit0561_mipi",
    .lcd_voltage_max = 100,
    .lcd_voltage_min = 0,
    .panel = &lcd_ili9806e_holit0561_mipi_spec,
};

static int __init lcd_ili9806e_holit0561_mipi_init(void)
{
    return sprdfb_panel_register(&lcd_ili9806e_holit0561_mipi);
}

subsys_initcall(lcd_ili9806e_holit0561_mipi_init);
