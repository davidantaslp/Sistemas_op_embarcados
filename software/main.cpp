/*Script principal de funcionamento do projeto de Sistemas Operacionais Embarcados
Integrantes: Amanda Heriques/mat e Davi Dantas/170031934
Detecção de presenca
Reconhecimento de facial
Aquisição de coordenadas
Envio de informação por sms 
*/
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <opencv2/videoio.hpp>
#include <raspicam/raspicam_cv.h>

#include <iostream>
#include <ctime>
#include <fstream>
#include <map>
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
#include <wiringSerial.h>

// Defines
#define BAUDRATE 9600
#define bits 8
#define ledAct 4
//define sensorPIR 5

using namespace cv;
using namespace cv::face;
using namespace std;

//GPS Variables
int hex2int(char *c);
int checksum_valid(char *string);
int parse_comma_delimited_str(char *string, char **fields, int max_fields);
int debug_print_fields(int numfields, char **fields);
int OpenGPSPort(const char *devname);
int SetTime(char *date, char *time);
//GPS funcs
int debug_print_fields(int numfields, char **fields){//debug print fields
	//printf("Parsed %d fields\r\n",numfields);

	for (int i = 0; i <= numfields; i++) {
		// printf("Field %02d: [%s]\r\n",i,fields[i]);
	}
}
int hexchar2int(char c){//convert hex char to int
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return -1;
}
int hex2int(char *c){//convert hex string to int
    int i, j, k;
    int n = 0;
    for (i = 0; i < strlen(c); i++) {
        j = hexchar2int(c[i]);
        if (j == -1)
            return -1;
        k = hexchar2int(c[i + 1]);
        if (k == -1)
            return -1;
        n = n * 16 + j * 16 + k;
        i++;
    }
    return n;
	int value;

	value = hexchar2int(c[0]);
	value = value << 4;
	value += hexchar2int(c[1]);

	return value;
}
int checksum_valid(char *string){//check checksum
	char *checksum_str;
	int checksum;
	unsigned char calculated_checksum = 0;

	// Checksum is postcede by *
	checksum_str = strchr(string, '*');
	if (checksum_str != NULL){
		// Remove checksum from string
		*checksum_str = '\0';
		// Calculate checksum, starting after $ (i = 1)
		for (int i = 1; i < strlen(string); i++) {
			calculated_checksum = calculated_checksum ^ string[i];
		}
		checksum = hex2int((char *)checksum_str+1);
		//printf("Checksum Str [%s], Checksum %02X, Calculated Checksum %02X\r\n",(char *)checksum_str+1, checksum, calculated_checksum);
		if (checksum == calculated_checksum) {
			//printf("Checksum OK");
			return 1;
		}
	} else {
		//printf("Error: Checksum missing or NULL NMEA message\r\n");
		return 0;
	}
	return 0;
}
int parse_comma_delimited_str(char *string, char **fields, int max_fields){//parse comma delimited string
	int i = 0;
	fields[i++] = string;

	while ((i < max_fields) && NULL != (string = strchr(string, ','))) {
		*string = '\0';
		fields[i++] = ++string;
	}

	return --i;
}

//Envio do sms
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
        //valiData = 1;
        if(result != NULL){
            printf("Dados válidos\n");
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
            printf("Aguardando GPS Sync...\n");
            //Verifica se os valores não são nulos para enviar o SMS
            if(flatt > 0 && flongg > 0){
                printf("GPS Sincronizado!\tEnviando SMS...\n");
                if(wiringPiSetup() < 0)return 1;
                if((fd = serialOpen("/dev/ttyS0",9600)) < 0)return 1;
                //Envio do sms
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
    }
    return 0;
}

int main(){
    gpioInitialise();
    gpioSetMode(18, PI_INPUT);
    gpioSerialReadOpen(18, BAUDRATE, bits);

    wiringPiSetup();
    //pinMode(sensorPIR, INPUT);
    pinMode(ledAct, OUTPUT);
    digitalWrite(ledAct, HIGH);
    sleep(0.1);
    digitalWrite(ledAct, LOW);
    printf("Iniciando sistema de segurança...\n");

    int sensorPIR = 1;
    //VideoCapture cap(0);
    //Camera = cap(0);
    VideoCapture Camera; // open the default camera
    //Camera.open(0);
    //raspicam::RaspiCam_Cv Camera;

    // Camera.set(CAP_PROP_FRAME_WIDTH, 320);
    // Camera.set(CAP_PROP_FRAME_HEIGHT, 240);

    //Camera.set( CV_CAP_PROP_FORMAT, CV_8UC1 );

    map<int, string> labels;

    ifstream infile("./recognizer/labels.txt");

    int a;
    int unknowTrue=0;
    string b;
    while (infile >> a >> b){
        labels[a] = b;
    }

    if (!Camera.open(0)) {cerr<<"Error opening the camera"<<endl;return -1;}

    CascadeClassifier classifier;
    classifier.load("./cascades/lbpcascade_frontalface.xml");

    Ptr<LBPHFaceRecognizer> recognizer =  LBPHFaceRecognizer::create(2, 2, 7, 7, 17);
    recognizer->read("./recognizer/embeddings.xml");

    Mat windowFrame;
    namedWindow("edges", 1);
    int numframes = 0;
    time_t timer_begin,timer_end;
    time ( &timer_begin );

    for(;;){
        digitalWrite(ledAct, LOW);
        // leitura do sensor de prensença
        if(sensorPIR == 0){
        continue;
        }
        Mat frame;
        Camera.grab();
        digitalWrite(ledAct, HIGH);
        Camera.retrieve(frame);
        cvtColor(frame, windowFrame, cv::COLOR_RGB2GRAY); // convert to grayscale
        vector<Rect> faces;
        classifier.detectMultiScale(frame, faces, 1.2, 3); //parametros de deteccao: imagem, vetor de faces, escala, vizinhos
        for(size_t i = 0; i < faces.size(); i++){
        rectangle(frame, faces[i], Scalar(0, 255, 0));
        Mat face = windowFrame(faces[i]);
        double confidence = 0.0;
        int predicted = recognizer->predict(face); //armazena a variavel de predicao
        recognizer->predict(face, predicted, confidence);
        if(labels.find(predicted) == labels.end() || confidence > 18){ // Verifica se o rosto foi reconhecido
            putText(frame, "Unknown", Point(faces[i].x ,faces[i].y - 5), FONT_HERSHEY_DUPLEX, 1, Scalar(0,255,0), 1);
            //Invasor identificado
            unknowTrue++;
            digitalWrite(ledAct, LOW);
        }else{
            putText(frame, labels[predicted], Point(faces[i].x ,faces[i].y - 5), FONT_HERSHEY_DUPLEX, 1, Scalar(0,255,0), 1);
            unknowTrue--;
            digitalWrite(ledAct, LOW);
            if(unknowTrue <= 0){
                unknowTrue = 0;
            }
        }
        digitalWrite(ledAct, LOW);
        cout << "ID: " << predicted << " | Confidence: " << confidence << endl;
        }
        //envio
        if(unknowTrue>= 30){ //Verifica se teve 30 frames com invasor e envia o sms
            digitalWrite(ledAct, HIGH);
            envioSms();
            printf("Aguardando 60 segundos...\n");
            for(int i = 60; i != 0; i--){
                printf("Verificando novamente em: %d segundos.\n",i);
                sleep(1);
            }
            unknowTrue = 0;
        }
        //imshow("edges", frame); //exibe os frames da câmera
        numframes++;

        if(waitKey(30) >=0) break;
    }
    Camera.release();
    time ( &timer_end );
    double secondsElapsed = difftime ( timer_end,timer_begin );
    cout << "FPS:" << numframes / secondsElapsed << endl;
    return 0;
}
