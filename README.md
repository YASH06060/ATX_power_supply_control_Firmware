# ATX Power Supply Control Firmware

This project contains the firmware source code for an embedded controller designed to manage an ATX power supply with multiple output voltages (12V, 5V, 3.3V). It features user interaction via buttons, LED status indication, buzzer notifications, and various power states including standby.

## Features

- Controls 12V, 5V, and 3.3V DC outputs based on user selection.
- Provides status indication using multi-color LEDs (green, red, orange).
- Supports a buzzer alarm for notifications and feedback.
- Buttons for scrolling through output modes and selecting outputs.
- Implements debouncing, long-press detection, and multi-button interactions.
- Enters standby mode after extended inactivity, with periodic buzzer alerts.
- Uses AVR microcontroller timers and interrupts for accurate time-keeping.
- Startup buzzer sound and LED indications for system status.
- Power supply on/off control via hardware and firmware logic.

## Hardware Connections

- **Green and Red LEDs** for scroll and select indicators connected to PORTB and PORTC pins.
- **Buzzer** connected to PORTB.
- **Buttons** for Scroll and Select connected to PORTD inputs.
- **Power Output Control Pins** for 12V, 5V, and 3.3V connected to PORTD outputs.
- **Power Supply On (PS_ON)** control via a PORTC pin.
- Status pin connected to PORTD.

## Functional Overview

- **Button Logic:** Handles button press events, including short and long presses, cycling through output states and toggling outputs on/off.
- **State Machine:** Manages three LED states (Green, Red, Orange) representing active output channels or combinations.
- **Timer Interrupt:** Maintains a second counter for time-based operations like long-press detection and standby duration.
- **Standby Mode:** Powers down outputs after inactivity; wakes on button press.
- **Buzzer Feedback:** Audible signals for button presses, warnings, and mode changes.
- **LED Scrolling & Status:** Visual feedback for current system and output states.

## Usage

1. Compile the code for your AVR microcontroller using your preferred IDE (e.g., MPLAB, Atmel Studio).
2. Connect the hardware according to the pin mapping specified.
3. Upload the firmware to the microcontroller.
4. Use the scroll and select buttons to change output voltages.
5. Hold both buttons for 4 seconds to enter standby mode.
6. The system will periodically beep during standby after 2 minutes of inactivity.
7. Press any button to wake up from standby.

## Development Environment

- Target Microcontroller: AVR (e.g., ATmega series)
- Compiler: GCC AVR / MPLAB / Atmel Studio
- Libraries: AVR standard headers, util/delay for timing, avr/interrupt for ISR handling

## Notes

- Ensure hardware is wired correctly to avoid damage.
- Debounce time and long-press duration can be adjusted in firmware as needed.
- Modify `LONG_PRESS_SEC` and timing constants to suit different hardware or user preferences.
- Buzzer and LED behavior can be customized for alternative notification schemes.


---

This embedded firmware project demonstrates control of a multi-output power supply with user-friendly interface mechanisms on a low-level microcontroller platform.
