#include "thermo.h" 
#include <stdlib.h>
#include <stdio.h>
int set_temp_from_ports(temp_t *temp){
    if(THERMO_SENSOR_PORT < 0 || THERMO_SENSOR_PORT > 28800 || THERMO_STATUS_PORT & ( 1 << 2)){//sensor is below, above, or error state
        temp->tenths_degrees = 0;                      
        temp->temp_mode = 3;
        return 1;
    } 
    int degree = ((THERMO_SENSOR_PORT>>5) - 450);   // sets degree in celsuis
    if((THERMO_SENSOR_PORT & 0b11111) >= 16){       // if remainder greater than 16, round up
        degree++;
    }
    if(THERMO_STATUS_PORT & (1 << 5)){                   //checks if in fahrenheit mode 
       temp->tenths_degrees = degree * 9/5 + 320;          // convert to fahrenheit and sets variable
       temp->temp_mode = 2;                                 // changes mode
    }
    else{
        temp->tenths_degrees = degree;                      // sets variable in celsius
        temp->temp_mode = 1;					// mode is in celsuis
    }
    return 0;
}
int set_display_from_temp(temp_t temp, int *display){
    *display = 0;                        
    int min_temp = -450;               // min temp in celsuis 
    int max_temp = 450;                // max temp in c
    if(temp.temp_mode == 2){
        min_temp = min_temp * 9/5 + 320;        // min temp in f
        max_temp = max_temp * 9/5 + 320;       // max temp in f
    }
   if( temp.tenths_degrees < min_temp || temp.tenths_degrees > max_temp){ //sets display to error if above or below  max temp
        *display = *display | (0b000110111101111110111110000000);
        return 1;
   }
   if(temp.temp_mode != 1 && temp.temp_mode != 2){              // if tempmode is not 1 or 2, display error
        *display = *display | (0b000110111101111110111110000000);
        return 1;
   }
    int arr[10] = {
        0b1111011,                    // 0
        0b1001000,                    // 1
        0b0111101,                    // 2
        0b1101101,                    // 3
        0b1001110,                    // 4
        0b1100111,                    // 5
        0b1110111,                    // 6
        0b1001001,                    // 7
        0b1111111,                    // 8
        0b1101111,                    // 9
    };
    int temp_hundreds = temp.tenths_degrees / 1000;             // gets hundreds place
    int temp_tens = (temp.tenths_degrees / 100) % 10;          // gets tens place
    if(temp_tens < 0){                                           
        temp_tens *= -1;                                //makes tens positive if negative
    }
    int temp_ones = (temp.tenths_degrees % 100)/10;    // gets one place
     if(temp_ones < 0){
        temp_ones *= -1;                               //makes ones positive if negative
    }
    int temp_tenths = (temp.tenths_degrees % 100) % 10;  //gets tenths place
     if(temp_tenths < 0){                        
        temp_tenths *= -1;                      // makes tenth place positive
    }
    int temp_mode;
    if(temp.temp_mode == 1){              // displays c 
        temp_mode = (1 << 28); 
    }
    else{
        temp_mode = (1 << 29);            // displays f
    }
    if(temp_hundreds != 0 ){
           temp_hundreds = *display | (arr[temp_hundreds] << 21);      // displays hundred place
     }
    if(temp_tens != 0 || temp_hundreds != 0){                       // displays tens place 
          temp_tens = *display | (arr[temp_tens] << 14);
    }
    temp_ones = *display | arr[temp_ones] << 7;                     //display ones 
    temp_tenths = *display | (arr[temp_tenths] << 0);              // display tenths
    if(temp.tenths_degrees < 0 && temp_tens == 0 && temp_hundreds == 0){   //displays negative right after ones place
         *display = *display | (1 << 16);
    }
    else if(temp.tenths_degrees < 0 && temp_tens != 0 && temp_hundreds == 0){       // displays negative after tens place
         *display = *display | (1 << 23);
    }
    *display = *display | temp_mode | temp_hundreds| temp_tenths | temp_tens | temp_ones;   // create final display
    return 0;
}
int thermo_update(){
    temp_t temp;                                                 
    int x = set_temp_from_ports(&temp);        //callsettemp
    int y = set_display_from_temp(temp,&THERMO_DISPLAY_PORT);  //call displayfromtemp
    if( y!= 0 || x != 0){     // if one of the methods fail 
        return 1;
    }
    return 0;
}

