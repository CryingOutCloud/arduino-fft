/*

	Example of use of the FFT library
        Copyright (C) 2014 Enrique Condes

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
  In this example, the Arduino simulates the sampling of a sinusoidal 1000 Hz
  signal with an amplitude of 100, sampled at 5000 Hz. Samples are stored
  inside the vReal array. The samples are windowed according to Hamming
  function. The FFT is computed using the windowed samples. Then the magnitudes
  of each of the frequencies that compose the signal are calculated. Finally,
  the frequency with the highest peak is obtained, being that the main frequency
  present in the signal.
*/

#include "arduinoFFT.h"
#include "KickFilters.h"

arduinoFFT FFT;
/*
These values can be changed in order to evaluate the functions
*/
const uint16_t samples = 256; //This value MUST ALWAYS be a power of 2
const double signalFrequency = 1000;
// const double samplingFrequency = 100000;

const double samplingFrequency = 50000;
const uint8_t amplitude = 1000;

const double miniFrequency = 1000000;
const double miniSampling = 6000000;
const uint8_t miniAmplitude = 10;

const float fs = 23.8; //sampling frequency in Hertz (Hz)
/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
int intReal[samples];
double vReal[samples];
double vImag[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void setup()
{
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Ready");

  pinMode(LED_BUILTIN, OUTPUT);
}


void loop()
{



  doFFT();
  // BandPassFFT();
}


void doFFT()
{
  // digitalWrite(LED_BUILTIN, HIGH);
  // /* Build raw data */
  // double cycles = (((samples-1) * signalFrequency) / samplingFrequency); //Number of signal cycles that the sampling will read
  // double miniCycles = (((samples-1) * miniFrequency) / miniSampling); //Number of signal cycles that the sampling will read

  // double big;
  // double mini;
  // double sum = 0;

  // for (uint16_t i = 0; i < samples; i++)
  // {
  //   // vReal[i] = int8_t((amplitude * (sin((i * (twoPi * cycles)) / samples))) / 2.0);/* Build data with positive and negative values*/
  //   big = uint8_t((amplitude * (sin((i * (twoPi * cycles)) / samples) + 1.0)) / 2.0);/* Build data displaced on the Y axis to include only positive values*/
  //   mini = int8_t((miniAmplitude * (sin((i * (twoPi * miniCycles)) / samples))) / 2.0);

  //   vReal[i] = big + mini;
  //   vImag[i] = 0.0; //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows

  //   sum += big + mini;
  // }

  // Subtract the average value of the waveform from each data point to remove FFT spike at 0
  // double average = sum / samples;
  // for (uint16_t i = 0; i < samples; i++)
  // {
  //   vReal[i] -= average;
  //   Serial.println(vReal[i]);
  // }


  int analogPin = A3;
  double sum = 0; 

  // Read from the A3 pin for a period equal to our number of samples
  for (uint16_t i = 0; i < samples; i++)
  {
    vReal[i] = analogRead(analogPin);
    Serial.println(vReal[i]);
  }

  double average = sum / samples;

  for (uint16_t i = 0; i < samples; i++)
  {
    vReal[i] -= average;
    Serial.println(vReal[i]);
  }

  Serial.println();

  digitalWrite(LED_BUILTIN, LOW);

  FFT = arduinoFFT(vReal, vImag, samples, samplingFrequency); /* Create FFT object */
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);	/* Weigh data */
  FFT.Compute(FFT_FORWARD); /* Compute FFT */
  FFT.ComplexToMagnitude(); /* Compute magnitudes */
  PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);
  // double x = FFT.MajorPeak();
  // Serial.println(x, 6);

  while(1); /* Run Once */
  // delay(2000); /* Repeat after delay */
}


void BandPassFFT()
{
  digitalWrite(LED_BUILTIN, HIGH);

  int analogPin = A3;
  double sum = 0; 

  //Read from the A3 pin for a period equal to our number of samples
  for (uint16_t i = 0; i < samples; i++)
  {
    intReal[i] = analogRead(analogPin);
    // Serial.println(intReal[i]);
  }


  double average = sum / samples;

  for (uint16_t i = 0; i < samples; i++)
  {
    intReal[i] -= average;
    // Serial.println(intReal[i]);
  }

  Serial.println();


  // Band pass filter: https://github.com/LinnesLab/KickFilters/blob/master/examples/Example02_basic_with_fft/Example02_basic_with_fft.ino
  int16_t filtered[samples] = {0};
  int16_t filtered2[samples] = {0};
  int16_t filtered3[samples] = {0};
  int16_t filtered4[samples] = {0};
  int16_t tmpArr[samples] = {0};

  // Perform band pass filtration
  // KickFilters<int16_t>::bandpass(vReal, filtered, tmpArr, samples, 10000, 100000, 1000);

  // No filtering
  // for (uint16_t i = 0; i < samples; i++)
  // {
  //   filtered4[i] = intReal[i];
  // }

  // First order
  // KickFilters<int16_t>::highpass(intReal, filtered4, samples, 5000, 250000);

  
  // Fourth order
  KickFilters<int16_t>::highpass(intReal, filtered, samples, 10000, 250000);
  KickFilters<int16_t>::highpass(filtered, filtered2, samples, 10000, 250000);
  KickFilters<int16_t>::highpass(filtered2, filtered3, samples, 10000, 250000);
  KickFilters<int16_t>::highpass(filtered3, filtered4, samples, 10000, 250000);
  
  
  for (uint16_t i = 0; i < samples; i++)
  {
    Serial.println(filtered4[i]);
  }
  Serial.println();

  
  double output_array[samples];

  // Convert input data to array of double from int
  for (int i = 0; i < samples; i++) {
    int index = (double)filtered4[i];

    output_array[i] = index;
  }



  FFT = arduinoFFT(output_array, vImag, samples, samplingFrequency); /* Create FFT object */
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);	/* Weigh data */
  FFT.Compute(FFT_FORWARD); /* Compute FFT */
  FFT.ComplexToMagnitude(); /* Compute magnitudes */
  PrintVector(output_array, (samples >> 1), SCL_FREQUENCY);
  // double x = FFT.MajorPeak();
  // Serial.println(x, 6);

  while(1); /* Run Once */
  // delay(2000); /* Repeat after delay */
}


void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    Serial.println(vData[i], 4);
  }
  Serial.println();

  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
	      break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
	      break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
	      break;
    }
    Serial.println(abscissa, 6);
    // if(scaleType==SCL_FREQUENCY)
    //   Serial.println("Hz");
  }
  Serial.println();
}
