// Simple example to embed pictures in your sketch
// and draw on the ILI9341 display with writeRect()
//
// By Frank Bösing
//
// https://forum.pjrc.com/threads/32601-SPI-Library-Issue-w-ILI9341-TFT-amp-PN532-NFC-Module-on-Teensy-3-2?p=94534&viewfull=1#post94534

// build_flags = -D USB_MIDI_AUDIO_SERIAL
#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce2.h>
#include "ILI9341_t3.h"
#include <XPT2046_Touchscreen.h>

// Images Backgrounds
#include "MenuPNGPage1.c" //Home
#include "MenuPNGPage2.c" //Effets
#include "MenuPNGPage3.c" //Analyseur

const int BUTTON_PIN = 21;
const int BUTTON1_PIN = 32;
const int BUTTON2_PIN = 31;
const int BUTTON3_PIN = 30;
const int BUTTON4_PIN = 29;

// Écran pins
#define TFT_DC      9
#define TFT_CS      10
#define TFT_RST     34  // 255 = unused, connect to 3.3V
#define TFT_MOSI    11
#define TFT_SCLK    13
#define TFT_MISO    12

// Touch Screen pins
#define CS_PIN 8
#define Tirq 2

// Calibration datas
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// Box Sizes
#define BOXSIZE 40
#define PENRADIUS 0.8

//MAX 0x4B is the default i2c address
#define MAX9744_I2CADDR 0x4B

// Pad Constantes
#define PADSIZE 45
bool EtatBouton1 = 0;
bool EtatBouton2 = 0;
bool EtatBouton3 = 0;
bool EtatBouton4 = 0;
bool EtatBouton5 = 0;
bool EtatBouton6 = 0;
bool EtatBouton7 = 0;
bool EtatBouton8 = 0;
bool EtatBouton9 = 0;

// Etat Bouton Variables
int Bouton = 1;

// Etat Menu Variables
int Etat = 1;

// Volume Variables
float Slide = 96;
float SlideBuffer = 96;
float PourcentageVolume = 50;
int8_t thevol = 31;

// Mixer Variables
float Mixer1 = 140, Mixer2 = 140, Mixer3 = 140, Mixer4 = 140;
float Mixer1Buffer = 140, Mixer2Buffer = 140, Mixer3Buffer = 140, Mixer4Buffer = 140;
float PourcentageMixer1 = 0, PourcentageMixer2 = 0, PourcentageMixer3 = 0, PourcentageMixer4 = 0;

// INSTANTIATE A Button OBJECT (Menu)
Bounce button = Bounce();

// INSTANTIATE A Button OBJECT (PADS)
Bounce button1 = Bounce();
Bounce button2 = Bounce();
Bounce button3 = Bounce();
Bounce button4 = Bounce();

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);
XPT2046_Touchscreen ts(CS_PIN, Tirq);

// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav1;     //xy=151,241
AudioPlaySdWav           playSdWav4;     //xy=155,345
AudioPlaySdWav           playSdWav2;     //xy=156,275
AudioPlaySdWav           playSdWav3;     //xy=156,308
AudioMixer4              mixer2; //xy=378,351
AudioMixer4              mixer1;         //xy=386,216
//AudioEffectFreeverb      freeverb2; //xy=522,288
//AudioEffectFreeverb      freeverb1;      //xy=523,254
AudioMixer4              mixer4; //xy=673,346
AudioMixer4              mixer3;         //xy=679,198
AudioOutputI2S           i2s1;           //xy=937,225
AudioConnection          patchCord1(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord2(playSdWav1, 1, mixer2, 0);
AudioConnection          patchCord3(playSdWav4, 0, mixer1, 3);
AudioConnection          patchCord4(playSdWav4, 1, mixer2, 3);
AudioConnection          patchCord5(playSdWav2, 0, mixer1, 1);
AudioConnection          patchCord6(playSdWav2, 1, mixer2, 1);
AudioConnection          patchCord7(playSdWav3, 0, mixer1, 2);
AudioConnection          patchCord8(playSdWav3, 1, mixer2, 2);
AudioConnection          patchCord9(mixer2, mixer4);
AudioConnection          patchCord10(mixer1, mixer3);
//AudioConnection          patchCord11(freeverb2, 0, mixer4, 3);
//AudioConnection          patchCord12(freeverb1, 0, mixer3, 0);
AudioConnection          patchCord13(mixer4, 0, i2s1, 1);
AudioConnection          patchCord14(mixer3, 0, i2s1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=932,414
// GUItool: end automatically generated code

// Use these with the Teensy 3.5 & 3.6 SD card
#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 11 // not actually used
#define SDCARD_SCK_PIN 13  // not actually used

//Print
void ImagePrint(void);
void VolumePrint(void);
void MixerPrint(void);
void PadPrint(char);
void ReprintPads(void);
//Test
void TestBoutonAppuye(void);
void TestTactilAppuye(void);
void TestPadAppuye(void);
bool setvolume(int8_t);
void playFile(const char *, int);

void setup() {

  Serial.begin(9600);
  Wire1.begin();
  AudioMemory(20);
  tft.begin();

  // Attache Input bouton
  button1.attach(BUTTON1_PIN, INPUT); // USE EXTERNAL PULL-UP
  button2.attach(BUTTON2_PIN, INPUT); // USE EXTERNAL PULL-UP
  button3.attach(BUTTON3_PIN, INPUT); // USE EXTERNAL PULL-UP
  button4.attach(BUTTON4_PIN, INPUT); // USE EXTERNAL PULL-UP
  button.attach( BUTTON_PIN, INPUT ); // USE EXTERNAL PULL-UP
  
  // DEBOUNCE INTERVAL IN MILLISECONDS
  button1.interval(5);
  button2.interval(5);
  button3.interval(5);
  button4.interval(5);
  button.interval(5);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN)))
  {
    // stop here, but print a message repetitively
    while (1)
    {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  Serial.println("Accessed the SD card");

  //LCD (ILI9341)
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }

  //MAX9744(Amp)
  if (!setvolume(thevol)) {
    Serial.println("Failed to set volume, MAX9744 not found!");
    while (1);
  }
  
  Serial.println("Touchscreen started");
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  ImagePrint();
  VolumePrint();
}

void loop(void) {

  button1.update();
  button2.update();
  button3.update();
  button4.update();
  button.update();

  if (button1.rose())
  {
    playFile("HI-HATS1.wav", 1);     // filenames are always uppercase 8.3 format
  }

  else if (button2.rose())
  {
    playFile("KICK1.wav", 2);
  }

  else if (button3.rose())
  {
    playFile("SNARE1.wav", 3);
  }

  else if (button4.rose())
  {
    playFile("PERC1.wav", 4);
  }

  

  // Si bouton appuyé
  if ( button.rose() ) {
    TestBoutonAppuye();
  }

  // Si ecran tactile appuyé
  else if (ts.touched()) {
    TestTactilAppuye();
  }

  if (Serial.available()) {
    TestPadAppuye();
  }

  delay(100);

}

//////////////////////////////////////////////////////////////////////////////////////
/*                              Fonction Affichage Image                            */
//////////////////////////////////////////////////////////////////////////////////////

void ImagePrint() {

  switch (Etat) {
    case 1:
      //tft.fillScreen(ILI9341_BLACK);
      tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);
      ReprintPads();

      break;

    case 2:
      //tft.fillScreen(ILI9341_BLACK);
      tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage2);

      tft.fillRoundRect(97,  26, 10, 136, 5, ILI9341_BLACK);
      tft.fillRoundRect(158, 26, 10, 136, 5, ILI9341_BLACK);
      tft.fillRoundRect(218, 26, 10, 136, 5, ILI9341_BLACK);
      tft.fillRoundRect(279, 26, 10, 136, 5, ILI9341_BLACK);

      tft.drawRoundRect(97,  26, 10, 136, 5, ILI9341_WHITE);
      tft.drawRoundRect(158, 26, 10, 136, 5, ILI9341_WHITE);
      tft.drawRoundRect(218, 26, 10, 136, 5, ILI9341_WHITE);
      tft.drawRoundRect(279, 26, 10, 136, 5, ILI9341_WHITE);

      break;

    case 3:
      //tft.fillScreen(ILI9341_BLACK);
      tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage3);
      break;

    default:
      //tft.fillScreen(ILI9341_BLACK);
      tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);
      break;
  }
}

//////////////////////////////////////////////////////////////////////////////////////
/*                           Fonction Affichage Volume                              */
//////////////////////////////////////////////////////////////////////////////////////

void VolumePrint() {
  tft.fillRect(73, 2, int16_t(Slide), 16/*+1*/, ILI9341_RED);
  tft.setCursor(48, 8);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(int16_t(PourcentageVolume)); tft.println('%');
  
  thevol = float(PourcentageVolume / 100 * 63 + 7);
  if (thevol > 63) thevol = 63;
  if (thevol < 0) thevol = 0;
  setvolume(thevol);
  //Serial.print(thevol);
}

//////////////////////////////////////////////////////////////////////////////////////
/*                           Fonction Affichage Mixer1                              */
//////////////////////////////////////////////////////////////////////////////////////

void MixerPrint() {
  tft.fillRoundRect(80, Mixer1, 44, 22, 5, ILI9341_BLACK); //97-22+5 | 162-22
  tft.fillRoundRect(141, Mixer2, 44, 22, 5, ILI9341_BLACK); //158-22+5 | 162-22
  tft.fillRoundRect(201, Mixer3, 44, 22, 5, ILI9341_BLACK); //218-22+5 | 162-22
  tft.fillRoundRect(262, Mixer4, 44, 22, 5, ILI9341_BLACK); //279-22+5 | 162-22

  tft.drawRoundRect(80, Mixer1, 44, 22, 5, ILI9341_WHITE);
  tft.drawRoundRect(141, Mixer2, 44, 22, 5, ILI9341_WHITE);
  tft.drawRoundRect(201, Mixer3, 44, 22, 5, ILI9341_WHITE);
  tft.drawRoundRect(262, Mixer4, 44, 22, 5, ILI9341_WHITE);

  tft.setCursor(80, 182);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Mixer1:");
  tft.setCursor(77, 200);
  tft.setTextSize(3);
  tft.print(int16_t(PourcentageMixer1)); tft.println('%');

  tft.setCursor(141, 182);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Mixer2:");
  tft.setCursor(138, 200);
  tft.setTextSize(3);
  tft.print(int16_t(PourcentageMixer2)); tft.println('%');

  tft.setCursor(202, 182);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Mixer3:");
  tft.setCursor(199, 200);
  tft.setTextSize(3);
  tft.print(int16_t(PourcentageMixer3)); tft.println('%');

  tft.setCursor(263, 182);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Mixer4:");
  tft.setCursor(260, 200);
  tft.setTextSize(3);
  tft.print(int16_t(PourcentageMixer4)); tft.println('%');
}

//////////////////////////////////////////////////////////////////////////////////////
/*                           Fonction Affichage Pad                                 */
//////////////////////////////////////////////////////////////////////////////////////

void PadPrint(char bouton) {

  switch (bouton) {
    case '7':

      EtatBouton1 = !EtatBouton1;

      if (EtatBouton1 == 1) {
        tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '8':

      EtatBouton2 = !EtatBouton2;

      if (EtatBouton2 == 1) {
        tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '9':

      EtatBouton3 = !EtatBouton3;

      if (EtatBouton3 == 1) {
        tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '4':

      EtatBouton4 = !EtatBouton4;

      if (EtatBouton4 == 1) {
        tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '5':

      EtatBouton5 = !EtatBouton5;

      if (EtatBouton5 == 1) {
        tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '6':

      EtatBouton6 = !EtatBouton6;

      if (EtatBouton6 == 1) {
        tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '1':

      EtatBouton7 = !EtatBouton7;

      if (EtatBouton7 == 1) {
        tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '2':

      EtatBouton8 = !EtatBouton8;

      if (EtatBouton8 == 1) {
        tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '3':

      EtatBouton9 = !EtatBouton9;

      if (EtatBouton9 == 1) {
        tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    default:
      break;
  }
}

//////////////////////////////////////////////////////////////////////////////////////
/*                              Fonction Test Bouton                                */
//////////////////////////////////////////////////////////////////////////////////////

void TestBoutonAppuye() {

  if (Bouton <= 3) {
    Bouton = Bouton + 1;
  }
  else {
    Bouton = 1;
  }

  switch (Bouton) {
    case 1:
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 1;
      ImagePrint();
      VolumePrint();
      break;

    case 2:
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 2;
      ImagePrint();
      VolumePrint();
      break;

    case 3:
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 3;
      ImagePrint();
      VolumePrint();
      break;

    default:
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 1;
      ImagePrint();
      VolumePrint();
      break;
  }
}

//////////////////////////////////////////////////////////////////////////////////////
/*                              Fonction Test Tactil                                */
//////////////////////////////////////////////////////////////////////////////////////

void TestTactilAppuye() {

  // Retrieve a point
  TS_Point p = ts.getPoint();

  // Scale using the calibration #'s
  // and rotate coordinate system
  p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
  p.y = map(p.y, TS_MAXX, TS_MINX, 0, tft.width());

  Serial.print("X = "); Serial.print(p.x);
  Serial.print("\tY = "); Serial.print(p.y);
  Serial.print("\tPressure = "); Serial.println(p.z);

  //Boutons tactils
  if (p.x > 180) {

    if ((p.y > 40) && (p.y < 120)) {
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 1;
      ImagePrint();
      VolumePrint();
    } else if ((p.y > 130) && (p.y < 210)) {
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 2;
      ImagePrint();
      VolumePrint();
      MixerPrint();
    } else if ((p.y > 255) && (p.y < 304)) {
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 3;
      ImagePrint();
      VolumePrint();
    }
  }
  //Volume
  else if ((p.y >= 0) && (p.y < 30)) {
    if ((p.x > 50) && (p.x < 180)) {
      Slide = float((p.x - 181.0) / -0.6842);
      PourcentageVolume = float(100.0 * Slide / 190.0);
      Serial.println(PourcentageVolume);
      Serial.println(Slide);
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);
      if ((Slide > SlideBuffer + 4) || (Slide < SlideBuffer - 4)) {
        ImagePrint();
        VolumePrint();
        if (Etat == 2) {
          MixerPrint();
        }

      }
      SlideBuffer = Slide;
    }
  }
  //Mixer1
  else if ((Etat == 2) && ((p.x >= 150) && (p.x < 175))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer1 = float((0.67059 * p.y) - 0.7);
      PourcentageMixer1 = float(-0.877143 * Mixer1 + 122.80702);
      Serial.print(PourcentageMixer1); Serial.println("%");
      Serial.println(Mixer1);
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);
      ImagePrint();
      VolumePrint();
      MixerPrint();
    }
  }
  //Mixer2
  else if ((Etat == 2) && ((p.x >= 110) && (p.x < 135))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer2 = float((0.67059 * p.y) - 0.7);
      PourcentageMixer2 = float(-0.877143 * Mixer2 + 122.80702);
      Serial.print(PourcentageMixer2); Serial.println("%");
      Serial.println(Mixer2);
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);
      ImagePrint();
      VolumePrint();
      MixerPrint();
    }
  }
  //Mixer3
  else if ((Etat == 2) && ((p.x >= 70) && (p.x < 95))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer3 = float((0.67059 * p.y) - 0.7);
      PourcentageMixer3 = float(-0.877143 * Mixer3 + 122.80702);
      Serial.print(PourcentageMixer3); Serial.println("%");
      Serial.println(Mixer3);
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);
      ImagePrint();
      VolumePrint();
      MixerPrint();
    }
  }
  //Mixer4
  else if ((Etat == 2) && ((p.x >= 25) && (p.x < 50))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer4 = float((0.67059 * p.y) - 0.7);
      PourcentageMixer4 = float(-0.877143 * Mixer4 + 122.80702);
      Serial.print(PourcentageMixer4); Serial.println("%");
      Serial.println(Mixer4);
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);
      ImagePrint();
      VolumePrint();
      MixerPrint();
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////
/*                              Fonction Test Pad                                   */
//////////////////////////////////////////////////////////////////////////////////////

void TestPadAppuye() {
  char receivedChar = Serial.read();
  PadPrint(receivedChar);
}

//////////////////////////////////////////////////////////////////////////////////////
/*                              Reprint Pads                                        */
//////////////////////////////////////////////////////////////////////////////////////

void ReprintPads() {
  if (EtatBouton1 == 1) {
    tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton2 == 1) {
    tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton3 == 1) {
    tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton4 == 1) {
    tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton5 == 1) {
    tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton6 == 1) {
    tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton7 == 1) {
    tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton8 == 1) {
    tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton9 == 1) {
    tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
}

// Setting the volume (Ampli)
boolean setvolume(int8_t v) {
  // cant be higher than 63 or lower than 0
  if (v > 63) v = 63;
  if (v < 0) v = 0;
  
  Serial.print("Setting volume to ");
  Serial.println(v);
  Wire1.beginTransmission(MAX9744_I2CADDR);
  Wire1.write(v);
  if (Wire1.endTransmission() == 0) 
    return true;
  else
    return false;
}

void playFile(const char *filename, int chord)
{
  Serial.print("Playing file: ");
  Serial.println(filename);
  Serial.print("Chord: ");
  Serial.println(chord);

  switch (chord)
  {
  case 1:
    playSdWav1.play(filename);
    delay(25);
    break;
  case 2:
    playSdWav2.play(filename);
    delay(25);
    break;
  case 3:
    playSdWav3.play(filename);
    delay(25);
    break;
  case 4:
    playSdWav4.play(filename);
    delay(25);
    break;
  default:

    break;
  }
}
