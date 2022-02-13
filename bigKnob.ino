#include <Arduino.h>
#include <BleKeyboard.h>
#include <Encoder.h>

BleKeyboard bleKeyboard("bigKnob", "bigKnob", 100);
Encoder myEnc(19, 18);
long oldPosition  = -999;
bool buttonState, oldButtonState;
int buttonPin = 5;
unsigned long t1, dt;
unsigned long chatteringThreshold = 50;
bool upperLayer = false;
int led = 17;
void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  bleKeyboard.begin();
}

void loop() {
  buttonState = digitalRead(buttonPin);
  long newPosition = myEnc.read();

  if (buttonState == LOW && oldButtonState == HIGH)
  {
    t1 = millis();
  }

  if (buttonState == LOW && oldButtonState == LOW)
  {

    dt = millis() - t1;
    if (dt > 1000)
    {
      upperLayer = !upperLayer;
      digitalWrite(led, upperLayer);
      if (upperLayer)
      {
        bleKeyboard.press(KEY_F5);
      }
      delay(500);
    }
  }

  if (buttonState == HIGH && oldButtonState == LOW)
  {
    if (dt < 1000 && dt > chatteringThreshold)
    {
      if (!upperLayer)
      {
        bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
      } else
      {
        bleKeyboard.press(KEY_LEFT_CTRL);
        bleKeyboard.press(KEY_LEFT_SHIFT);
        bleKeyboard.press(KEY_F5);
      }
    }
  }

  oldButtonState = buttonState;

  if (newPosition != oldPosition) {
    if (newPosition > oldPosition)
    {
      if (!upperLayer)
      {
        bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
      } else
      {
        bleKeyboard.write(KEY_F10);
      }
    } else if (newPosition < oldPosition)
    {
      if (!upperLayer)
      {
        bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
      } else
      {
        bleKeyboard.write(KEY_F11);
      }
    }
    oldPosition = newPosition;
    delay(50);
  }

  bleKeyboard.releaseAll();
}
