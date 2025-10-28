# NeoWatch

## Purpose

The purpose of the NeoWatch project is to create a convenient and intelligent watch. The project aims to combine smart features to provide more than just time information. The clock will be equipped with additional features such as notifications, sound adjustment, ambient temperature display, and the ability to interact with the user through an OLED screen and control buttons.

## Features

The NeoWatch project includes the following features:

1. **Time Display**: The clock will display the time (hours, minutes, seconds) and date on the screen with customizable themes.

## Required Modules

| Module | Functionality                          |
| ------ | -------------------------------------- |
| ESP32  | Main microcontroller for the clock     |
| GC9A01 | Display module for showing information |
| Button | User interaction module for input      |

## Detailed Features

1. **Time Display**

| ID  | Functionality                  | Note                                                                 |
| --- | ------------------------------ | -------------------------------------------------------------------- |
| 01  | Display Current Time On Screen | Display Year, Month, Day of Week, Day of Month, Hour, Minute, Second |
| 02  | Theme                          | Change appearance                                                    |

2. **Smart Phone Interaction Capability**

| ID  | Functionality           | Note                           |
| --- | ----------------------- | ------------------------------ |
| 01  | Connect BLE             | App connect with Clock via BLE |
| 02  | Add/Remove/Change theme | Update theme                   |


## Firmware Architecture

![NeoWatch Architecture](./Project.svg)

## Developer Team

1. **Tran Ngoc Hieu**
2. **Nguyen Ba Vu Thach**
3. **Le Tan Loc**
