# arduino-fft
Proof of concept for FFT Magnetometer data processing as a part of UAlberta's RockSat team. 


# Arduino Code
Plug Function waveform into port A3 of Arduino Mega board, Ground to GND. 
Can view FFT results in Serial port at 115200 baud. 

Prints all Amplitude data first, then all of the corresponding frequency datapoints. 

If adjustments are required, tinker with variables:
- samples
- signalFrequency *
- samplingFrequency **
- amplitude


# ESP32 Code
Follow setup tutorial here: https://randomnerdtutorials.com/installing-esp32-arduino-ide-2-0/
Use the ESP32-WROOM-DA board module.

Plug Function waveform into port P34 board, Ground to GND. 
Can view FFT results in Serial port at 115200 baud. 

Prints all Amplitude data first, then all of the corresponding frequency datapoints. 

If adjustments are required, tinker with variables:
- samples
- signalFrequency *
- samplingFrequency **
- amplitude
