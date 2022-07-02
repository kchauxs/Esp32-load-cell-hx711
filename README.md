# Esp32 load cell hx711

Electronic scale with the ESP32 using a load cell and the HX711 amplifier.

## Functioning

The project consists of 2 buttons:

- **MODE** button: It is used to enter the calibration menu and select the weights.
- **TARE** button: Used to tare a weight and to enter the calibration menu.

Once the circuit is energized, if the MODE and TARE buttons are pressed at the same time, the system enters the calibration stage, where a known weight is placed on the scale to adjust the scaling factor.

When started, the system tares the initial weight and displays the weight, which can display units of grams. Some weight can be tared in the project by means of the TARE button.
