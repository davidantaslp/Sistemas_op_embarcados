#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <stdlib.h>
#include <pigpio.h>

int hex2int(char *c);
int checksum_valid(char *string);
int parse_comma_delimited_str(char *string, char **fields, int max_fields);
int debug_print_fields(int numfields, char **fields);
int OpenGPSPort(const char *devname);
int SetTime(char *date, char *time);

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

int SetTime(char *date, char *time){//set time
	struct timespec ts;
	struct tm gpstime;
	time_t secs;
	char tempbuf[2];
	int ret;

	printf("GPS    UTC_Date %s, UTC_Time %s\r\n",date, time);
	// GPS date has format of ddmmyy
	// GPS time has format of hhmmss.ss

	if ((strlen(date) != 6) | (strlen(time) != 9)) {
		printf("No date or time fix. Exiting\r\n");
		return 1;
	}

	// Parse day:
	strncpy(tempbuf, (char *)date, 2);
	tempbuf[2] = '\0';
	gpstime.tm_mday = atoi(tempbuf);

	// Parse month:
	strncpy(tempbuf, (char *)date+2, 2);
	tempbuf[2] = '\0';
	gpstime.tm_mon = atoi(tempbuf) - 1;

	// Parse year:
	strncpy(tempbuf, (char *)date+4, 2);
	tempbuf[2] = '\0';
	gpstime.tm_year = atoi(tempbuf) + 100;

	// Parse hour:
	strncpy(tempbuf, (char *)time, 2);
	tempbuf[2] = '\0';
	gpstime.tm_hour = atoi(tempbuf);

	// Parse minutes:
	strncpy(tempbuf, (char *)time+2, 2);
	tempbuf[2] = '\0';
	gpstime.tm_min = atoi(tempbuf);

	// Parse seconds:
	strncpy(tempbuf, (char *)time+4, 2);
	tempbuf[2] = '\0';
	gpstime.tm_sec = atoi(tempbuf);

	printf("Converted UTC_Date %02d%02d%02d, UTC_Time %02d%02d%02d.00\r\n",gpstime.tm_mday,(gpstime.tm_mon)+1,(gpstime.tm_year)%100, gpstime.tm_hour, gpstime.tm_min, gpstime.tm_sec);

	ts.tv_sec = mktime(&gpstime);
	// Apply GMT offset to correct for timezone
	ts.tv_sec += gpstime.tm_gmtoff;

	printf("Number of seconds since Epoch %ld\r\n",ts.tv_sec);

	ts.tv_nsec = 0;
	ret = clock_settime(CLOCK_REALTIME, &ts);
	if (ret)
		perror("Set Clock");

	//clock_gettime(CLOCK_REALTIME, &ts);
	//printf("Number of seconds since Epoch %ld\r\n",ts.tv_sec);
	//gpstime = gmtime(&ts.tv_sec);
	//printf("System UTC_Date %02d%02d%02d, ",gpstime->tm_mday,(gpstime->tm_mon)+1,(gpstime->tm_year)%100);
	//printf("UTC_Time %02d%02d%02d.00\r\n", gpstime->tm_hour, gpstime->tm_min, gpstime->tm_sec);
	printf("\r\n");
}

int OpenGPSPort(const char *devname){//open gps port
	int fd; // File descriptor for the serial port
	struct termios options;

	if ((fd = open(devname, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {
		perror("Open");
		return 1;
	}

	// Set to blocking
	fcntl(fd, F_SETFL, 0);

	// Get port attributes
	tcgetattr(fd, &options);

	// Set input and output baud rates
	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);

	// Set input modes
	options.c_iflag |= ICRNL;

	// Set 8 bits, no parity, 1 stop bit
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	options.c_lflag &= ~ECHO;
	options.c_lflag |= ICANON;

	// Set port attributes
	tcsetattr(fd, TCSAFLUSH, &options); // Flush input and output buffers

	return(fd);
}
