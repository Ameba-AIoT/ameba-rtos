# A2DP Sink + HFP HandsFree + LE Audio PBP Source Demo

> **Note:** This example is supported on **RTL8730E** only.

---

## Compile Config

### Common Config

Change `kPrimaryAudioConfig` in `component/audio/configs/ameba_audio_mixer_usrcfg.cpp`:

```c
// change from:
kPrimaryAudioConfig = {1024, 4, AUDIO_OUT_MIN_FRAMES_STAGE1};
// to:
kPrimaryAudioConfig = {240, 4, AUDIO_OUT_MIN_FRAMES_STAGE2};
```

### LE Audio Stream Config

Select **one** of the following configurations and change the corresponding macros in `rtk_bt_le_audio_def.h`:

1. BIS num 1 + Sample rate 16kHz + 2-channel + ISO interval 20 ms

   > change `RTK_BT_LE_AUDIO_BIG_ISO_INTERVAL_CONFIG` to `RTK_BT_ISO_INTERVAL_20_MS`

   > change `RTK_BT_LE_AUDIO_COMPO_DEMO_AUDIO_STREAM_SAMPLE_RATE` to `RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K`

   > change `RTK_BT_LE_AUDIO_COMPO_DEMO_DEFAULT_BIS_CODEC_CFG` to `RTK_BT_LE_CODEC_CFG_ITEM_16_2`

2. BIS num 1 + Sample rate 48kHz + 1-channel + ISO interval 20 ms

   > change `RTK_BT_LE_AUDIO_BIG_ISO_INTERVAL_CONFIG` to `RTK_BT_ISO_INTERVAL_20_MS`

   > change `RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE` to `0`

3. BIS num 1 + Sample rate 48kHz + 2-channel + ISO interval 30 ms

   > change `RTK_BT_LE_AUDIO_BIG_ISO_INTERVAL_CONFIG` to `RTK_BT_ISO_INTERVAL_30_MS`

4. BIS num 2 + Sample rate 48kHz + 2-channel + ISO interval 30 ms

   > change `RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM` to `2`

   > change `RTK_BT_LE_AUDIO_BIG_ISO_INTERVAL_CONFIG` to `RTK_BT_ISO_INTERVAL_30_MS`

### Optional: Local Play Config

To enable audio local playback, change the following macros before testing:

Change in `rtk_bt_le_audio_def.h`:

```c
RTK_BLE_AUDIO_BROADCAST_LOCAL_PLAY_SUPPORT   to   1
RTK_BT_LE_AUDIO_ISO_TX_SYNC_SUPPORT          to   1
```

Change in `bt_vendor_config.h`:

```c
VENDOR_CMD_GET_LE_ISO_SYNC_REF_AP_INFO_SUPPORT   to   1
```

Change in `bt_api_config.h`:

```c
RTK_BT_GET_LE_ISO_SYNC_REF_AP_INFO_SUPPORT   to   1
```

Unmask the following code in `bt_audio_track_api.c`:

```c
// change from:
//track_buf_size = 22608;
// to:
track_buf_size = 22608;
```

---

## GCC menuconfig

#### Manual (TUI)

1. BT: `./ameba.py menuconfig` --> `CONFIG BT` --> `Enable BT` --> `BT Mode Selection (DUAL_MODE)` --> `BT Example Demo` --> `BT A2DP HFP and LE Audio PBP`

2. (Optional, for PBP sink peer device) BT: `./ameba.py menuconfig` --> `BT Example Demo` --> `BLE Audio` --> `BLE Audio Public Broadcast Profile`

3. Audio: `./ameba.py menuconfig` --> `CONFIG Application` --> `Audio Config` --> `Enable Audio Framework` --> `Select Audio Interfaces (Mixer)`

4. Audio: `./ameba.py menuconfig` --> `Audio Config` --> `Third Party Libraries` --> `speexdsp`

#### CLI / MCP (menuconfig --set)

> **CLI:** `./ameba.py menuconfig --set SYMBOL=VALUE [...]`  
> **MCP (ameba-dev):** call `kconfig_set` tool with symbols below as `assignments` list

| # | Symbol | Value | Note |
|---|--------|-------|------|
| 1 | `CONFIG_BT_MENU` | `y` | |
| 2 | `CONFIG_BT_DUAL_MODE` | `y` | RTL8730E defaults to DUAL_MODE |
| 3 | `CONFIG_BT_EXAMPLE_DEMO_MENU` | `y` | |
| 4 | `CONFIG_BT_A2DP_HFP_PBP_MENU` | `y` | |
| 5 | `CONFIG_AUDIO_FWK_MENU` | `y` | |
| 6 | `CONFIG_AUDIO_MIXER_MENU` | `y` | Select Audio Interface: Mixer |
| 7 | `CONFIG_THIRD_PARTY_SPEEXDSP_MENU` | `y` | Required for HFP echo cancellation |

---

## Build

#### Manual

```bash
./ameba.py build
```

#### CLI

```bash
./ameba.py build -q
```

#### MCP (ameba-dev)

Use `build_firmware` tool; SoC must be selected beforehand via `set_target`.

---

## Flash

#### Manual / CLI

```bash
./ameba.py flash
```

#### MCP (ameba-dev)

Use `flash_firmware` tool; SoC and image directory are resolved from `set_target`.

---

## ATCMD Reference

1. A2DP sink + AVRCP + HFP handfree + PBAP + Auracast demo

   1.1 enable  `AT+BTDEMO=a2dp_hfp_pbp,1`

   1.2 disable  `AT+BTDEMO=a2dp_hfp_pbp,0`

   1.3 use Phone to connect to the device named "A2DP HFP Auracast"

   1.4 start LE Audio PBP broadcast  `AT+BLEBAP=broadcast_start`

   1.5 stop LE Audio PBP broadcast  `AT+BLEBAP=broadcast_stop`

   **Role switch test, idle state --> broadcast source(BMS) --> broadcast sink(BMR):**

   1.6 start LE Audio PBP broadcast  `AT+BLEBAP=broadcast_start`

   1.7 stop LE Audio PBP broadcast  `AT+BLEBAP=broadcast_stop`

   > **Note:** must stop broadcast! otherwise will cause insufficient bandwidth!

   1.8 start scan  `AT+BLEBAP=escan,1`

   1.9 stop scan  `AT+BLEBAP=escan,0`

   1.10 create PA sync and BIG sync to the PBP broadcast  `AT+BLEBAP=pa_sync_create,<bd_addr type>,<bd_addr>,<adv_sid>,<broadcast_id>`

   > e.g. `AT+BLEBAP=pa_sync_create,0,00e04c8002eb,0xe,123abc`

   **Role switch test, idle state --> broadcast sink(BMR) --> broadcast source(BMS):**

   1.11 start scan for the PBP broadcast  `AT+BLEBAP=escan,1`

   1.12 stop scan  `AT+BLEBAP=escan,0`

   1.13 create PA sync and BIG sync to the PBP broadcast  `AT+BLEBAP=pa_sync_create,<bd_addr type>,<bd_addr>,<adv_sid>,<broadcast_id>`

   > e.g. `AT+BLEBAP=pa_sync_create,0,00e04c8002eb,0xe,123abc`

   1.14 terminate BIG sync to the PBP broadcast  `AT+BLEBAP=big_sync_remove,<bd_addr type>,<bd_addr>`

   > **Note:** must terminate BIG sync before start broadcast!

   1.15 start LE Audio PBP broadcast  `AT+BLEBAP=broadcast_start`

   **For HFP HF part:**

   1.16 HFP HF call answer  `AT+BTHFP=call_answer,<peer_phone_bd_addr>`

   1.17 HFP HF call terminate  `AT+BTHFP=call_terminate,<peer_phone_bd_addr>`

   **For AVRCP part:**

   1.18 get album information, artist name, etc:

   ```
   AT+BTAVRCP=element_attr,<bd_addr>,1
   AT+BTAVRCP=element_attr,<bd_addr>,2
   AT+BTAVRCP=element_attr,<bd_addr>,3
   AT+BTAVRCP=element_attr,<bd_addr>,4
   AT+BTAVRCP=element_attr,<bd_addr>,5
   AT+BTAVRCP=element_attr,<bd_addr>,6
   AT+BTAVRCP=element_attr,<bd_addr>,7
   ```

   get cover art:

   ```
   AT+BTAVRCP=cover_art_conn,<bd_addr>
   AT+BTAVRCP=element_attr,<bd_addr>,8
   ```

   **For PBAP part:**

   1.19 connect  `AT+BTPBAP=conn,<bd_addr>`

   1.20 disconnect  `AT+BTPBAP=disconn,<bd_addr>`

   1.21 get phone book object  `AT+BTPBAP=get_pb_size,<bd_addr>,<repository>,<phone_book_object>`

   > `<repository>` refer to `rtk_bt_pbap_repository`

   > `<phone_book_object>` refer to `RTK_BT_PBAP_PROPERTY_MASK`

   > For example, get local repository Main book object: `AT+BTPBAP=get_pb_size,fc5b8c1aedd8,1,0`

   1.22 get phone book detailed information  `AT+BTPBAP=pull_pb,<bt_mac>,<repository>,<phone_book_object>,<offset>,<size>,<mask>`

   > For example: `AT+BTPBAP=pull_pb,fc5b8c1aedd8,1,0,0,128,0x82`

2. PBP sink demo (peer device)

   2.1 enable  `AT+BTDEMO=pbp,sink,1,<sound channel>`

   > sound_channel: {left, right, stereo}

   2.2 stop ext adv before ext scan  `AT+BLEGAP=eadv,0,0`

   2.3 scan for the PBP broadcast  `AT+BLEBAP=escan,1`

   2.4 create PA sync to the PBP broadcast  `AT+BLEBAP=pa_sync_create,<bd_addr type>,<bd_addr>,<adv_sid>,<broadcast_id>`

   > e.g. `AT+BLEBAP=pa_sync_create,0,00e04c8002eb,0xe,123abc`

   2.5 create BIG sync to the PBP broadcast  `AT+BLEBAP=big_sync_create,<bd_addr type>,<bd_addr>`

3. Phone

   3.1 Use Phone to connect to the device "A2DP HFP Auracast"

   3.2 Play music on the Phone

   3.3 Receive Phone call by HFP HF
