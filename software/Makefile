C_FLAGS=-lwiringPi -lpigpio
LFLAGS=-L/usr/local/lib/
CFLAGS=-I/usr/local/include/
CV_FLAGS= -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_objdetect -lopencv_photo -lopencv_video -lopencv_videoio -lopencv_videostab -lopencv_face `pkg-config --cflags --libs opencv` -lwiringPi -lpigpio

soe : main.o gps.o
	gcc $(CFLAGS) -o soe main.o gps.o $(LFLAGS) $(C_FLAGS)

soecpp : main.cpp
	g++ $(CFLAGS) -o soecpp main.cpp $(LFLAGS) $(C_FLAGS) $(CV_FLAGS)

# maincpp.o: main.cpp
# 	g++ $(CFLAGS) -c main.cpp $(LFLAGS) $(C_FLAGS) $(CV_FLAGS)
# main.o: main.c
# 	gcc $(CFLAGS) -c main.c $(C_FLAGS)
# gps.o: gps.c gps.h
# 	gcc $(CFLAGS) -c gps.c $(C_FLAGS)


clean:
	rm -f *. soe
