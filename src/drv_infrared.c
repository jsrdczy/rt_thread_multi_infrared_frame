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

#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include "infrared.h"
#include "drv_infrared.h"
#include "io_sensor.h"

#define DBG_SECTION_NAME     "drv.infrared"
#define DBG_LEVEL     DBG_INFO
#include <rtdbg.h>

#ifdef PKG_USING_MULTI_INFRARED
#ifdef PKG_USING_DRV_MULTI_INFRARED

#ifdef MULTI_INFRARED_RECEIVE

/* Infrared receiver configuration parameters */
#define RECEIVE_HWTIMER          MULTI_INFRARED_RECEIVE_HWTIMER   /* Timer name */
#define RECEIVE_HWTIMEER_SEC      0
#define RECEIVE_HWTIMEER_USEC     100  /* one decode time cnt 100us */
#define INFRARED_ONETBIT_TIMEOUT   1200  /* 1200 *100 = 120ms */

/*pointer to multi infrared class */
static struct infrared_class *infrared[MULTI_INFRARED_RECEIVE_CHANNEL_NUM];

static rt_device_t           receive_time_dev ;
volatile static rt_uint32_t receive_flag = 0x00000000; /* whether infrared channel is receiving */

/*to receive_pin[0]*/
static void receive_pin1_callback(void* param)
{
    rt_hwtimerval_t receive_time;
    /*judeg infrared channel is recving*/
    if(receive_flag & (1 << 0))
    {
        infrared[0]->decoder.ops->decode((struct decoder_class *)&infrared[0]->decoder,infrared[0]->decode_time);
    }
    else /*start timer*/
    {
        if(!receive_flag)
        {
            receive_flag |= 1 << 0;
            receive_time.sec = RECEIVE_HWTIMEER_SEC;
            receive_time.usec = RECEIVE_HWTIMEER_USEC;
            rt_device_write(receive_time_dev, 0, &receive_time, sizeof(receive_time));
        }
        receive_flag |= 1 << 0;
        /*init decoder time and stat*/
        infrared[0]->decode_time = 0;
        infrared[0]->decoder.ops->init((struct decoder_class *)&infrared[0]->decoder);
    }
}


static void receive_pin2_callback(void* param)
{		
    rt_hwtimerval_t receive_time;
    /*judeg infrared channel is recving*/
    if(receive_flag & (1 << 1))
    {
        infrared[1]->decoder.ops->decode((struct decoder_class *)&infrared[1]->decoder,infrared[1]->decode_time);
    }
    else /*start timer*/
    {
        if(!receive_flag)
        {
            receive_flag |= 1 << 1;
            receive_time.sec = RECEIVE_HWTIMEER_SEC;
            receive_time.usec = RECEIVE_HWTIMEER_USEC;
            rt_device_write(receive_time_dev, 0, &receive_time, sizeof(receive_time));
        }
        receive_flag |= 1 << 1;
        /*init decoder time and stat*/
        infrared[1]->decode_time = 0;
        infrared[1]->decoder.ops->init((struct decoder_class *)&infrared[1]->decoder);
    }			
}

static void receive_pin3_callback(void* param)
{
    rt_hwtimerval_t receive_time;
    /*judeg infrared channel is recving*/
    if(receive_flag & (1 << 2))
    {
        infrared[2]->decoder.ops->decode((struct decoder_class *)&infrared[2]->decoder,infrared[2]->decode_time);
    }
    else /*start timer*/
    {
        if(!receive_flag)
        {
            receive_flag |= 1 << 2;
            receive_time.sec = RECEIVE_HWTIMEER_SEC;
            receive_time.usec = RECEIVE_HWTIMEER_USEC;
            rt_device_write(receive_time_dev, 0, &receive_time, sizeof(receive_time));
        }
            receive_flag |= 1 << 2;
            /*init decoder time and stat*/
            infrared[2]->decode_time = 0;
            infrared[2]->decoder.ops->init((struct decoder_class *)&infrared[2]->decoder);
    }			
}

static void receive_pin4_callback(void* param)
{
    rt_hwtimerval_t receive_time;
    /*judeg infrared channel is recving*/
    if(receive_flag & (1 << 3))
    {
        infrared[3]->decoder.ops->decode((struct decoder_class *)&infrared[3]->decoder,infrared[3]->decode_time);
    }
    else /*start timer*/
    {
        if(!receive_flag)
        {
            receive_flag |= 1 << 3;
            receive_time.sec = RECEIVE_HWTIMEER_SEC;
            receive_time.usec = RECEIVE_HWTIMEER_USEC;
            rt_device_write(receive_time_dev, 0, &receive_time, sizeof(receive_time));
        }
        receive_flag |= 1 << 3;
        /*init decoder time and stat*/
        infrared[3]->decode_time = 0;
        infrared[3]->decoder.ops->init((struct decoder_class *)&infrared[3]->decoder);
    }			
}

static rt_err_t receive_timeout_callback(rt_device_t dev, rt_size_t size)
{
    struct infrared_class *infrared;
    /*check if infrared recv time out*/
    for(rt_uint8_t i = 0; i < MULTI_INFRARED_RECEIVE_CHANNEL_NUM; i++)
    {
        if(receive_flag & (rt_uint32_t)(0x00000001 << i)) /*jugde that infrared channel is receiving*/
        {
            infrared = infrared_find(i);
            /*one bit decode time count and time out*/ 
            ++infrared->decode_time;
            if(infrared->decode_time > INFRARED_ONETBIT_TIMEOUT)
            {
                infrared->decode_time = 0;
                infrared->decoder.ops->init((struct decoder_class *)&infrared->decoder);
                receive_flag &= ~(rt_uint32_t)(0x00000001 << i); /*clear receive flag*/
            }
        }
    }  
    /*no infrared channel recving then stop channel */
    if(!receive_flag)
    {
        rt_device_control(receive_time_dev, HWTIMER_CTRL_STOP, RT_NULL);
    }	
    return 0;
}

/* pin configuration */
/*if need add channel please add pin num , actual num must equal to MULTI_INFRARED_RECEIVE_CHANNEL_NUM*/
const static rt_base_t receive_pin[MULTI_INFRARED_RECEIVE_CHANNEL_NUM] = {GET_PIN(D,4),GET_PIN(D,3),GET_PIN(E,7),GET_PIN(E,5)};
void  (*pin_hdr[MULTI_INFRARED_RECEIVE_CHANNEL_NUM])(void *) = {receive_pin1_callback,receive_pin2_callback,receive_pin3_callback,receive_pin4_callback};

rt_err_t infrared_receive_init(void)
{
    rt_err_t ret = RT_EOK;
    rt_hwtimer_mode_t mode;
    rt_uint32_t freq = 1000000; /*1us 1Mhz*/
    /*pin init*/
    for(uint8_t i = 0;i < MULTI_INFRARED_RECEIVE_CHANNEL_NUM;i++)
    {
        rt_pin_mode(receive_pin[i],PIN_MODE_INPUT_PULLUP);
        rt_pin_attach_irq(receive_pin[i],PIN_IRQ_MODE_FALLING,pin_hdr[i],RT_NULL);
        rt_pin_irq_enable(receive_pin[i],PIN_IRQ_ENABLE);
    }
		
    receive_time_dev = rt_device_find(RECEIVE_HWTIMER);
    if (receive_time_dev == RT_NULL)
    {
        LOG_E("hwtimer sample run failed! can't find %s device!", RECEIVE_HWTIMER);
        return RT_ERROR;
    }
    ret = rt_device_open(receive_time_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        LOG_E("open %s device failed!\n", RECEIVE_HWTIMER);
        return ret;
    }
    rt_device_set_rx_indicate(receive_time_dev, receive_timeout_callback);
    ret = rt_device_control(receive_time_dev, HWTIMER_CTRL_FREQ_SET, &freq);
    if (ret != RT_EOK)
    {
        LOG_E("set frequency failed! ret is :%d", ret);
        return ret;
    }
    mode = HWTIMER_MODE_PERIOD;
    ret = rt_device_control(receive_time_dev, HWTIMER_CTRL_MODE_SET, &mode);
    if (ret != RT_EOK)
    {
        LOG_E("set mode failed! ret is :%d", ret);
        return ret;
    }
    return ret;
}

#endif /* INFRARED_RECEIVE */


int drv_infrared_init()
{
    rt_err_t res = RT_EOK;
		
    for(uint8_t i = 0;i < MULTI_INFRARED_RECEIVE_CHANNEL_NUM;i++)
    {
        infrared[i] = infrared_find(i);
        res = infrared_init(infrared[i]);
    } 		
    if(res < 0)
    {
        rt_kprintf("multi infrared init fail!\r\n");
        return -1;
    }

#ifdef MULTI_INFRARED_RECEIVE
    infrared_receive_init();
#endif /* MULTI INFRARED_RECEIVE */

    return 0;
}
INIT_APP_EXPORT(drv_infrared_init);

#endif /* PKG_USING_DRV_INFRARED */
#endif /* PKG_USING_INFRARED */
