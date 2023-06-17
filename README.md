# BizarroHomer
Code for FRC Team 1511's T-Shirt Robot, BizarroHomer (a.k.a. "ThunderShot").

The robot is controlled from a Raspberry Pi which takes input from a single Bluetooth DualShock4 controller. Controlling the robot using a Raspberry Pi gives us much more flexibility for control over solutions such as the NI roboRIO. Since BizarroHomer (a.k.a. "ThunderShot") was designed specifically for use at demonstrations and outreach events for the team, a simple and straightforward control solution was greatly desired.

### Table of Contents
* [Setup](#setup)
* [Software Structure](#software-structure)
   - [Main Program](#main-program)
   - [Hardware Control Program](#hardware-control-program)
   - [Diagnostic Server Program](#diagnostic-server-program)
   - [DualShock4 LED Control Program](#dualshock4-led-control-program)
* [Pairing DualShock4 Controller](#pairing-dualshock4-controller)
* [DualShock4 LED Status Patterns](#dualshock4-led-status-paterns)

## Setup
1. Add to `boot/config.txt` to setup hardware.
   - Enable pwm0 and pwm1.
     ```
     dtoverlay=pwm-2chan,pin=12,func=4,pin2=13,func2=4
     ```
   - Enable CAN (I'm currently using [this RS485 CAN HAT](https://www.waveshare.com/rs485-can-hat.htm). Qther CAN interfaces __will__ be different).
     ```
     dtparam=spi=on
     dtoverlay=mcp2515-can0,oscillator=12000000,interrupt=25,spimaxfrequency=2000000
     ```
2. Setup workspace.
   - Create directory `/var/frc151/BizarroHomer` and give the `pi` user permissions to access it.
     ```bash
     $ sudo mkdir -p /var/frc1511/BizarroHomer
     $ sudo chown pi: /var/frc1511
     $ sudo chmod u+w /var/frc1511
     ```
   - Create IPC message queue key file.
     ```bash
     $ touch /var/frc1511/BizarroHomer/ipc_msg_queue_key
     ```
3. Install tools.
   ```
   $ sudo apt-get update
   $ sudo apt-get install -y --no-install-recommends bluez libsdl2-dev
   ```
4. Compile and install programs using CMake.
5. Restart for systemd services to startup.
6. [Pair a DualShock4 controller](#pairing-dualdhock4-controller)

## Software Structure

The software is split up into several different programs, each with different permissions and purposes.
### Main Program
The main program is run as the pi user. It reads controller input and handles the main logic of the robot software. It communicates with the [Hardware Control](#hardware-control-program) program to interact with robot hardware and the [DualShock4 LED Control](#dualshock4-led-control-program) program to control the color of the DualShock4 controller's LEDs.

### Hardware Control Program
The hardware control program is run as the root user. It contols the motors, encoders, etc. on the robot. It receives control messages to control hardware, and sends status messages containing sensor input. This program will disable all hardware outputs when the main program stops.

### Diagnostic Server Program
The diagnostic server program is run as the root user. It hosts a HTTP/1.1 web server on port 80 to provide diagnostic information about the running status of the robot. The program receives messages from the main program containing values that are wished to be displayed on the site. The site updates values every 0.5 seconds from the server. The site also displays stdout and stderr for each of the running programs.

### DualShock4 LED Control Program
The DualShock4 LED control program is run as the root user. It controls the LEDs on the DualShock4 controller. The LED colors signify different states/errors of the robot program.

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

## DualShock4 LED Status Paterns
The LEDs on the DualShock4 controller is used to signify different states/errors of the robot program. 

### Independent States
Some independent LED states for the controller.

| State          | Description |
| -------------- | ----------- |
| Flashing White | Controller is searching for the robot. Give it some time to connect. If controller turns off, turn it back on again until it connects. If continuously failing to connect, make sure that the controller is paired and trusted. |
| Solid Blue     | Controller is connected, although the main program and/or LED control program is not running. Wait for them to start up. |
| Flashing Blue  | Main program stopped/crashed. It should start up again in a few seconds. |
| Off            | Controller if off/dead or LED control program isn't running. Most likely the former though. |

### Colors
These colors may be blinking in combination with each other. Every color signifies a different thing.

| Color   | Description |
| ------- | ----------- |
| Red     | Robot battery is low (less than 11.8 V). Battery should be replaced for the robot to function properly. |
| Yellow  | Controller battery is low (less than 30 percent). Controller should be charged or plugged into robot to elimilate risk of bad things happening. |
| Orange  | Robot program doing setup. Give it some time to become responsive to inputs. |
| Green   | Robot program is running fine. No pressure stored in fill tank (presumably). |
| Magenta | Fill tank is pressurized. Be cautious around the robot. |
