# System Manager Architecture Recommendation for RTOS

## 1. Executive Summary

The current architecture is strong for modularity and maintainability, but a pure queue-routed hub is not always optimal for strict RTOS timing paths.

Recommended direction: keep the current event-driven queue model for normal control flow, and add a fast path for time-critical signals.

This gives you:

- clear subsystem boundaries
- lower latency for urgent events
- better deterministic behavior under load
- reduced risk of central routing bottlenecks

---

## 2. Recommended Architecture: Hybrid Event Model

Use two communication lanes.

### Lane A: Control Plane (existing model)

Use dedicated subsystem queues for normal work:

- UI navigation
- settings updates
- network sync requests
- non-urgent display refresh

Primitive: FreeRTOS queues with fixed-size event messages.

### Lane B: Real-Time Plane (new fast path)

Use direct task notifications and event groups for urgent signals:

- button edge detected
- low battery warning
- BLE connection state flip
- watchdog escalation

Primitives:

- direct-to-task notification for one-to-one urgent wakeups
- event groups for shared state flags
- ISR-safe APIs for interrupt sources

---

## 3. Event Classification and Primitive Mapping

| Event Class | Timing Goal | Fan-out | Recommended Primitive | Notes |
|------------|-------------|---------|------------------------|-------|
| Critical signal | under 5-10 ms | 1 consumer | Task notification | Lowest overhead in FreeRTOS |
| Critical state flag | under 10 ms | many consumers | Event group bit | Good for state broadcast |
| Normal command | 10-100 ms | 1 consumer | Queue | Preserve ordering and payload |
| Bulk or log data | best effort | 1+ consumers | Queue or ring buffer | Avoid blocking critical tasks |

Rule of thumb:

- If payload is tiny and urgency is high, do not route through central queue fan-out.
- If ordering and payload matter, use queue.

---

## 4. System Manager Role in Hybrid Design

The System Manager remains the policy and routing authority, but not every urgent event should pay full routing cost.

### Responsibilities to keep

- validate event schema
- route normal events to subsystem queues
- maintain event audit log
- enforce retry, timeout, and backpressure policy

### Responsibilities to reduce on critical path

- avoid multi-queue fan-out for ISR-origin urgent events
- avoid heavy payload copying for frequent events

Recommended pattern:

1. ISR or high-priority producer sends direct notification to the target real-time task.
2. That task performs minimal critical action.
3. Task emits a normal summary event to System Manager queue for traceability.

---

## 5. Tasking and Priority Guidance

Example priority bands (adapt to your project):

| Task Type | Priority Band | Purpose |
|----------|---------------|---------|
| Safety or hard response | Highest | battery critical, watchdog, immediate user feedback |
| Input and display real-time | High | button, touch, fast display cues |
| Network and protocol | Medium | BLE stack interaction, sync processing |
| UI composition and settings | Medium-low | app logic and preference handling |
| Logging and diagnostics | Low | telemetry, non-blocking logs |

Guidelines:

- Keep high-priority tasks short and non-blocking.
- Never wait on long mutex chains in high-priority tasks.
- Push expensive work to lower-priority workers via queue.

---

## 6. Memory and Determinism Recommendations

For embedded RTOS predictability:

- prefer static allocation for queues, stacks, and key buffers
- use fixed-size event structs
- avoid dynamic allocation in ISR and hot paths
- pre-size queues from measured burst rate, not guesses

Queue sizing baseline:

Queue depth >= peak event rate per second * max service delay in seconds * safety factor

Start with safety factor 1.5-2.0, then tune using real runtime telemetry.

---

## 7. Backpressure, Timeout, and Failure Policy

Define policy per event class.

| Condition | Critical Signal | Normal Command |
|----------|-----------------|----------------|
| Consumer busy | overwrite stale signal or coalesce | queue and preserve order |
| Queue full | should not apply if using notifications | drop oldest or reject newest based on command semantics |
| Timeout | escalate immediately | retry with bounded attempts |
| Failure ack | immediate fault path | NACK plus retry or degrade mode |

Implementation notes:

- Add per-queue high-watermark alarms.
- Track event age at dequeue time.
- Add circuit-breaker behavior for repeated failures.

---

## 8. Observability Requirements

To validate RTOS behavior, log these metrics:

- end-to-end latency per event type (P50, P95, P99)
- queue depth histogram per subsystem
- event drop counts by reason
- retry counts and final failure counts
- task runtime and starvation indicators

Acceptance targets example:

- button feedback P99 under 10 ms
- low-battery alert dispatch under 20 ms
- no queue overflow in 30-minute stress run

---

## 9. Migration Plan from Current Design

1. Keep all existing subsystem queues and message contracts.
2. Mark events as Critical or Normal in a shared event header.
3. Introduce a small real-time dispatcher for Critical events.
4. Convert top 2-3 latency-sensitive paths to notifications/event groups.
5. Keep audit events routed through System Manager queue.
6. Measure latency and queue pressure before and after.
7. Tune priorities, depths, and timeout thresholds.

This avoids large refactors and preserves current architecture strengths.

---

## 10. Reference Pseudocode

```c
// ISR path: urgent button edge
void button_isr_handler(void) {
    BaseType_t hp_task_woken = pdFALSE;
    vTaskNotifyGiveFromISR(button_rt_task_handle, &hp_task_woken);
    portYIELD_FROM_ISR(hp_task_woken);
}

// Real-time task: minimal immediate response
void button_rt_task(void *arg) {
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // Critical action first
        trigger_haptic_feedback_fast();

        // Then publish normal audit event
        sys_event_t ev = {
            .type = EVT_BUTTON_FEEDBACK_DONE,
            .class = EVT_CLASS_NORMAL,
        };
        xQueueSend(system_manager_queue, &ev, 0);
    }
}
```

---

## 11. Final Recommendation

Adopt a hybrid architecture:

- keep queue-based System Manager routing for normal control flow
- add direct notification and event-group fast paths for urgent RTOS events
- measure and enforce latency SLOs using runtime telemetry

This is the best balance between maintainability and real-time performance for your NeoWatch firmware.