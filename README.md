# BizarroHomer
Code for FRC Team 1511's T-Shirt Robot, BizarroHomer (a.k.a. "ThunderShot").

The robot is controlled from a Raspberry Pi which takes input from a single Bluetooth DualShock4 controller. Controlling the robot using a Raspberry Pi gives us much more flexibility for control over solutions such as the NI roboRIO. Since BizarroHomer (a.k.a. "ThunderShot") was designed specifically for use at demonstrations and outreach events for the team, a simple and straightforward control solution was greatly desired.

The software is split up into several different programs, each with different permissions and purposes.
### Main Program
The main program is run as the pi user. It reads controller input and handles the main logic of the robot software. It communicates with the [Hardware Control](#Hardware-Control-Program) program to interact with robot hardware and the [DualShock4 LED Control](#DualShock4-LED-Control-Program) program to control the color of the DualShock4 controller's LEDs.

### Hardware Control Program
The hardware control program is run as the root user. It contols the motors, encoders, etc. on the robot. It receives control messages to control hardware, and sends status messages to provide feedback such as motor internal encoder measurements. This program also will disable all hardware outputs when the main program stops. This keeps things operating safely.

### DualShock4 LED Control Program
The DualShock4 LED control program is run as the root user. It controls the LEDs on the DualShock4 controller. The LED colors signify different states/errors of the robot program. For example, if the main program exits then the LEDs will signify that to alert the driver.

## General Setup
1. Run the `setup.sh` script
2. [Pair a DualShock4 controller](#Pairing-DualShock4-Controller)
3. Compile and install using CMake
4. Restart Raspberry Pi

## Pairing DualShock4 Controller
Start up bluetoothctl.
```
$ bluetoothctl
```
Then, turn on bluetooth and scan for devices.
```
[bluetooth]# power on
[bluetooth]# agent off
[bluetooth]# agent NoInputNoOutput
[bluetooth]# default-agent
[bluetooth]# scan on
```
When the controller appears in the scan, pair to it using its Bluetooth address.
```
[bluetooth]# pair XX:XX:XX:XX:XX:XX
[bluetooth]# trust XX:XX:XX:XX:XX:XX
```
Now the Bluetooth device should automatically connect to the Pi. To check paired devices,
```
[bluetooth]# paired-devices
```
