/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-26     balanceTWK        the first version
 * 2022-02-28     jsrdczy           improve multi receive
 */

#ifndef __DECODER_H__
#define __DECODER_H__
#include <rtthread.h>

typedef enum
{
		START_STA = 0,             
		LEAD_CODE_STA,              
		CUSTOM_CODE1_STA,            
		CUSTOM_CODE2_STA,            
		DATA_CODE1_STA,              
		DATA_CODE2_STA,
    REPEAT_STA,	
}nec_sta_enum;


struct nec_data_struct
{
    rt_uint8_t custom1;
    rt_uint8_t custom2;
    rt_uint8_t key1;
    rt_uint8_t key2;
    rt_uint8_t repeat;
};

struct infrared_decoder_data
{
    union 
    {
        struct nec_data_struct    nec;          /* Temperature.         unit: dCelsius    */
    }data;
};

#endif
