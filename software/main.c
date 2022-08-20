/*Script principal de funcionamento do projeto de Sistemas Operacionais Embarcados
Integrantes: Amanda Heriques/mat e Davi Dantas/170031934
Detecção de presenca
Reconhecimento de facial
Aquisição de coordenadas
Envio de informação por sms 
*/

//================Libs
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <time.h>
#include <pigpio.h>

//===============Funcs
int sendSMS(){ //envia sms
    printf("Enviando sms...\n\n");
    int fd;
    if(wiringPiSetup() < 0)return 1;
    if((fd = serialOpen("/dev/ttyS0",9600)) < 0)return 1;
    char *number = "556182185657";
    serialPrintf(fd,"AT+CMGF=0\r\n");
    sleep(1);
    while(serialDataAvail(fd)>0){ //Recebe msg
        printf ("%c", serialGetchar (fd));
        //fflush (stdout) ;
    }
    //serialPrintf(fd,"AT\r\n"); //send AT command
    serialPrintf(fd, "AT+CSQ\r\n"); //check signal quality
    sleep(1);
    serialPrintf(fd, "AT+CREG?\r\n"); //check network registration
    sleep(1);
    serialPrintf(fd,"AT+CMGF=1\r\n"); //set text mode
    sleep(1);
    serialPrintf(fd, "AT+CMGS=+%s\r\n",number); //send SMS to number
    sleep(1);
    serialPrintf(fd,"%d", 13); //send carriage return
    serialPrintf(fd, "Mensagem!!\r\n"); //send message
    serialPrintf(fd, "%d", 26);// fim da mensagem ****
    sleep(1);
    serialClose(fd);
    return 0;
}

int gpsReading(void){

    
}

//================Main
int main(){
    printf("Iniciando...\n");
    while(1){
        gpsReading();
        //printf("SMS\n");
        sendSMS();
    }
}