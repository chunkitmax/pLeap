CC=g++
CFLAGS=-Wall -O2 -g0
ALTFLAGS=`pkg-config gtkmm-3.0 --cflags --libs`

# where LeapSDK folder are located
LIB=$(LEAP_PATH)/LeapSDK/lib/x64/libLeap.so -Wl,-rpath,$(LEAP_PATH)/LeapSDK/lib/x64

IDIR=../inc/Leap /usr/include/X11/ ../inc
SDIR=../src
ODIR=src

INC=$(foreach d, $(IDIR), -I$d)
SFILES=$(wildcard $(SDIR)/*.cpp)
OFILES=$(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SFILES))


EXEC=LinuxApp


DEPENDS:=$(OFILES:.o=.d)

all: $(EXEC)

$(ODIR)/%.o: $(SDIR)/%.cpp
	@echo '$< -> $@ $(@:.o=.d)'
	@$(CC) -c $< -o $@ $(ALTFLAGS) $(CFLAGS) $(INC) $(LIB)
	@$(CC) -M $< > $(@:.o=.d) $(ALTFLAGS) $(CFLAGS) $(INC) $(LIB)
	
$(EXEC): $(OFILES)
	@echo 'Linking { $(OFILES) } -> $(EXEC)'
	@$(CC) $(OFILES) -o $@ $(ALTFLAGS) $(CFLAGS) $(INC) $(LIB)

-include $(DEPENDS)

clean:
	@rm -f $(ODIR)/*
	@rm -f $(EXEC)
	