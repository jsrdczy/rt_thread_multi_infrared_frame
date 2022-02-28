/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-25     balanceTWK   the first version
 * 2022-02-28     jsrdczy      improve multi receive
 */
 
#ifndef __INFRARED__
#define __INFRARED__

#include <rtthread.h>
#include "decoder.h"

#define MAX_SIZE             5
#define INFRARED_BUFF_SIZE   200

struct ir_raw_data
{
    rt_uint32_t decoder_time;  /*unit 100us*/
};

/*nec decoder class */
struct decoder_class 
{
    char* name;
    struct decoder_ops* ops;
	
	  /*nec decoder status*/
    nec_sta_enum nec_state;
	  rt_uint8_t decode_cnt;  //bit num in one byte
	  rt_uint8_t decode_size; //bytes bum in one frame
	  rt_uint8_t decode_size_curr; // current recv bytes num in one frame
	  struct nec_data_struct nec_data;
	
	  struct rt_ringbuffer *ringbuff;
    void* user_data;
};

struct decoder_ops
{
    rt_err_t (*init)(struct decoder_class *decoder);
    rt_err_t (*deinit)(struct decoder_class *decoder);
    rt_err_t (*read)(struct decoder_class *decoder,struct infrared_decoder_data* data);
    rt_err_t (*write)(struct decoder_class *decoder,struct infrared_decoder_data* data);
    rt_err_t (*decode)(struct decoder_class *decoder,rt_uint32_t decode_time);
    rt_err_t (*control)(struct decoder_class *decoder,int cmd, void *arg);
};

struct infrared_class
{
	  struct decoder_class decoder; //decoder class
	  volatile rt_uint32_t decode_time;  /*one carrier wave and one idle time total*/
    rt_size_t (*send)(struct ir_raw_data* data, rt_size_t size);
};

/*multi infrared*/
/*decoder register*/
struct infrared_class *infrared_find(rt_uint8_t idx);
/*init and deinit infrared class */
rt_err_t infrared_init(struct infrared_class *infrared);
int  infrared_deinit(struct infrared_class *infrared);
/*decoder read data or write data from infrared ringbuffer*/
rt_err_t decoder_write_data(struct infrared_class *infrared,struct ir_raw_data* data, rt_size_t size);
/*infrared read and write data*/
rt_err_t infrared_read(struct infrared_class *infrared,struct infrared_decoder_data* data);
rt_err_t infrared_write(struct infrared_class *infrared, struct infrared_decoder_data* data);

#endif /* __INFRARED__ */
