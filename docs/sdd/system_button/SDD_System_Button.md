# System Button Detailed Design Document

## 1. Purpose

System Button detects user input and publishes normalized button actions to System Manager.

This document only defines 3 actions:

- Click
- Double Click
- Hold 1 second

---

## 2. Event Enum (App-Level)

```c
typedef enum
{
    SYS_BUTTON_EVENT_CLICK = 0,
    SYS_BUTTON_EVENT_DOUBLE_CLICK,
    SYS_BUTTON_EVENT_HOLD_1S,
    SYS_BUTTON_EVENT_MAX
} system_button_event_t;
```

---

## 3. ESP-IDF Button Event Mapping

Using `espressif/button` (`iot_button.h`):

```c
// Click
BUTTON_SINGLE_CLICK

// Double Click
BUTTON_DOUBLE_CLICK

// Hold 1 second (trigger once when threshold is reached)
BUTTON_LONG_PRESS_START
```

Notes:

- `BUTTON_LONG_PRESS_START` is the correct one-shot event for Hold 1 second.
- `BUTTON_LONG_PRESS_HOLD` is periodic while still pressed; do not use it for one-shot Hold 1 second.

---

## 4. Required Configuration

Set long-press threshold to 1000 ms.

```c
// Example: set before use (API depends on your wrapper)
iot_button_set_param(btn_handle, BUTTON_LONG_PRESS_TIME_MS, (void *)1000);
```

If your wrapper owns config, ensure it sets long press time to 1000 ms internally.

---

## 5. Callback Registration (Example)

```c
// Register only the 3 required events
iot_button_register_cb(btn, BUTTON_SINGLE_CLICK, NULL, on_button_event, (void *)SYS_BUTTON_EVENT_CLICK);
iot_button_register_cb(btn, BUTTON_DOUBLE_CLICK, NULL, on_button_event, (void *)SYS_BUTTON_EVENT_DOUBLE_CLICK);
iot_button_register_cb(btn, BUTTON_LONG_PRESS_START, NULL, on_button_event, (void *)SYS_BUTTON_EVENT_HOLD_1S);
```

---

## 6. Publish Contract to System Manager

Each callback publishes one `sm_event_msg_t` to the System Manager ingress queue.

```c
/* Unified event message (defined in system_manager_events.h) */
typedef struct
{
    sm_event_source_t source;       /* SM_SRC_BUTTON                    */
    uint16_t          event_id;     /* system_button_event_t value      */
    uint32_t          timestamp_ms;
    union {
        struct { uint8_t button_id; } button;
        /* other subsystem payloads omitted */
    } data;
} sm_event_msg_t;
```

Routing expectation:

- `SYS_BUTTON_EVENT_CLICK` -> UI normal action
- `SYS_BUTTON_EVENT_DOUBLE_CLICK` -> UI shortcut action
- `SYS_BUTTON_EVENT_HOLD_1S` -> mode/action requiring long press

---

## 7. Minimal Behavior Rules

1. One physical interaction produces one logical event.
2. Hold 1 second emits once per press cycle.
3. No app logic inside driver callback; callback only packages and publishes event.
4. If queue is full, count drop and return quickly.

---

## 8. Verification Checklist

1. Single short press -> `SYS_BUTTON_EVENT_CLICK`
2. Two short presses in double-click window -> `SYS_BUTTON_EVENT_DOUBLE_CLICK`
3. Press and hold >= 1000 ms -> `SYS_BUTTON_EVENT_HOLD_1S`
4. Hold event fires once, not repeatedly while still pressed
5. Events arrive correctly at System Manager queue
