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

#include <infrared.h>
#include "ipc/ringbuffer.h"

#define DBG_SECTION_NAME     "nec.decoder"
#define DBG_LEVEL     DBG_INFO
#include <rtdbg.h>

#ifdef MULTI_INFRARED_NEC_DECODER

#define NEC_BUFF_SIZE  8

/* unit100us */
#define IR_DECODER_LEAD_TIMER_MAX     138       //137 * 100 = 13.7ms 
#define IR_DECODER_LEAD_TIMER_MIN     132       //133 * 100 = 13.3ms   //average 13.5ms = 9 + 4.5
#define IR_DECODER_REPEAT_TIMER_MAX   118       //117 * 100 = 11.7ms 
#define IR_DECODER_REPEAT_TIMER_MIN   112       //113 * 100 = 11.3ms   //average 11.5ms = 9 + 2.5
#define IR_DECODER_LOW_TIMER_MAX      13        // 13 * 100 = 1.3ms  
#define IR_DECODER_LOW_TIMER_MIN      7         // 10 * 100 = 0.9ms    //actual time 1.12 ms
#define IR_DECODER_HIGH_TIMER_MAX     23        // 18 * 100 = 1.8ms
#define IR_DECODER_HIGH_TIMER_MIN     17        // 14 * 100 = 1.4ms    //actual time 1.68ms + 560 us


/*init decoder mem, if mem init ok , then return ok */
static rt_err_t nec_decoder_init(struct decoder_class *decoder)
{
    RT_ASSERT(decoder != RT_NULL);
	  if(!decoder->ringbuff)
		{
				decoder->ringbuff = rt_ringbuffer_create(sizeof(struct nec_data_struct) * NEC_BUFF_SIZE);
		}
	
	  decoder->decode_cnt = 0;
		decoder->decode_size = 0;
		decoder->decode_size_curr = 0;
	  decoder->nec_state = START_STA;
	  rt_memset(&decoder->nec_data,0,sizeof(struct nec_data_struct)); /*clear nec status and data*/
	  return RT_EOK;
}
/*free decoder*/
static rt_err_t nec_decoder_deinit(struct decoder_class *decoder)
{
    RT_ASSERT(decoder != RT_NULL);
	
    rt_ringbuffer_destroy(decoder->ringbuff);
	  decoder->decode_cnt = 0;
	  decoder->decode_size = 0;
	  decoder->decode_size_curr = 0;
	  decoder->nec_state = START_STA;
	  rt_memset(&decoder->nec_data,0,sizeof(struct nec_data_struct)); /*clear nec status and data*/
    return RT_EOK;
}
/*read data from decoder*/
static rt_err_t nec_decoder_read(struct decoder_class *decoder,struct infrared_decoder_data *nec_data)
{
    if (rt_ringbuffer_get(decoder->ringbuff, (rt_uint8_t *) & (nec_data->data.nec), sizeof(struct nec_data_struct)) == sizeof(struct nec_data_struct))
    {
        LOG_D("NEC addr:0x%01X key:0x%01X repeat:%d", nec_data->data.nec.addr, nec_data->data.nec.key, nec_data->data.nec.repeat);
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

static rt_err_t nec_decoder_control(struct decoder_class *decoder,int cmd, void *arg)
{
    return RT_EOK;
}

/*nec_decoder*/
static rt_err_t nec_decoder_decode(struct decoder_class *decoder,rt_uint32_t decode_time)
{
    struct infrared_class *infrared = decoder->user_data;

    switch(decoder->nec_state)
    {
        case START_STA: /* judge lead code or repeat code */
        {
            /*is lead code*/ 
            if(decode_time >= IR_DECODER_LEAD_TIMER_MIN && decode_time <= IR_DECODER_LEAD_TIMER_MAX)
            {
                decoder->nec_state = LEAD_CODE_STA;
                decoder->decode_cnt = 0;
                decoder->nec_data.repeat = 0;
                decoder->decode_size = 4;
                decoder->decode_size_curr = 0;
            }/*is repeat code*/ 
            else if(decode_time >= IR_DECODER_REPEAT_TIMER_MIN && decode_time <= IR_DECODER_REPEAT_TIMER_MAX)
            {
                decoder->nec_state = START_STA; /*return to sta and repeat ++*/
                decoder->nec_data.repeat++;
                rt_ringbuffer_put(decoder->ringbuff, (rt_uint8_t *)&(decoder->nec_data), sizeof(struct nec_data_struct));
            }
            else
            {
                decoder->nec_state = START_STA;
            }
            infrared->decode_time = 0;  /*clear decode time for next decode */
        }
        break;
        case LEAD_CODE_STA:
        {
            if(decode_time >= IR_DECODER_HIGH_TIMER_MIN && decode_time <= IR_DECODER_HIGH_TIMER_MAX) /* bit 1 */
            {
                *(rt_uint8_t *)((rt_uint8_t *)&decoder->nec_data + decoder->decode_size_curr) >>= 1;
                *(rt_uint8_t *)((rt_uint8_t *)&decoder->nec_data + decoder->decode_size_curr) |= 0x80;
                decoder->decode_cnt++;
            }
            else if(decode_time >= IR_DECODER_LOW_TIMER_MIN && decode_time <= IR_DECODER_LOW_TIMER_MAX) /*bit 0*/
            {
                *(rt_uint8_t *)((rt_uint8_t *)&decoder->nec_data + decoder->decode_size_curr) >>= 1;
                *(rt_uint8_t *)((rt_uint8_t *)&decoder->nec_data + decoder->decode_size_curr) |= 0x00;
                decoder->decode_cnt++;
            }
            else /* time no match ,init receive*/
            {
                decoder->decode_cnt = 0;
                decoder->nec_state = START_STA;
                decoder->decode_size_curr = 0;
                decoder->decode_size = 0;
            }
            infrared->decode_time = 0;  /*clear decode time for next decode*/
            if(decoder->decode_cnt >= 8) /* bit num 8 reach*/ 
            {
                decoder->decode_cnt = 0;
                ++decoder->decode_size_curr;
                if(decoder->decode_size_curr >= decoder->decode_size)
                {
                    decoder->nec_state = START_STA;
                    decoder->decode_size_curr = 0;
                    decoder->decode_size = 0;
                    rt_ringbuffer_put(decoder->ringbuff, (rt_uint8_t *)&(decoder->nec_data), sizeof(struct nec_data_struct));
                }
            }
        }
        break;
		default:
        break;
    }			
    return RT_EOK;
}

static rt_err_t nec_decoder_write(struct decoder_class *decoder,struct infrared_decoder_data *data)
{
    return RT_EOK;
}
//register nec decode to infrared*/
static struct decoder_ops ops;

int nec_decoder_register()
{
    struct infrared_class *infrared;
	  
    ops.control = nec_decoder_control;
    ops.decode = nec_decoder_decode;
    ops.init = nec_decoder_init;
    ops.deinit = nec_decoder_deinit;
    ops.read = nec_decoder_read;
    ops.write = nec_decoder_write;	
	  
    for(rt_uint8_t i = 0;i < MULTI_INFRARED_RECEIVE_CHANNEL_NUM;i++)
    {
        infrared = infrared_find(i);
        infrared->decoder.name = "nec";
        infrared->decoder.ops = &ops;
        infrared->decoder.user_data = infrared;
    }
    return 0;
}
INIT_PREV_EXPORT(nec_decoder_register);
#endif /* INFRARED_NEC_DECODER */


