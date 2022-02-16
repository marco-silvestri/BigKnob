#include <Arduino.h>
#include <BleKeyboard.h>
#include <Encoder.h>
#include <analogWrite.h>
#include <Adafruit_NeoPixel.h>

long oldPosition  = -999;
bool buttonState, oldButtonState;
int buttonPin = 5;
int tickPin = 22;
unsigned long t1, dt;
unsigned long chatteringThreshold = 50;
bool upperLayer = false;
//int led = 17;

BleKeyboard bleKeyboard("bigKnob2", "bigKnob2", 100);
Encoder myEnc(19, 18);

#define PIN 17
#define NUMPIXELS 2
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  //pinMode(led, OUTPUT);
  //digitalWrite(led, LOW);
  bleKeyboard.begin();
  pixels.begin();
  ledsOn(50, 0, 0);
  delay(1000);
}

void loop() {
  if (bleKeyboard.isConnected())
  {
    if (!upperLayer)
    {
      ledsOn(0, 0, 50);
    } else
    {
      ledsOn(0, 50, 0);
    }

    buttonState = digitalRead(buttonPin);
    long newPosition = myEnc.read();
    if (buttonState == LOW && oldButtonState == HIGH)
    {
      ledsOff();
      ledsOn(80, 30, 0);
      t1 = millis();
    }

    if (buttonState == LOW && oldButtonState == LOW)
    {
      dt = millis() - t1;
      if (dt > 1000)
      {
        upperLayer = !upperLayer;
        //digitalWrite(led, upperLayer);
        if (upperLayer)
        {
          bleKeyboard.press(KEY_F5);
        }
        
        delay(200);
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
        tone(tickPin, 800, 5);
        if (!upperLayer)
        {
          bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
        } else
        {
          bleKeyboard.write(KEY_F10);
        }
      } else if (newPosition < oldPosition)
      {
        tone(tickPin, 800, 5);
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
}

void tone(uint8_t _pin, unsigned int frequency, unsigned long duration) {
  pinMode (_pin, OUTPUT );
  analogWriteFrequency(frequency);
  analogWrite(_pin, 500);
  delay(duration);
  analogWrite(_pin, 0);
}

void ledsOn(int r, int g, int b) {
  pixels.clear();
  pixels.setPixelColor(0, r, g, b);
  pixels.setPixelColor(1, r, g, b);
  pixels.show();
}


void ledsOff() {
  pixels.clear();
  pixels.setPixelColor(0, 0, 0, 0);
  pixels.setPixelColor(1, 0, 0, 0);
  pixels.show();
}
