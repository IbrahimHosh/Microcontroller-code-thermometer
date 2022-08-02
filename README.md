# Microcontroller-code-thermometer
Code runs a microcontroller in a digital thermometer. The hardware has the following relevant features.  A temperature sensor whose value can be accessed via a memory mapped port. This is presented as a global variable. Another port indicates whether the temperature should be displayed in Celsius or Fahrenheit. A digital with a port control port; setting certain global variable will change the temperature display with User code that display based on the temperature sensor.
Compile program using MakeFile and use ./thermo_main 10 C to set the global variables. 
thermo_update.c is where most of the important code is, look there.
