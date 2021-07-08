#include "arduinoFFT.h"
//#include <SoftwareSerial.h>

#define SAMPLES 128             //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 2048 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.
 
arduinoFFT FFT = arduinoFFT();
 
unsigned int samplingPeriod;
unsigned long microSeconds;
 
double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values


#define MIC A0
#define RED_PIN 9
#define GREEN_PIN 6
#define BLUE_PIN 5

String data = "";
int period = 3;
int r = 0;
int g = 0;
int b = 0;
double f = 0;
bool on = false; //false
int arrayScale = 0;
String rgbColours[] = {};
//==============================================================
//                     LED FUNCTIonS
//==============================================================

void changeColour (int r, int g, int b, int nr, int ng, int nb) {
    double deltar = r - nr;
    double deltag = g - ng;
    double deltab = b - nb;
    double deltari = deltar / 50;
    double deltagi = deltag / 50;
    double deltabi = deltab / 50;
    double newr;
    double newg;
    double newb;
      
        for (int i = 0; i < 51; i++) {
        newr = r  - deltari * i;
        newg = g  - deltagi * i;
        newb = b  - deltabi * i;
              if (newr < 0) newr = 0.0;
              if (newg < 0) newg = 0.0;
              if (newb < 0) newb = 0.0;
              if (newr > 255) newr = 255.0;
              if (newg > 255) newg = 255.0;
              if (newb > 255) newb = 255.0;
        analogWrite(RED_PIN, newr);
        analogWrite(GREEN_PIN, newg);
        analogWrite(BLUE_PIN, newb);
        delay(period);
            }
}

void change(int r, int g, int b, int nr, int ng, int nb) {
     changeColour(r, g, b, nr, ng, nb);
     changeColour(nr, ng, nb, r, g, b);
}
void beat () {
    change(r, g, b, r + 50, g - 30, b - 10) ;
}
void musical() {
    change(r, g, b, r + 30, g + 30, b + 30) ;
}
void vocal() {
    change(r, g, b, r - 50, g + 30, b + 10) ;
}
void setup() 
{
    Serial.begin(9600);
    samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY)); //Period in microseconds 
}
 bool stop = false;
void loop() 
{  
    if(Serial.available()) {
      data = Serial.readString();
      Serial.println(data);
        int n = data.length();
        char char_array[n + 1];
        strcpy(char_array, data.c_str());
        if (String(char_array[0]) == "(" ) {
          Serial.println("CHANGED COLOUR");
          //RGB colours
          String sr = String(strtok(char_array, "(-"));
          String sg = String(strtok(NULL, "-"));
          String sb = String(strtok(NULL, ")"));
          changeColour(r, g, b, sr.toInt(), sg.toInt(), sb.toInt());
          r = sr.toInt();
          g = sg.toInt();
          b = sb.toInt();          
        }
        if (String(char_array[0]) == "p" ) {
          on = 1;
        }
        if (String(char_array[0]) == "s" ) {
          on = 0;
        }
    }
    analogWrite(RED_PIN, r);
    analogWrite(GREEN_PIN, g);
    analogWrite(BLUE_PIN, b);
     if (on) {
       for(int i=0; i<SAMPLES; i++)
    {
        microSeconds = micros();    //Returns the number of microseconds since the Arduino board began running the current script. 
     
        vReal[i] = analogRead(0); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
        vImag[i] = 0; //Makes imaginary term 0 always
        
        /*remaining wait time between samples if necessary*/
        while(micros() < (microSeconds + samplingPeriod))
        {
          //do nothing
        }
    }
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    /*Find peak frequency and print peak*/
    f = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
    int foo = f;
    if (foo == 2) {
      f = 321;
      Serial.println("emp");
    }
    Serial.println(f);

//    double *f = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
//    Serial.print(f[0]);
//    Serial.print(" ");
//    Serial.println(f[1]);
            if (f < 200) {
              beat();
              Serial.println("BEAT");
            }
                                
            else if (350 <= f) {
              vocal();
              Serial.println("VOCAL");

            }
     }
//     while(1);    
}
