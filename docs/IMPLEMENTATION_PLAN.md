# NeoWatch — Full Requirements Implementation Plan

**Status:** Approved for implementation  
**Created:** 2026-04-09  
**Plan Location:** `C:\Users\Hieu\.claude\plans\valiant-puzzling-wirth.md`

---

## Context

The NeoWatch firmware has a well-defined architecture across 6 SDDs but the current code is in a hardware-test/scaffolding state:

- `system_manager_init()` is commented out in `main.c` — the device boots into an infinite LED color loop
- `bsp_i2c_init()` is never called → `bsp_lcd_init()` receives a NULL I2C handle → guaranteed crash if display starts
- `system_display_task()` is not a FreeRTOS task and is never spawned
- `system_display_update()` is declared in the header but has **zero implementation**
- `bsp_lcd_clock_display()` passes hardcoded `(1,1,1,1,1,1)` instead of reading real RTC time
- Button HOLD uses `BUTTON_LONG_PRESS_HOLD` (periodic) instead of `BUTTON_LONG_PRESS_START` (one-shot at 1s)
- System Network subsystem (BLE) is completely unimplemented
- STM32-era middleware files (`cbuffer`, `snake_game`, `fonts`) reference non-existent headers

**Goal:** Fix these issues in priority order so the device runs a real clock display, button navigation, BLE app communication, and NVS-persisted settings.

---

## Phase 1: Critical Boot Fixes

**All items here are blockers — nothing else works until these are done.**

### 1.1 Fix I2C init order — `source/src/bsp/bsp.c`

Add `bsp_i2c_init()` call before any LCD init. Clean up commented-out dead code.

```c
void bsp_init(void) {
    bsp_flash_init();
    bsp_i2c_init();    // ← MUST precede any bsp_lcd call
    bsp_rtc_init();
    bsp_gpio_init();
    bsp_button_init();
    // NOTE: bsp_lcd_init() is owned by system_display_init() — do NOT call here
}
```

### 1.2 Uncomment `system_manager_init()` — `main/main.c`

Remove `system_ui_test()` (infinite LED loop) and uncomment the manager init.

```c
void app_main(void) {
    bsp_init();
    system_manager_init();
}
```

### 1.3 Uncomment `system_display_init()` — `source/src/system/system_manager.c`

The call is commented out on line ~54. Uncomment it.

**Verify Phase 1:** Logs show I2C init, SSD1306 panel driver success, LVGL task start, system boots to STANDBY.

---

## Phase 2: Core Behavioral Fixes

### 2.1 Fix button HOLD event — `source/src/driver/drv_button.c`

Change the mapping in `drv_button_map_event()`:

```c
case DRV_BUTTON_EVENT_HOLD:
    return BUTTON_LONG_PRESS_START;  // was BUTTON_LONG_PRESS_HOLD (periodic — wrong)
```

Update `drv_button_get_event()` to match (swap the return values for HOLD vs LONG_PRESS_HOLD cases).

Set long_press_time threshold to 1000 ms in `drv_button_init()`:

```c
button_config_t btn_cfg = { .long_press_time = 1000, .short_press_time = 0 };
```

### 2.2 Implement Display as a real FreeRTOS task — `source/src/system/system_display.c`

**2.2a** Add static `QueueHandle_t s_display_queue = NULL` (depth 32).

**2.2b** Add public function:

```c
base_status_t system_display_send_event(system_display_event_t event_id,
                                        uint8_t brightness, uint8_t mode);
```

For `SYS_DISPLAY_EVENT_REFRESH`: drop oldest if queue full (coalesce). All others: reject-newest (timeout 0).

**2.2c** In `system_display_init()`, after BSP inits: create queue and spawn task:

```c
s_display_queue = xQueueCreate(32, sizeof(sm_event_msg_t));
xTaskCreate(system_display_task_fn, "sys_display", 4096, NULL, 6, NULL);
```

**2.2d** Implement `system_display_task_fn()` — consume `Q_DIS` events:

- `MODE_BOOT/STANDBY/MENU/SETTING` → update internal mode state, call `system_display_update()`
- `REFRESH` → call `system_display_update()`
- `BRIGHTNESS_SET` → call `bsp_lcd_clock_set_mode()` (or brightness API) with `evt.data.display.brightness`
- `SLEEP/WAKE` → call ESP-IDF panel on/off API

**2.2e** Implement `system_display_update()` (currently declared but body missing):

```c
base_status_t system_display_update(void) {
    time_t epoch = bsp_rtc_get_time();
    epoch += (time_t)(system_settings_get_time_zone() * 3600);  // apply timezone
    bsp_rtc_time_t t = bsp_rtc_epoch_to_time(epoch);
    return bsp_lcd_clock_display(t.year, t.month, t.day, t.hour, t.min, t.sec);
}
```

Add prototype to `source/inc/system/system_display.h`.

### 2.3 Fix `bsp_lcd_clock_display()` — `source/src/bsp/bsp_lcd.c`

Currently calls `ui_init()` unconditionally on every call and ignores parameters. Fix:

- Create LVGL clock label **once** (lazy init with a static flag)
- On each call update label with `lv_label_set_text_fmt()` using the passed `year, month, ...`
- Uncomment the existing label-based code block (lines 282–293) and wire it to a one-time init
- Hold `lvgl_api_lock` around all `lv_*` calls (already done in surrounding code — preserve this)

### 2.4 Wire mode transitions to Display queue — `source/src/system/system_manager.c`

`sm_set_mode()` has comment `/* Future: post mode event to Q_DIS here */`. Implement it:

```c
static void sm_set_mode(sm_mode_t mode) {
    s_current_mode = mode;
    system_ui_send_cmd(ui_cmd_for_mode(mode), 0, 0);
    system_display_send_event(display_event_for_mode(mode), 0, (uint8_t)mode);
}
```

**Verify Phase 2:** Real time displayed on screen, ticking every second. Single HOLD fires exactly once. Mode transitions render on display.

---

## Phase 3: Settings Event-Driven Architecture

### 3.1 Add Q_SET queue and task — `source/src/system/system_settings.c`

Add:

- `static QueueHandle_t s_settings_queue = NULL`
- Public `system_settings_send_cmd(system_settings_cmd_t cmd, ...)` that posts to queue
- `system_settings_task()` FreeRTOS task that consumes queue
- Dispatch table per command (see SDD):

| Command | Action | Publishes back |
|---------|--------|----------------|
| `SET_BRIGHTNESS` | validate 0–100, save NVS | `SYS_SET_EVT_UPDATED` |
| `SET_CLOCK_TYPE` | validate enum range, save NVS | `SYS_SET_EVT_UPDATED` |
| `SET_TIMEZONE` | validate −12..+14, save NVS | `SYS_SET_EVT_UPDATED` |
| `GET_ALL` | copy snapshot | `SYS_SET_EVT_READBACK` |
| `FACTORY_RESET` | erase + defaults | `SYS_SET_EVT_FACTORY_RESET_DONE` |

Publish-back via `system_manager_publish()` with `SM_SRC_SETTINGS`.

### 3.2 Update `system_manager_init()` to spawn settings task — `system_manager.c`

`system_settings_init()` already called — just ensure Phase 3.1 creates the queue and task inside it.

### 3.3 Handle `SYS_SET_EVT_UPDATED` in manager to fan out

In `sm_handle_settings_event()`:

```c
case SYS_SET_EVT_UPDATED:
    // fan out brightness to display
    system_display_send_event(SYS_DISPLAY_EVENT_BRIGHTNESS_SET,
                              system_settings_get_brightness(), 0);
    break;
```

**Verify Phase 3:** Settings command flows Button → Manager → Q_SET → Settings Task → NVS write → `EVT_UPDATED` → Manager → Display (brightness). Factory reset chain works end-to-end.

---

## Phase 4: System Network (BLE)

### 4.1 Add network enums to `source/inc/system/system_manager_events.h`

Add `system_network_event_t` and `system_network_cmd_t` (both defined in `SDD_System_Network.md`).

Extend `sm_event_msg_t` union:

```c
struct { uint8_t event_type; uint8_t reserved[3]; } network;
```

### 4.2 Create `source/inc/system/system_network.h`

```c
base_status_t system_network_init(void);
base_status_t system_network_send_cmd(system_network_cmd_t cmd,
                                      const uint8_t *pb_buf, uint16_t pb_len);
```

### 4.3 Create `source/src/system/system_network.c`

Structure:

1. **NimBLE init** in `system_network_init()`:
   - `nimble_port_init()`, register GAP + GATT callbacks
   - Configure NUS service (UUIDs `6E400001...` service, `6E400002...` RX write, `6E400003...` TX notify)
   - Create Q_NET (depth 32), spawn task, start advertising

2. **GAP callback** (static):
   - `CONNECT` → store `conn_handle`, `system_manager_publish(SYS_NET_EVT_APP_CONNECTED)`
   - `DISCONNECT` → `system_manager_publish(SYS_NET_EVT_APP_DISCONNECTED)`, restart advertising

3. **NUS RX write handler** (GATT callback):
   - Append to static reassembly buffer (sized to MTU)
   - When `uint16_t payload_len` header is satisfied: Nanopb decode `NetPacket`
   - Route decoded packet to appropriate `SYS_NET_EVT_*` publish
   - On parse failure: reset buffer, log error

4. **Network task** consuming Q_NET:
   - `CMD_START/STOP_ADVERTISING`, `CMD_SEND_NUS_PAYLOAD`, `CMD_DISCONNECT`

### 4.4 Protobuf schema — `source/proto/neowatch.proto`

Key messages per SDD: `NetPacket` (oneof body), `ThemeChanged`, `DeviceStatus`, `Ack`.

Add nanopb `idf_component.yml` dependency; add generated `.pb.c` to `main/CMakeLists.txt` SRCS.

### 4.5 Handle `SM_SRC_NETWORK` in manager — `system_manager.c`

Add case to `system_manager_task()` switch:

```c
case SM_SRC_NETWORK: sm_handle_network_event(&evt); break;
```

Implement `sm_handle_network_event()`:

- `APP_CONNECTED` → `system_ui_send_cmd(SYS_UI_CMD_APP_CONNECTED, 0, 0)`
- `APP_DISCONNECTED` → `system_ui_send_cmd(SYS_UI_CMD_APP_DISCONNECTED, 0, 0)`
- `THEME_CHANGED` → `system_settings_send_cmd(SYS_SET_CMD_SET_THEME, theme_id)`

### 4.6 Update `main/CMakeLists.txt`

Add `system_network.c` and `neowatch.pb.c` to SRCS; add `bt nvs_flash` to REQUIRES.

**Verify Phase 4:** "NeoWatch" visible in BLE scanner. App connect/disconnect cycles LED green/off. Theme change packet persisted via Settings. Bad packet rejected without crash.

---

## Phase 5: Legacy Code Quarantine

STM32-era files that cannot compile on ESP-IDF but are not in CMakeLists.txt SRCS (so don't currently break the build). Move to `source/legacy/` to prevent accidental inclusion.

Files to move:

- `source/src/middleware/cbuffer.c` + `source/inc/middleware/cbuffer.h` (uses `cmsis_compiler.h`)
- `source/src/middleware/snake_game.c` + `source/inc/middleware/snake_game.h` (calls `HAL_GetTick()`, uses `drv_ssd1306.h`)
- `source/src/middleware/fonts.c` + `source/inc/middleware/fonts.h` (includes `main.h`)
- `source/inc/middleware/table_music.h` (orphaned, no driver)
- `source/inc/middleware/horse_anim.h` (orphaned large data asset)

Also clean up `bsp_lcd.c`:

- Remove dead `m_i2c_init()` static function (called nowhere, entirely commented out)
- Implement `bsp_lcd_clock_set_mode()` to actually store and apply the mode

**Verify Phase 5:** `idf.py build` produces zero warnings about missing includes from quarantined files.

---

## Phase 6: Robustness (Watchdog + Overflow)

### 6.1 Per-queue overflow policy

- Q_DIS REFRESH: drop oldest (coalesce repeated refreshes under pressure)
- Q_SET, Q_NET: reject newest, increment drop counter, `ESP_LOGW`

### 6.2 Queue watchdog timer — `system_manager.c`

10-second periodic `esp_timer` checks `uxQueueMessagesWaiting(s_ingress_queue)`. If > 28 (of 32): `ESP_LOGW`. If saturated for 3 consecutive checks: `system_ui_send_cmd(SYS_UI_CMD_ERROR_STATE, 0, 0)`.

**Verify Phase 6:** Flooding Q_DIS with REFRESH events does not cause deadlock. Watchdog fires warning log under sustained ingress saturation.

---

## Critical Behavioral Contracts

1. `bsp_i2c_init()` must complete before any `bsp_lcd_*` call
2. All `lv_*` calls must hold `lvgl_api_lock` mutex — preserve the lock pattern from `bsp_lcd.c`
3. `system_manager_publish()` is task-context safe (uses `xQueueSend`, not ISR variant); button callbacks and NimBLE host task callbacks are both task-context — safe
4. `system_settings_get()` returns a value copy — publish snapshots by value, not pointer
5. LVGL port task (priority 2) is sole owner of `lv_timer_handler()` — no other task may call it

---

## Implementation Order

```
Phase 1  →  must be first (crash prevention + entry point)
Phase 2  →  immediately after Phase 1 (core watch features)
Phase 3  →  after Phase 2 (needs system_display_send_event API)
Phase 4  →  can start after Phase 1, parallel with Phase 3
Phase 5  →  independent, any time
Phase 6  →  after Phases 2–4 are all complete
```

---

## Critical Files

| File | Phase | Change |
|------|-------|--------|
| `main/main.c` | 1 | Uncomment `system_manager_init()`, remove `system_ui_test()` |
| `source/src/bsp/bsp.c` | 1 | Add `bsp_i2c_init()` and `bsp_rtc_init()` calls |
| `source/src/system/system_manager.c` | 1,2,3,4 | Uncomment `system_display_init()`; wire `sm_set_mode()` to Q_DIS; add network handler; add settings fan-out |
| `source/src/system/system_display.c` | 2 | Rewrite as FreeRTOS task; implement `system_display_update()` |
| `source/inc/system/system_display.h` | 2 | Add `system_display_send_event()` prototype |
| `source/src/bsp/bsp_lcd.c` | 2,5 | Fix `bsp_lcd_clock_display()` to use passed params; implement `bsp_lcd_clock_set_mode()`; remove dead `m_i2c_init()` |
| `source/src/driver/drv_button.c` | 2 | Change `BUTTON_LONG_PRESS_HOLD` → `BUTTON_LONG_PRESS_START`; set `long_press_time = 1000` |
| `source/src/system/system_settings.c` | 3 | Add Q_SET queue, task, `system_settings_send_cmd()` |
| `source/inc/system/system_settings.h` | 3 | Add `system_settings_send_cmd()` prototype |
| `source/inc/system/system_manager_events.h` | 4 | Add `system_network_event_t`, `system_network_cmd_t`, extend union |
| `source/src/system/system_network.c` | 4 | **New file** — NimBLE + NUS + Nanopb + Q_NET task |
| `source/inc/system/system_network.h` | 4 | **New file** — public API |
| `source/proto/neowatch.proto` | 4 | **New file** — protobuf schema |
| `main/CMakeLists.txt` | 4 | Add `system_network.c`, `neowatch.pb.c`, `bt nvs_flash` |
| `source/legacy/` | 5 | Move STM32 artifacts here |
