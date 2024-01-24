# StakeBot_Arm_Drivers

This repository includes the Arduino code used to control the StakeBot's arms by controlling the stepper motors through serial communication

## Setup

### Wiring 
First checkout the excel file "Custom Stepper Motor Pinout" to see how to wire the Pololu High Powered Stepper Motor Drivers to the Arduino Micro.
Each of these wire do not require any resistors, ect. except for the buttons.

> Remember that a button is basically a circuit that wires voltage back to ground (through a resistor) with a "probe like" wire that feeds back to a digital port. Check out this [link](https://docs.arduino.cc/built-in-examples/digital/Button/) for an example of how to wire together a button.

For this driver, there are 2 Pololu High Power Stepper Motor Drivers(driver A, driver B) and 4 buttons two for each driver (Inner A, Outter A, Inner B, Outter B) 

### Button Test
After wiering together the hardware first verify that your buttons are working by uploading the code within the Button Test folder. Next open the serial monitor with the baud rate set to 115200 and you should see 4 zeros, upon the press of each button these zeros should become ones, if you see 4 ones and upon the press of a button it becomes zero, you need to move the probe wire to the other side of the button.

## Running The Driver

After ensuring that the buttons are wired correctly and working, next you need to install the Pololu High Power Stepper Driver library by typing its name into Arduino's library manager and installing it.

After this you should be able to compile and upload the code.

## Using The Driver

These stepper motor drivers work by performing an action given by sending a command through the serial monitor. This means that in order to uses these driver you must write to the Arduino via serial communication (UART) 

There are two ways to do this:

- You can open the serial monitor inside the Arduino IDE and type the commands and send them to the arduino. 
- You can try running the python script where it will connect to the arduino and allow you to type commands. 

> Remember to change the stepDev list to the correct serial port

The avaiable commands are:
- calibrate
    - calibrates the motors to a zero position
- goToPosition\n{motor position A},{motor position B}
    - this will move each motor the the selected position given in inches
- getPosition\n{motor A or B}
    - this will return the position of motor A or B in inches
- measureBounds\n{motor A or B}
    - This will cause the motors to measure the available distance from within its bounds and returns that distance in inches

> I have not tested the python script, I pulled it off from an eariler version of the StakeBot's code so im not sure if it will work.




