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
#include <wiringPi.h>
//===============Variaveis
#define BAUDRATE 9600
#define bits 8
#define sensorPIR 0

//https://maps.google.com/?q=<lat>,<lng>
//===============Funcs
int sendSMS(char dataSend){ //envia sms
    printf("Enviando sms...\n\n");
    int fd;
    if(wiringPiSetup() < 0)return 1;
    if((fd = serialOpen("/dev/ttyS0",9600)) < 0)return 1;
    char *number = "559999999999"; //teste
    // serialPrintf(fd, "AT+CSQ\r\n"); //check signal quality
    // sleep(1);  
    // serialPrintf(fd, "AT+CREG?\r\n"); //check network registration
    // sleep(1);
    serialPrintf(fd,"AT+CMGF=1\r\n"); //set text mode
    sleep(1);
    serialPrintf(fd, "AT+CMGS=\"+%s\"\r\n",number); //send SMS to number
    sleep(1);
    serialPrintf(fd,"%c", 13); //send carriage return
    serialPrintf(fd, "Texto\r\n"); //send message
    serialPrintf(fd, "%c", 26);// fim da mensagem ****
        while(serialDataAvail(fd)>0){ //Recebe msg
            printf ("%c", serialGetchar (fd));
            //fflush (stdout) ;
        }
    sleep(1);
    serialClose(fd);
    return 0;
}

int gpsReading(void){
    int fd;
    char buffer[1000];
	int nbytes;
	int i,j = 0;
	char *field[20];
	char *gpgll="$GPGLL";
    gpioSerialRead(18,&buffer,sizeof(buffer));
    printf("buffer serial:%s\n\n",buffer);
    char* result = strstr(buffer,gpgll);
    if(result != NULL){
        printf("Achei:%.50s\n",result);
        i = parse_comma_delimited_str(result, field, 20);
        debug_print_fields(i,field);
        printf("UTC Time  :%s\r\n",field[5]);
        printf("Latitude  :%s\r\n",field[1]);
        printf("Longitude :%s\r\n\n",field[3]);
        //gpioTerminate();
        //return field;
    }
    // gpioTerminate();
    // return NULL;
}


int envioSms(void){ //envio do sms com as info de geolocalizacao
    //Get data from gps
    char buffer[1000];
	int i = 0;
    int j = 0;
    int valiData = 0;
	char *field[20];
	//char *gpgll="$GPGLL";
    char *gpgll="$GPGGA";

    char buffConvert[11];
    char latt[11];
    char longg[11];
    char minutos[2];
    char segundos[6];
    float flatt, flongg;
    //char *number = "5561982185657"; //Amanda
    char *number = "5561995553032"; //Dantas
    
    printf("Iniciando GPS...\n\n");
    while(valiData == 0){
        gpioSerialRead(18,&buffer,sizeof(buffer));
        int fd;
        
        // printf("buffer serial:%s\n\n",buffer);  
        sleep(1);
        char* result = strstr(buffer,gpgll);
        printf("Verificando se a leitura foi correta.\n");
        if(result != NULL){
            printf("Dados válidos, enviando SMS\n");
            // printf("Achei:%.50s\n",result);
            i = parse_comma_delimited_str(result, field, 20);
            debug_print_fields(i,field); //Valores raw, precisa converter as coordenadas
            // printf("UTC Time  :%s\r\n",field[1]);
            // printf("Latitude  :%s\r\n",field[2]);
            // printf("Longitude :%s\r\n",field[4]);

            //Conversão da Latitude
            stpcpy(buffConvert,field[2]);
            //printf("Buffer: %s\r\n",buffConvert);
            for(j = 0; j<2; j++){ //Graus
                //printf("%c",buffConvert[j]);
                latt[j] = buffConvert[j];
            }
            for(j = 2; j<4; j++){ //Minutos
                //printf("%c",buffConvert[j]);
                minutos[j-2] = buffConvert[j];
            }
            for(j = 4; j<10; j++){ //Segundos
                //printf("%c",buffConvert[j]);
                segundos[j-4] = buffConvert[j];
            }
            flatt = atof(latt) + (atof(minutos)/60) + ((atof(segundos)*60)/3600);

            //Conversão da Longitude
            stpcpy(buffConvert,field[4]);
            //printf("Buffer: %s\r\n",buffConvert);
            for(j = 1; j<3; j++){ //Graus
                //printf("%c",buffConvert[j]);
                longg[j-1] = buffConvert[j];
            }
            for(j = 3; j<5; j++){ //Minutos
                //printf("%c",buffConvert[j]);
                minutos[j-3] = buffConvert[j];
            }
            for(j = 5; j<11; j++){ //Segundos
                //printf("%c",buffConvert[j]);
                segundos[j-5] = buffConvert[j];
            }
            flongg = atof(longg) + (atof(minutos)/60) + ((atof(segundos)*60)/3600);
            printf("Latitude em graus:%.6f\n", flatt);
            printf("Longitute em graus:%f\n", flongg);

            
            if(wiringPiSetup() < 0)return 1;
            if((fd = serialOpen("/dev/ttyS0",9600)) < 0)return 1;
            //Envio do sms
            printf("Enviando sms...\n\n");
            serialPrintf(fd,"AT+CMGF=1\r\n"); //set text mode
            sleep(1);
            serialPrintf(fd, "AT+CMGS=\"+%s\"\r\n",number); //send SMS to number
            sleep(1);

            serialPrintf(fd,"%c", 13); //send carriage return
            serialPrintf(fd, "https://maps.google.com/?q=-%.7f,-%.7f\r\n",flatt, flongg); //send message
            printf("https://maps.google.com/?q=-%.7f,-%.7f\r\n",flatt, flongg);
            serialPrintf(fd, "%c", 26);// fim da mensagem ****
            while(serialDataAvail(fd)>0){ //Recebe msg
                printf ("%c", serialGetchar (fd));
                //fflush (stdout) ;
            }
            sleep(1);
            serialClose(fd);
            return 0;
            valiData = 1; 
        }
    }
    return 0;
}

//================Main
int main(){
    gpioInitialise();
    gpioSetMode(18, PI_INPUT);
    gpioSerialReadOpen(18, BAUDRATE, bits);

    wiringPiSetup();
    pinMode(sensorPIR, INPUT);
    printf("Iniciando sistema de segurança...\n");
    int presenca = 1;
    int invasor = 1;
    char *gpsInfo[20];
    while(1){
        //presenca = digitalRead(sensorPIR);
        if (presenca == 0){
            printf("SEM PRESENÇA\n");
        }
        else{
            printf("PRESENÇA\n");
            //Func para reconheciemento facial
            //Verifica se a presença é de um invasor
            if (invasor == 1){

                envioSms();
                sleep(5);

            }
            else{
                printf("Invasor não detectado!\n");
            } 
        }
    }

    return 0;
    gpioTerminate();
} 
