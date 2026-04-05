# System Settings Detailed Design Document

## 1. Purpose

System Settings manages persistent device configuration for NeoWatch.

In the System Manager architecture, System Settings is:

1. A consumer of settings commands routed by System Manager (typically via Q_SET).
2. A publisher of settings result/update events back to System Manager.

---

## 2. Current Implemented Settings

Based on current source implementation, these settings are actively supported:

1. Brightness (0 to 100)
2. Clock type (LEFT, RIGHT)
3. Time zone (-12 to +14)

All settings are loaded from and persisted to NVS namespace: sys_settings.

---

## 3. Architecture in System Manager Flow

1. System Manager receives a settings request from UI or Network.
2. System Manager routes command to Q_SET.
3. System Settings validates value range and applies update.
4. System Settings persists to NVS.
5. System Settings publishes success/failure event to System Manager.
6. System Manager fans out follow-up events to Display/UI/Network if needed.

This keeps validation and persistence centralized in one subsystem.

---

## 4. Event Enum Definitions

### 4.1 System Manager -> Settings Commands

```c
typedef enum
{
	SYS_SET_CMD_SET_BRIGHTNESS = 0,
	SYS_SET_CMD_SET_CLOCK_TYPE,
	SYS_SET_CMD_SET_TIMEZONE,
	SYS_SET_CMD_GET_ALL,
	SYS_SET_CMD_FACTORY_RESET,

	// Recommended future commands
	SYS_SET_CMD_SET_TIME,
	SYS_SET_CMD_SET_THEME,
	SYS_SET_CMD_SET_LANGUAGE,
	SYS_SET_CMD_SET_UNITS,
	SYS_SET_CMD_SET_DO_NOT_DISTURB,

	SYS_SET_CMD_MAX
} system_settings_cmd_t;
```

### 4.2 Settings -> System Manager Events

```c
typedef enum
{
	SYS_SET_EVT_UPDATED = 0,
	SYS_SET_EVT_READBACK,
	SYS_SET_EVT_FACTORY_RESET_DONE,
	SYS_SET_EVT_VALIDATION_FAILED,
	SYS_SET_EVT_PERSIST_FAILED,

	// Recommended future events
	SYS_SET_EVT_TIME_UPDATED,
	SYS_SET_EVT_THEME_UPDATED,

	SYS_SET_EVT_MAX
} system_settings_evt_t;
```

---

## 5. Required and Suggested Events

### 5.1 Required by Current Product Direction

1. SetTime
2. SetTimezone
3. SetTheme

### 5.2 Mapping to Command/Event Model

1. SetTime -> SYS_SET_CMD_SET_TIME -> SYS_SET_EVT_TIME_UPDATED or SYS_SET_EVT_VALIDATION_FAILED
2. SetTimezone -> SYS_SET_CMD_SET_TIMEZONE -> SYS_SET_EVT_UPDATED or SYS_SET_EVT_VALIDATION_FAILED
3. SetTheme -> SYS_SET_CMD_SET_THEME -> SYS_SET_EVT_THEME_UPDATED or SYS_SET_EVT_VALIDATION_FAILED

### 5.3 More Possible Events (Recommended)

1. SetBrightness
2. SetClockType
3. FactoryReset
4. SetLanguage
5. SetUnits (Metric/Imperial)
6. SetDoNotDisturb
7. SetRaiseToWake
8. SetAutoSleepTimeout
9. SetVibrationIntensity
10. SetNotificationFilter

These events are common watch settings and align naturally with Settings ownership.

---

## 6. Message Contracts (Q_SET)

### 6.1 Command Message

```c
typedef struct
{
	system_settings_cmd_t cmd;
	uint32_t timestamp_ms;
	union
	{
		uint8_t brightness;  // 0..100
		uint8_t clock_type;  // enum index
		int8_t time_zone;    // -12..+14
		uint32_t unix_time;  // for SetTime
		uint8_t theme_id;    // for SetTheme
	} data;
} system_settings_cmd_msg_t;
```

### 6.2 Event Message

```c
typedef struct
{
	system_settings_evt_t evt;
	uint32_t timestamp_ms;
	base_status_t status;
	system_settings_t snapshot;
} system_settings_evt_msg_t;
```

---

## 7. Validation and Persistence Rules

Validation constraints:

1. Brightness must be within 0 to 100.
2. Clock type must be within defined enum values.
3. Time zone must be within -12 to +14.
4. Theme ID must be one of supported themes (table-driven).
5. SetTime value must be sane (non-zero and inside accepted epoch range).

Persistence behavior:

1. Apply in RAM only after command validation passes.
2. Persist to NVS immediately after apply.
3. If NVS write fails, publish SYS_SET_EVT_PERSIST_FAILED.
4. Factory reset restores defaults then writes all keys to NVS.

---

## 8. Defaults

Current defaults:

1. Brightness = 50
2. Clock type = LEFT
3. Time zone = 0

Recommended future defaults:

1. Theme = default system theme
2. Language = English
3. DND = disabled

---

## 9. Event Flow Examples

### 9.1 SetTimezone

1. UI sends timezone update request.
2. System Manager routes SYS_SET_CMD_SET_TIMEZONE to Q_SET.
3. Settings validates range and persists to NVS.
4. Settings publishes SYS_SET_EVT_UPDATED.
5. System Manager routes refresh events to Display and optional sync to Network.

### 9.2 SetTheme

1. Network receives SetTheme command from app.
2. System Manager routes SYS_SET_CMD_SET_THEME to Q_SET.
3. Settings validates theme and stores value.
4. Settings publishes SYS_SET_EVT_THEME_UPDATED.
5. System Manager routes display theme refresh to Q_DIS.

---

## 10. Error Handling

1. Invalid value range -> SYS_SET_EVT_VALIDATION_FAILED.
2. NVS write/read failure -> SYS_SET_EVT_PERSIST_FAILED.
3. Unknown command -> ignore safely and publish validation failed event.
4. Queue full -> drop with counter increment and diagnostics log.

---

## 11. Minimal Verification Checklist

1. Init loads persisted values from NVS correctly.
2. SetBrightness accepts 0..100 and rejects out-of-range values.
3. SetTimezone accepts -12..+14 and rejects out-of-range values.
4. Factory reset restores defaults and persists them.
5. SetTheme event triggers display update path.
6. SetTime event updates RTC path and generates result event.
7. Invalid input never corrupts stored settings.