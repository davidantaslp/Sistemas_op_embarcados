#include <stdio.h>
#include <wiringPi.h>

#define sensorPIR 0


int main(void) {
    wiringPiSetup();
    pinMode(sensorPIR, INPUT);

    while(1){
        if (digitalRead(sensorPIR) == 0){
            printf("SEM PRESENÇA\n");
        }
        else{
            printf("PRESENÇA\n");
        }
    }
    return 0;
}
