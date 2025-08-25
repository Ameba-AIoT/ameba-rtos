# Ameba Audio

## Table of Contents

- [Ameba Audio](#ameba-audio)
	- [Table of Contents](#table-of-contents)
	- [About](#about)
	- [Supported IC](#supported-ic)
	- [Configurations](#configurations)
		- [Hardware configurations](#hardware-configurations)
	- [Software Patches](#software-patches)
		- [A2dp Hal](#hal-patch)
		- [A2dp Demo](#a2dp-patch)
	- [How to run](#how-to-run)

## About

Ameba audio project can achieve:
1. if a2dp connected, using chip as a2dp source, and play audio through a2dp.
2. if a2dp disconnected, play through speaker.

## Supported IC
1. AmebaSmart

## Configurations

Please see discriptions in component/soc/usrcfg/xx/include/ameba_audio_hw_usrcfg.h, ameba_audio_hw_usrcfg.h is for audio hardware configurations.
Please see discriptions in component/audio/configs/include/ameba_audio_mixer_usrcfg.h, and setup component/audio/configs/ameba_audio_mixer_usrcfg.cpp.

Enter directory:amebasmart_gcc_project
./menuconfig.py--->CONFIG APPLICATION--->AUDIO CONFIG--->Select Audio Interface(Mixer)
./menuconfig.py--->CONFIG BT--->BT Example Demo--->BT A2DP

### Hardware configurations

1. Setup the hardware pins, like amplifier pins and so on.
2. Define whether using pll clock or xtal clock for playback.
3. Please refer to component/audio/audio_hal/xx/README.md.

## Software Patches
### A2dp Hal

1. Edit file: component/audio/audio_hal/a2dp/a2dp_audio_hw_stream_out.c
1. add headers:
```c
#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <rtk_bt_common.h>
#include <bt_api_config.h>
#include <rtk_bt_br_gap.h>
#include <rtk_bt_sdp.h>
#include <rtk_bt_avrcp.h>
#include <rtk_bt_a2dp.h>
#include <bt_audio_intf.h>
#include <bt_audio_track_api.h>
#include <sbc_codec_entity.h>
```

2. add extern variables:
```c
extern void *a2dp_demo_codec_entity;
extern uint8_t remote_bd_addr[6];
extern rtk_bt_sbc_codec_t sbc_codec_t;
extern uint8_t src_a2dp_credits;
```

3. implement A2dpWrite interface:
```c
static ssize_t A2dpWrite(struct AudioHwStreamOut *stream, const void *buffer, size_t bytes, bool block)
{
    (void) block;
    rtk_bt_a2dp_stream_data_send_t data_send_t = {0};
    struct enc_codec_buffer *enc_buffer = NULL;
    int res = bytes;

    if (!a2dp_demo_codec_entity) {
        return HAL_OSAL_ERR_INVALID_OPERATION;
    }

    struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;

    enc_buffer = rtk_bt_audio_data_encode(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_codec_entity, (short *)buffer, bytes);
    if (enc_buffer) {
        memset((void *)&data_send_t, 0, sizeof(rtk_bt_a2dp_stream_data_send_t));
        memcpy((void *)data_send_t.bd_addr, (void *)remote_bd_addr, 6);
        data_send_t.seq_num = out->sequence;
        data_send_t.frame_buf = (uint8_t *)enc_buffer->pbuffer;
        data_send_t.frame_num = (uint8_t)enc_buffer->frame_num;
        data_send_t.time_stamp += data_send_t.frame_num * sbc_codec_t.encoder_t.subbands * sbc_codec_t.encoder_t.blocks;
        data_send_t.len = (uint16_t)(enc_buffer->frame_num * enc_buffer->frame_size);
        data_send_t.flush = false;
        res = rtk_bt_a2dp_data_send(&data_send_t);
        if (res) {
            HAL_AUDIO_ERROR("[A2DP] data send fail %d \r\n", bytes);
        } else {
            HAL_AUDIO_VERBOSE("A2dp send:%d, ret:%d, seq:%d, fb:%p, fn:%d, ts:%ld, len:%d",
                                   bytes, res, data_send_t.seq_num, data_send_t.frame_buf, data_send_t.frame_num, data_send_t.time_stamp, data_send_t.len);
        }
        rtk_bt_audio_free_encode_buffer(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_codec_entity, enc_buffer);
        out->sequence++;
    } else {
        HAL_AUDIO_ERROR("[A2DP SRC Demo]: Encode fail \r\n");
    }

    return res;
}
```

4. implement A2dpGetSrcA2dpCredits interface:
```c
static int8_t A2dpGetSrcA2dpCredits(void)
{
    return src_a2dp_credits;
}
```

5. implement A2dpSrcA2dpCreditsDec interface:
```c
static void A2dpSrcA2dpCreditsDec(void)
{
    src_a2dp_credits--;
}
```

### A2dp Demo

patch a2dp.patch to component/bluetooth directory.

## How to run

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

### Note

1. Use CMD `./build.py -a audio_track` to compile track example.

2. One smart board as source, one smart board as slave, running same image.

3. After system boot, sink cmd:
   'AT+BTDEMO=a2dp,snk,1'

4. After system boot, source cmd:
   'AT+BTDEMO=a2dp,src,1'

5. After above steps, we can get bluetooth mac address through logs.

6. Source board connect to sink's bluetooth mac:
   'AT+BTA2DP=conn,00e04c800499'
   'AT+BTA2DP=start,00e04c800499'

7. Source board run:
   'audio_track -r 16000 -c 2 -f 16'


8. After all above steps, we will here sine wave through sink board.






