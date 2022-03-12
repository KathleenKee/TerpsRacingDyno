# TerpsRacingDyno
Use:
Each script is independent of eachother
Upload the desired script to an arduino board to read from a dyno

Hardware:
perf board with Arduino Nano Every, a mirco SD card Adapter, and an analog to digital chip XFW-HX711
The HX711 chip connects to a load cell
A hall efect sensor is connected through 5V power, ground, and digital pin 7

Load Cell:
Current calibration factor is load cell specific to lbs, to find your calibration there's code from sparkfun:
https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide/all 
General process is to add weights to the load cell and read the output, it works linearly so a single factor can scale the line from raw numbers to your preferred unit
