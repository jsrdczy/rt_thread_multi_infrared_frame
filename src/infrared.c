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

#define DBG_SECTION_NAME     "infrared"
#define DBG_LEVEL     DBG_INFO
#include <rtdbg.h>

/*multi infrared class */
static struct infrared_class infrared[MULTI_INFRARED_RECEIVE_CHANNEL_NUM];

/*find infrared class according to idx*/
/* if add channel add infrared pointer here!!! note channel num must be equal to MULTI_INFRARED_RECEIVE_CHANNEL_NUM*/
struct infrared_class *infrared_find(rt_uint8_t idx)
{
    struct infrared_class *res = RT_NULL;
    switch(idx)
    {
        case 0:
        res = &infrared[0];
        break;
				
        case 1:
        res = &infrared[1];
        break;
				
        case 2:
        res = &infrared[2];
        break;
				
        case 3:
        res = &infrared[3];
        break;
				
        default:
        break;
    }
		
    return res;
}

rt_err_t infrared_init(struct infrared_class *infrared)
{
    RT_ASSERT(infrared != RT_NULL);
		
    infrared->decode_time = 0;
    return RT_EOK;
}

rt_err_t decoder_write_data(struct infrared_class *infrared,struct ir_raw_data* data, rt_size_t size)
{
    infrared->send(data, size);
    return RT_EOK;
}

rt_err_t infrared_read(struct infrared_class *infrared,struct infrared_decoder_data* data)
{
    if(infrared->decoder.ops->read)
    {
        return infrared->decoder.ops->read((struct decoder_class *)&infrared->decoder,data);
    }
    return -RT_ERROR;
}

rt_err_t infrared_write(struct infrared_class *infrared,struct infrared_decoder_data* data)
{
    if(infrared->decoder.ops->write)
    {
        return infrared->decoder.ops->write((struct decoder_class *)&infrared->decoder,data);
    }
    return -RT_ERROR;
}

