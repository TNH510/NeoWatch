# System Network Detailed Design Document

## 1. Purpose

System Network handles BLE communication between NeoWatch and mobile app using NUS (Nordic UART Service).

In the System Manager architecture, System Network is:

1. A consumer of network commands from Q_NET.
2. A publisher of connection/data events back to System Manager.

---

## 2. Scope

### In Scope

- BLE advertising and connection lifecycle.
- NUS RX/TX data exchange.
- Parsing app commands received over NUS.
- Publishing network-origin events to System Manager.

### Out of Scope

- UI rendering logic.
- Settings persistence logic.
- Display behavior implementation.

---

## 3. Architecture in System Manager Flow

1. System Manager routes network-related commands to Q_NET.
2. System Network dequeues command and executes BLE/NUS action.
3. On connection/data/status changes, System Network publishes events to System Manager.
4. System Manager routes resulting actions to Q_UI, Q_DIS, or Q_SET.

This preserves subsystem decoupling and queue-based async behavior.

---

## 4. Event Enum Definitions

### 4.1 Network -> System Manager Events

```c
typedef enum
{
  SYS_NET_EVT_APP_CONNECTED = 0,
  SYS_NET_EVT_APP_DISCONNECTED,
  SYS_NET_EVT_THEME_CHANGED,
  SYS_NET_EVT_RX_COMMAND,
  SYS_NET_EVT_SYNC_STARTED,
  SYS_NET_EVT_SYNC_DONE,
  SYS_NET_EVT_SYNC_FAILED,
  SYS_NET_EVT_LINK_QUALITY_CHANGED,
  SYS_NET_EVT_MAX
} system_network_event_t;
```

### 4.2 System Manager -> Network Commands

```c
typedef enum
{
  SYS_NET_CMD_DEVICE_STATUS_CHANGED = 0,
  SYS_NET_CMD_START_ADVERTISING,
  SYS_NET_CMD_STOP_ADVERTISING,
  SYS_NET_CMD_SEND_NUS_PAYLOAD,
  SYS_NET_CMD_DISCONNECT,
  SYS_NET_CMD_SYNC_REQUEST,
  SYS_NET_CMD_MAX
} system_network_cmd_t;
```

Notes:

- This includes your existing required events:
  - ThemeChanged
  - AppConnected
  - AppDisconnected
  - DeviceStatusChanged
- Additional events/commands are recommended for complete NUS operation.

---

## 5. Protocol: Nanopb over NUS

NUS is used as transport. Application payload format is protobuf (Nanopb), not raw opcodes.

### 5.1 Transport Framing on NUS

Each logical protobuf packet is sent as:

1. `uint16_t payload_len` (little-endian)
2. `payload_len` bytes of protobuf-encoded `NetPacket`

If a packet is larger than one BLE notification/indication chunk, it is fragmented at transport level and reassembled before protobuf decode.

### 5.2 Protobuf Schema (for Nanopb)

```proto
syntax = "proto3";

enum NetEventType {
  NET_EVT_APP_CONNECTED = 0;
  NET_EVT_APP_DISCONNECTED = 1;
  NET_EVT_THEME_CHANGED = 2;
  NET_EVT_SYNC_STARTED = 3;
  NET_EVT_SYNC_DONE = 4;
  NET_EVT_SYNC_FAILED = 5;
  NET_EVT_LINK_QUALITY_CHANGED = 6;
}

enum NetCommandType {
  NET_CMD_DEVICE_STATUS_CHANGED = 0;
  NET_CMD_START_ADVERTISING = 1;
  NET_CMD_STOP_ADVERTISING = 2;
  NET_CMD_SEND_DATA = 3;
  NET_CMD_DISCONNECT = 4;
  NET_CMD_SYNC_REQUEST = 5;
}

message ThemeChanged {
  uint32 theme_id = 1;
}

message DeviceStatus {
  uint32 battery_percent = 1;
  bool charging = 2;
  uint32 mode = 3;
}

message Ack {
  uint32 seq = 1;
  bool ok = 2;
  uint32 code = 3;
}

message NetPacket {
  uint32 seq = 1;
  uint64 timestamp_ms = 2;

  oneof body {
    NetEventType event = 10;
    NetCommandType command = 11;
    ThemeChanged theme_changed = 12;
    DeviceStatus device_status = 13;
    Ack ack = 14;
  }
}
```

Nanopb notes:

1. Add `.options` file to bound repeated fields/string sizes.
2. Avoid unbounded `bytes` for deterministic memory.
3. Use static buffers for encode/decode in task context.

---

## 6. Queue Message Contracts (Nanopb)

### 6.1 Event Message (Network -> Manager)

```c
typedef struct
{
  system_network_event_t event;
  uint32_t timestamp_ms;
  uint16_t pb_len;
  uint8_t pb_buf[128];   // protobuf-encoded NetPacket
} system_network_evt_msg_t;
```

### 6.2 Command Message (Manager -> Network via Q_NET)

```c
typedef struct
{
  system_network_cmd_t cmd;
  uint32_t timestamp_ms;
  uint16_t pb_len;
  uint8_t pb_buf[128];   // protobuf-encoded NetPacket
} system_network_cmd_msg_t;
```

Guidelines:

1. Use fixed-size queue messages for deterministic memory use.
2. Reject oversized protobuf frames and publish SYS_NET_EVT_SYNC_FAILED or RX parse error event.
3. Keep BLE callbacks short; defer heavy work to network task context.

---

## 7. Core Event Flows

### 7.1 App Connected

1. BLE link established.
2. NUS service is ready.
3. System Network publishes SYS_NET_EVT_APP_CONNECTED.
4. System Manager routes status update to UI/Display.

### 7.2 Theme Changed via NUS + Nanopb

1. App sends protobuf `NetPacket` with `theme_changed` body.
2. Network reassembles frame, decodes protobuf, validates payload.
3. Network publishes SYS_NET_EVT_THEME_CHANGED.
4. System Manager routes to Settings and Display update flow.

### 7.3 Device Status Changed from Manager

1. Manager sends SYS_NET_CMD_DEVICE_STATUS_CHANGED to Q_NET.
2. Network builds protobuf `NetPacket` with `device_status` body and transmits via NUS TX.
3. Optional ACK result is published as SYS_NET_EVT_SYNC_DONE or SYS_NET_EVT_SYNC_FAILED.

---

## 8. Recommended Additional Events

For production robustness, add these events:

- SYS_NET_EVT_PAIRING_REQUIRED
- SYS_NET_EVT_AUTH_FAILED
- SYS_NET_EVT_MTU_CHANGED
- SYS_NET_EVT_TX_BUSY
- SYS_NET_EVT_RX_PARSE_ERROR
- SYS_NET_EVT_HEARTBEAT_TIMEOUT

Why:

- Better visibility into BLE reliability and app protocol health.
- Easier retry and fallback logic in System Manager.

---

## 9. Error Handling Rules

1. Invalid NUS frame or protobuf decode failure: drop frame, publish RX parse error event.
2. TX when disconnected: return failure immediately, publish sync failed event.
3. Repeated sync failure: apply bounded retry and cooldown.
4. Connection loss during transfer: abort transfer and publish app disconnected.

---

## 10. Security and Reliability Notes

1. Require BLE bonding/encryption for write commands that modify settings.
2. Validate protobuf message type and field ranges before processing.
3. Use `seq` field in `NetPacket` for idempotent sync operations.
4. Use heartbeat (app ping) to detect stale connection state.

---

## 11. Minimal Verification Checklist

1. Advertising starts and app can connect.
2. On connect, SYS_NET_EVT_APP_CONNECTED is published.
3. On disconnect, SYS_NET_EVT_APP_DISCONNECTED is published.
4. Theme change frame triggers SYS_NET_EVT_THEME_CHANGED and downstream update.
5. Device status command from manager is transmitted over NUS.
6. Malformed protobuf frame is rejected safely without task crash.
7. Encode/decode success is verified for all required packet types.
8. Queue load test does not deadlock network task.