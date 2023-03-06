#include <Arduino.h>
#include <Joystick.h>
#include <button.h>

Joystick_ Joystick(
    0x03, // HID report ID
    JOYSTICK_TYPE_GAMEPAD,
    16,    // Number of buttons
    0,     // Number of hat switches
    true,  // X axis
    true,  // Y axis
    true,  // Z axis
    false, // X rotation
    false, // Y rotation
    true,  // Z rotation
    false, // Rudder
    false, // Throttle
    false, // Accelerator pedal
    false, // Brake pedal
    false  // Steering wheel
);

Button Button1(2, false);
Button Button2(3, false);

int lightsDimmerButton = 13;
int lightsBrighterButton = 14;

bool button1LastPressed = false;
bool button2LastPressed = false;

// Set defaults for buttons and axes
void setJoystickDefaults()
{
  Joystick.setXAxis(512);
  Joystick.setYAxis(512);
  Joystick.setZAxis(512);
  Joystick.setRzAxis(512);

  button1LastPressed = false;
  Joystick.setButton(lightsDimmerButton, 0);

  button2LastPressed = false;
  Joystick.setButton(lightsBrighterButton, 0);
}

void setup()
{
  // Initialize Button Pins
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  // Initialize Joystick Library
  Joystick.begin();
  setJoystickDefaults();
}

volatile bool calibrationMode = false;
int calibrationStep = 0;

void loop()
{
  Button1.update();
  Button2.update();

  button_state_t button1State = Button1.get_state();
  button_state_t button2State = Button2.get_state();

  // Long press both buttons for calibration mode
  if (button1State == BUTTON_LONG_HOLD_START && button1State == BUTTON_LONG_HOLD_START)
  {
    calibrationMode = true;
    calibrationStep = 0;
    setJoystickDefaults();
  }

  if (calibrationMode)
  {
    bool button1Pressed = (button1State == BUTTON_SHORT_PRESS);
    bool button2Pressed = (button2State == BUTTON_SHORT_PRESS);

    // Button 1 cycles through calibration steps.
    // These may not be strictly correct in direction, but OCD aside it doesn't matter
    if (button1Pressed)
    {
      switch (calibrationStep)
      {
      case 0:
        Joystick.setYAxis(0);
        calibrationStep++;
        break;
      case 1:
        Joystick.setYAxis(1023);
        calibrationStep++;
        break;
      case 2:
        Joystick.setYAxis(512);
        Joystick.setXAxis(0);
        calibrationStep++;
        break;
      case 3:
        Joystick.setXAxis(1023);
        calibrationStep++;
        break;
      case 4:
        Joystick.setXAxis(512);
        Joystick.setZAxis(1023);
        calibrationStep++;
        break;
      case 5:
        Joystick.setZAxis(0);
        calibrationStep++;
        break;
      case 6:
        Joystick.setZAxis(512);
        Joystick.setRzAxis(0);
        calibrationStep++;
        break;
      case 7:
        Joystick.setRzAxis(1023);
        calibrationStep++;
        break;
      case 8:
        calibrationStep = 0;
        calibrationMode = false;
        setJoystickDefaults();
        break;
      }
    }

    // Exit callibration and restore defaults
    if (button2Pressed)
    {
      calibrationStep = 0;
      calibrationMode = false;
      setJoystickDefaults();
    }
  }
  else
  {
    bool button1IsPressed = Button1.is_pressed();
    bool button2IsPressed = Button2.is_pressed();

    // Handle dimmer button state
    if (button1IsPressed != button1LastPressed)
    {
      Joystick.setButton(lightsDimmerButton, button1IsPressed ? 1 : 0);
      button1LastPressed = button1IsPressed;
    }

    // Handle brighter button state
    if (button2IsPressed != button2LastPressed)
    {
      Joystick.setButton(lightsBrighterButton, button2IsPressed ? 1 : 0);
      button2LastPressed = button2IsPressed;
    }
  }
}