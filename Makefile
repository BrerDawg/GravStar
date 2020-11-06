# A Makefile for both Linux and Windows, 30-dec-2015

#define all executables here
app_name= gravstar


all: ${app_name}


#define compiler options	
CC=g++

ifneq ($(OS),Windows_NT)			#linux?
#	CFLAGS=-g -O0 -fno-inline
#	LIBS=-L/usr/X11/lib -L/usr/local/lib -lfltk_images /usr/local/lib/libfltk.a -lpng -lz -ljpeg -lrt -lm -lXcursor -lXfixes -lXext -lXft -lfontconfig -lXinerama -lpthread -ldl -lX11
#	INCLUDE= -I/usr/local/include
	INCLUDE= `pkg-config --cflags-only-I lilv-0`
	CFLAGS=-g -Wno-deprecated-declarations -Wno-format-security -Wno-int-to-pointer-cast -fpermissive -fno-inline -Dbuild_date="\"`date +%Y-%b-%d`\"" #-Dbuild_date="\"2016-Mar-23\""			#64 bit
	LIBS=-lfltk -lfltk_images -lfltk_gl -lX11 -lpng -lz -ljpeg -lrt -lm -lXcursor -lXfixes -lXext -lXft -lfontconfig -lXinerama -lXrender -lpthread -ldl -lX11 #64 bit
else								#windows?
	CFLAGS=-g -DWIN32 -mms-bitfields -Dcompile_for_windows
	LIBS= -L/usr/local/lib -static -mwindows -lfltk -lfltk_images -lfltk_jpeg -lole32 -luuid -lcomctl32 -lwsock32 -lm
	INCLUDE= -I/usr/local/include
endif

#define object files for each executable, see dependancy list at bottom
obj1= gravstar.o GCProfile.o pref.o GCLed.o GCCol.o
#obj2= backprop.o layer.o



#linker definition
gravstar: $(obj1)
	$(CC) $(CFLAGS) -o $@ $(obj1) $(LIBS)


#linker definition
#backprop: $(obj2)
#	$(CC) $(CFLAGS) -o $@ $(obj2) $(LIBS)



#compile definition for all cpp files to be complied into .o files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

%.o: %.cxx
	$(CC) $(CFLAGS) $(INCLUDE) -c $<



#dependancy list per each .o file
gravstar.o: gravstar.h GCProfile.h pref.h GCCol.h GCLed.h
GCProfile.o: GCProfile.h
pref.o: pref.h GCCol.h GCLed.h
GCCol.o:  GCCol.h
GCLed.o: GCLed.h
#layer.o: layer.h


.PHONY : clean
clean : 
		-rm $(obj1)					#remove obj files
ifneq ($(OS),Windows_NT)
		-rm ${app_name}				#remove linux exec
else
		-rm ${app_name}.exe			#remove windows exec
endif




