# System Manager Detailed Design Document

## Overview

The System Manager is responsible for managing the overall system operations, including coordinating between different components, handling system events, and ensuring smooth functioning of the entire system. It acts as a central hub that oversees the interactions between various subsystems and ensures that they work together seamlessly.

## Concept: Message Queue-Based Event Management

### Purpose

The System Manager implements an event-driven architecture using message queues to decouple interactions between different system layers and components. This approach provides a flexible, scalable, and maintainable solution for inter-system communication.

### Key Principles

#### 1. **Decoupled Architecture**

- Different system layers (UI, Network, Display, Settings, Button) communicate through message queues rather than direct function calls
- Components do not need to know about each other's internal implementation
- Reduces tight coupling and facilitates independent development and testing

#### 2. **Event-Driven Communication**

- All system interactions are represented as discrete events/messages
- Each event contains relevant data needed for processing
- Events are published to queues and consumed by interested subscribers

#### 3. **Asynchronous Processing**

- Messages are queued and processed in order
- Prevents blocking operations between components
- Improves system responsiveness and overall performance

### Architecture Components

#### Message Queue Manager

- Central component that manages all message queues
- Handles message routing based on event type
- Maintains queue priorities if needed

#### Event Publishers

- System layers that generate events (Button input, Network data, Settings changes)
- Publish events to the relevant message queues
- Do not wait for processing results

#### Event Subscribers/Consumers

- System components that listen to and process events
- React to specific event types
- Execute appropriate actions based on event data

### Message Types

```
UI Events:
├── Button Press Events
├── Touch Events
├── Display Interaction Events
└── Settings Update Events

Network Events:
├── Data Received Events
├── Connection Status Events
├── Sync Events
└── Error Events

System Events:
├── Power State Changes
├── Mode Changes
├── Configuration Updates
└── System Health Events
```

### Benefits

1. **Scalability**: Easy to add new components or event types without modifying existing code
2. **Maintainability**: Clear separation of concerns and reduced code dependencies
3. **Testability**: Components can be tested in isolation with mock message queues
4. **Reliability**: Events can be logged and replayed for debugging
5. **Flexibility**: Events can be processed synchronously or asynchronously as needed
6. **Priority Handling**: Different event types can be assigned different priority levels

### Implementation Considerations

- Queue depth management to prevent memory overflow
- Event timeout mechanisms for unprocessed messages
- Error handling and recovery for failed event processing
- Logging and monitoring of message flow
- Performance optimization for high-frequency events

## Sequence Diagram

### Example: Button Press Event Flow

```mermaid
sequenceDiagram
    participant Button as Button System
    participant MQM as Message Queue<br/>Manager
    participant Display as Display System
    participant UI as UI System
    participant Network as Network System
    participant Settings as Settings System

    Button->>MQM: Publish ButtonPressEvent<br/>(button_id, timestamp)
    activate MQM
    MQM->>MQM: Route event based on<br/>subscribers
    deactivate MQM

    par Parallel Processing
        MQM->>Display: Post event to<br/>Display Queue
        activate Display
        Display->>Display: Process button event<br/>update display state
        Display->>MQM: Publish DisplayUpdateEvent
        deactivate Display

        MQM->>UI: Post event to<br/>UI Queue
        activate UI
        UI->>UI: Process button event<br/>update UI state
        UI->>MQM: Publish UIStateChangeEvent
        deactivate UI

        MQM->>Settings: Post event to<br/>Settings Queue
        activate Settings
        Settings->>Settings: Check if settings<br/>adjustment needed
        alt Settings Changed
            Settings->>MQM: Publish SettingsChangeEvent
        end
        deactivate Settings
    end

    MQM->>Network: Post event to<br/>Network Queue
    activate Network
    Network->>Network: Log event for<br/>sync if needed
    Network->>MQM: Publish SyncReadyEvent
    deactivate Network

    MQM->>MQM: Log ButtonPressEvent<br/>in system history
```

### Event Flow Timeline

```
Time | Button        | MQM               | Display         | UI              | Network
-----|---------------|-------------------|-----------------|-----------------|----------
T0   | Detect Press  |                   |                 |                 |
T1   | Publish Event | ✓ Receive         | ✓ Queue         | ✓ Queue         |
T2   |               | Route to all      |                 |                 |
T3   |               |                   | Process & Reply | Process & Reply |
T4   |               |                   | ✓ Update done   | ✓ Update done   |
T5   |               |                   |                 |                 | ✓ Queue
T6   |               |                   |                 |                 | Process
T7   |               | ✓ All events      |                 |                 |
     |               | processed         |                 |                 |
```

### Message Queue Structure

```
┌─────────────────────────────────────────────────────┐
│           System Manager - Message Hub              │
├─────────────────────────────────────────────────────┤
│                                                     │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐         │
│  │  Button  │  │  Network │  │ Settings │         │
│  │  Queue   │  │  Queue   │  │  Queue   │         │
│  └──────────┘  └──────────┘  └──────────┘         │
│       ↑             ↑              ↑               │
│       └─────────────┼──────────────┘               │
│                     │                             │
│           Message Queue Router                    │
│                     │                             │
│       ┌─────────────┼──────────────┐             │
│       ↓             ↓              ↓             │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐      │
│  │ Display  │  │   UI     │  │ System   │      │
│  │  Queue   │  │  Queue   │  │  Queue   │      │
│  └──────────┘  └──────────┘  └──────────┘      │
│                                                │
└─────────────────────────────────────────────────┘
```
