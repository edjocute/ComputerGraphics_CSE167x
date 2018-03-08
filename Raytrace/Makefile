CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
CFLAGS = -g -DGL_GLEXT_PROTOTYPES -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED -DOSX -Wno-deprecated-register -Wno-deprecated-declarations -Wno-shift-op-parentheses
#INCFLAGS = -I./glm-0.9.7.1 -I/usr/X11/include -I./include/
INCFLAGS = -I/usr/X11/include -I./include/
LDFLAGS = -framework GLUT -framework OpenGL -L./lib/mac/ \
		-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
		-lGL -lGLU -lm -lstdc++ -lfreeimage
else
#CFLAGS = -g -DGL_GLEXT_PROTOTYPES -O3
CFLAGS = -g -std=c++11 -O3 -fopenmp
INCFLAGS = -I./include/ -I/usr/X11R6/include -I/sw/include \
		-I/usr/sww/include -I/usr/sww/pkg/Mesa/include
LDFLAGS = -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib -L./lib/nix/ \
		-L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lfreeimage
#-lGLEW -lglut -lGLU -lGL -lX11 
endif

RM = /bin/rm -f 
all: transforms
transforms: main.o readfile.o Transform.o scene.o
	$(CC) $(CFLAGS) -o transforms main.o Transform.o readfile.o scene.o $(INCFLAGS) $(LDFLAGS) 
main.o: main.cpp Transform.h variables.h scene.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c main.cpp
readfile.o: readfile.cpp readfile.h variables.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c readfile.cpp
Transform.o: Transform.cpp Transform.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c Transform.cpp
scene.o: scene.cpp scene.h variables.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c scene.cpp

clean: 
	$(RM) *.o transforms *.png


 
