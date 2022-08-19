int debug_print_fields(int numfields, char **fields);
int hexchar2int(char c);
int hex2int(char c);
int checksum_valid(char *string);
int parse_comma_delimited_str(char *string, char **fields, int max_fields);
int SetTime(char *date, char *time);
int OpenGPS(const char *devname);