# Arduino Ultrasonic Security Alarm

This project implements a security system using an Arduino Nano , an HC-SR04 ultrasonic sensor for distance detection, a buzzer for the alarm sound, and three buttons for a custom keypad input.

## Features
* **Arming:** Pressing the arm button sets a base distance (with a 2-second delay to allow leaving the area).
* **Detection:** If the distance changes by more than 30cm, the alarm is triggered.
* **Disarming:** Enter a 5-digit code using the 3 buttons to stop the alarm.
* **Visual/Audio Feedback:** Uses a buzzer and an LED to indicate status (armed, disarmed, wrong code).

## Hardware Components
* Arduino Nano 
* HC-SR04 Ultrasonic Sensor
* Buzzer
* LED
* 3x Push Buttons (for code input)
* 1x Push Button (for arming)
* Resistors (if not using `INPUT_PULLUP`)

## Wiring
| Component | Arduino Pin |
| :--- | :--- |
| Ultrasonic Trig | 9 |
| Ultrasonic Echo | 10 |
| Arm Button | 2 |
| Keypad Button 1 | 3 |
| Keypad Button 2 | 4 |
| Keypad Button 3 | 5 |
| Buzzer | 6 |
| LED | 13 |

## Code Configuration
You can change the security code in the code:
```cpp
int masterCode[] = {3, 3, 1, 2, 2};
