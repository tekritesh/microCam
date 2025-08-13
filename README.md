# microCam
Version 2 of https://github.com/uw-x/insect-robot-cam


## H/W
| Tasks                              | Status            |
| -----------------------            | ----------------- |
| Schematic for HM01B0 Flex PCB      | &#9745; Done      |
| Bring up for HM01B0 Flex PCB       | &#9745; Done      |
| Lens Selection for HM01B0 Flex PCB | &#9744; Todo      |
| Schematic for nrf52832 Flex PCB    | &#9744; Todo      |



## F/W

#### HIMAX HM01B0
| Tasks                   | Status            |
| ----------------------- | ----------------- |
| Driver for HM01B0 EVK   | &#9744; Done      |


#### BLE
| Tasks                       | Status            |
| -----------------------     | ----------------- |
| Driver for Peripheral Setup | &#9745; Done      |
| Tx Notifications Setup      | &#9745; Done      |
| Rx Notifications Setup      | &#9745; Done      |
| MTU(247) Negotiation        | &#9745; Done      |
| BLE Settings Control        | &#9744; Todo      |
| IMU Transmission            | &#9744; Todo      |

#### IMU
| Tasks                   | Status            |
| ----------------------- | ----------------- |
| Driver                  | &#9744; Todo      |


#### Power Management
| Tasks                   | Status            |
| ----------------------- | ----------------- |
| Low Power Mode | &#9744; Todo |




## Mobile App

| Tasks                    | Status            |
| -----------------------  | ----------------- |
| Update Android for legacy App | &#9745; Done |
| Tx Notifications Setup        | &#9745; Done |
| Rx Notifications Setup        | &#9745; Done |
| MTU(247) Negotiation          | &#9745; Done |
| BLE Settings Control          | &#9744; Todo |
| IMU Transmission              | &#9744; Todo |
| Image Saving                  | &#9744; Todo |



## Analysis

| Tasks                    | Status            |
| -----------------------  | ----------------- |
| Plotting a frame         | &#9745; Completed |







## Folder Structure

**microCam/firmware:** Contains Firmware for nRF52832

**microCam/hardware:** Contains schematics etc for microCam

**microCam/app:** Contains Andorid Application for Data Stream

**microCam/analysis:** Contains Pythons scripts for plotting and Analysis

## Installation

#### Install Python Dependency Manager

Refer : https://python-poetry.org/docs/


#### Install Local Dependencies
This should create a local env variable and install all the libs
```
poetry install
```

#### Build Package
This should create *tar or *whl for us to invoke in any application under ```dist``` directory 
```
poetry build
```

#### Test Package
Local Test of the package
```
poetry run pytest
```

## Usage

#### Using the poetry env to use for Jupyter
Get the env variable name
```
poetry env info
```
- Use this variable for your py-kernel when you launch your jupyter instance in vscode. 
- If you cant find this, then:
- Ctrl/Cmd + P
- Select Python Interpreter
- Enter Interpreter Path
- Use the path as per ```poetry env info``` to add your ``venv`` to vs code

...

## Contributing

...