# System UI Detailed Design Document

## 1. Purpose

System UI controls interaction state and user-facing feedback for NeoWatch, including:

1. Screen navigation state
2. User notifications and prompts
3. RGB LED status indication

In the System Manager architecture, System UI is a consumer of Q_UI events and can publish UI result/state events back to System Manager.

---

## 2. Scope

### In Scope

1. Navigation state transitions (boot, standby, menu, settings)
2. LED feedback state machine
3. UI-level notifications and status indicators
4. Forwarding UI-triggered actions to System Manager

### Out of Scope

1. Pixel rendering and frame flush implementation (System Display)
2. Settings persistence (System Settings)
3. BLE transport/protocol handling (System Network)

---

## 3. Architecture in System Manager Flow

1. System Manager receives events from Button, Network, and Settings.
2. Manager routes UI-related commands to Q_UI.
3. System UI updates local interaction state and LED indication.
4. When needed, System UI emits result/state events back to Manager.
5. Manager fans out updates to Display, Settings, and Network.

This keeps UI logic decoupled from display drivers and backend subsystems.

---

## 4. UI Event Enum Definitions (Implemented)

All enums are defined in `system_manager_events.h`.

### 4.1 System Manager -> UI Commands

```c
typedef enum
{
  SYS_UI_CMD_MODE_BOOT = 0,
  SYS_UI_CMD_MODE_STANDBY,
  SYS_UI_CMD_MODE_MENU,
  SYS_UI_CMD_MODE_SETTING,
  SYS_UI_CMD_SHOW_NOTIFICATION,
  SYS_UI_CMD_HIDE_NOTIFICATION,
  SYS_UI_CMD_LED_SET_STATE,
  SYS_UI_CMD_LED_CLEAR,
  SYS_UI_CMD_APP_CONNECTED,
  SYS_UI_CMD_APP_DISCONNECTED,
  SYS_UI_CMD_ERROR_STATE,
  SYS_UI_CMD_MAX
} system_ui_cmd_t;
```

### 4.2 UI -> System Manager Events

```c
typedef enum
{
  SYS_UI_EVT_MODE_CHANGED = 0,
  SYS_UI_EVT_NOTIFICATION_ACK,
  SYS_UI_EVT_LED_STATE_APPLIED,
  SYS_UI_EVT_INPUT_ACTION,
  SYS_UI_EVT_ERROR,
  SYS_UI_EVT_MAX
} system_ui_evt_t;
```

---

## 5. Required RGB LED Event Table

Use RGB LED to indicate system states as follows:

| State            | LED Behavior                         | Priority |
| ---------------- | ------------------------------------ | -------- |
| System Error     | Flash red rapidly (for example 4 Hz) | Highest  |
| App Connected    | Green solid                          | Medium   |
| Normal Operation | Off                                  | Lowest   |

Rules:

1. Higher-priority state overrides lower-priority state.
2. Error indication remains active until explicit clear event.
3. LED updates must be non-blocking and task-safe.

---

## 6. Suggested Additional UI Events

Recommended future events for better UX:

1. SYS_UI_CMD_SHOW_TOAST
2. SYS_UI_CMD_HIDE_TOAST
3. SYS_UI_CMD_SHOW_CONFIRM_DIALOG
4. SYS_UI_CMD_SET_BRIGHTNESS_PREVIEW
5. SYS_UI_CMD_SCREEN_TIMEOUT_UPDATED
6. SYS_UI_CMD_DO_NOT_DISTURB_ON
7. SYS_UI_CMD_DO_NOT_DISTURB_OFF
8. SYS_UI_CMD_BATTERY_LOW_WARNING
9. SYS_UI_CMD_CHARGING_STATE_CHANGED
10. SYS_UI_CMD_SYNC_STATUS_CHANGED

These events allow richer UI behavior without coupling UI directly to Network or Settings internals.

---

## 7. Queue Message Contract (Q_UI)

UI events use the unified `sm_event_msg_t` (defined in `system_manager_events.h`):

```c
typedef struct
{
    sm_event_source_t source;       /* SM_SRC_UI or SM_SRC_MANAGER       */
    uint16_t          event_id;     /* system_ui_cmd_t or system_ui_evt_t */
    uint32_t          timestamp_ms;
    union {
        struct {
            uint8_t arg0;           /* command-specific argument 0       */
            uint8_t arg1;           /* command-specific argument 1       */
        } ui;
        /* other subsystem payloads omitted */
    } data;
} sm_event_msg_t;
```

Public API for sending commands to Q_UI:

```c
base_status_t system_ui_send_cmd(system_ui_cmd_t cmd, uint8_t arg0, uint8_t arg1);
```

Guidelines:

1. Keep fixed-size queue messages for deterministic memory usage.
2. Use payload IDs rather than large text blobs in queue payload.
3. Do not block in LED update path.

---

## 8. Core Event Flows

### 8.1 App Connected Flow

1. Network publishes app connected event to Manager.
2. Manager sends SYS_UI_CMD_APP_CONNECTED to Q_UI.
3. UI applies green solid LED state.
4. UI emits SYS_UI_EVT_LED_STATE_APPLIED.

### 8.2 Error Flow

1. Any subsystem reports error to Manager.
2. Manager sends SYS_UI_CMD_ERROR_STATE to Q_UI.
3. UI applies rapid red flash LED.
4. UI keeps error indication active until clear command.

### 8.3 Mode Change Flow

1. Button or app action triggers mode transition request.
2. Manager sends one of mode commands to Q_UI.
3. UI updates interaction state and emits SYS_UI_EVT_MODE_CHANGED.
4. Manager routes display update to Q_DIS.

---

## 9. Error Handling

1. Unknown command: ignore safely and publish SYS_UI_EVT_ERROR.
2. Queue full: drop oldest non-critical UI command and count overflow.
3. LED driver failure: publish SYS_UI_EVT_ERROR and fallback to LED off.
4. Invalid payload_id: reject and log diagnostics.

---

## 10. Minimal Verification Checklist

1. System Error event triggers rapid red flash.
2. App Connected event triggers green solid.
3. Normal Operation clears LED to off.
4. Error priority overrides app-connected indication.
5. Mode transition commands produce SYS_UI_EVT_MODE_CHANGED.
6. Queue stress does not deadlock UI task.