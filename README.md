# 红外框架

## 简介

红外框架，基于 `rt-thread` 的 `pin` , `hwtimer` 驱动实现的复数通道红外接收解码框架。
发送可参考另外一个infrared软件包

## 驱动框架

![multi_infrared_frame](./doc/multi_infrared_frame.png) 

### 目录结构

multi_infrared 软件包目录结构如下所示：

```c 
multi infrared
├───docs                            // 文档
├───inc                             // 头文件
├───src                             // 源文件				
│───LICENSE                         // 软件包许可证
│───README.md                       // 软件包使用说明
└───SConscript                      // RT-Thread 默认的构建脚本
```

###  许可证

multi_infrared 软件包遵循 Apache-2.0 许可，详见 LICENSE 文件。

### 获取软件包

使用 multi_infrared 软件包需要在 RT-Thread 的包管理中选中它，具体路径如下：

```c
RT-Thread online packages
    peripheral libraries and drivers  --->
         [*] multi_infrared : multi_infrared is base on rt-thread pin,hwtimer.  --->   
			   Select multi infrared decoder  --->                                 
		 [*]   Use the multi infrared driver provided by the software package
		 [*]     Enable multi infrared receive                               
		 (timer5)  multi infrared receive hwtimer dev name            
		 (4)       multi infrared receive channel nums                  
			   Version (latest)  --->
```

decoder 解码器选择nec
```c
[*] enable nec decoder
```

### 使用示例

```c
    /* 选择解码器 */
	static struct infrared_class *infrared[MULTI_INFRARED_RECEIVE_CHANNEL_NUM];
    struct infrared_decoder_data infrared_data[MULTI_INFRARED_RECEIVE_CHANNEL_NUM];
	
	/* infrared init*/
	for(uint8_t i = 0;i < MULTI_INFRARED_RECEIVE_CHANNEL_NUM;i++)
	{
		infrared[i] = infrared_find(i);
	}
	
    while (1)
    {
        /* 读取数据 */
	  for(uint8_t i = 0;i < MULTI_INFRARED_RECEIVE_CHANNEL_NUM;i++)
	  {
		if(infrared_read(infrared[i],&infrared_data[i]) == RT_EOK)
		{
			if(infrared_data[i].data.nec.repeat)
			{
				if(is_print&0x04)
						rt_kprintf("channel %d repeat %02X %02X %02X %02X %d\n",i,infrared_data[i].data.nec.custom1,infrared_data[i].data.nec.custom2, \
						infrared_data[i].data.nec.key1,infrared_data[i].data.nec.key2,infrared_data[i].data.nec.repeat);                                                                                                                 
			}
			else
			{
				if(is_print&0x04)
						rt_kprintf("channel %d %02X %02X %02X %02X\n",i,infrared_data[i].data.nec.custom1,infrared_data[i].data.nec.custom2, \
						infrared_data[i].data.nec.key1,infrared_data[i].data.nec.key2);                                                                                                                 
			}
			*(uint8_t *)((uint8_t *)&io_handle.infrared_data + i) = infrared_data[i].data.nec.key1;
		}
	  }	  
      rt_thread_mdelay(10);
    }
```

### 联系方式 & 感谢

- 维护：jsrdczy
- 主页：https://github.com/jsrdczy/rt_thread_multi_infrared_frame.git
