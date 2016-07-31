CC=g++
CFLAGS=-Wall -O2 -g0
ALTFLAGS=`pkg-config gtkmm-3.0 --cflags --libs`

INC=/home/petel__/LeapSDK/include
LIB=/home/petel__/LeapSDK/lib/x64/libLeap.so -Wl,-rpath,/home/petel__/LeapSDK/lib/x64

IDIR=../inc
SDIR=../src
ODIR=src

IFILES=$(wildcard $(IDIR)/*.h)
SFILES=$(wildcard $(SDIR)/*.cpp)
OFILES=$(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SFILES))

EXEC=LinuxApp


DEPENDS:=$(OFILES:.o=.d)

all: $(EXEC)

$(ODIR)/%.o: $(SDIR)/%.cpp
	@echo '$< -> $@ $(@:.o=.d)'
	@$(CC) -c $< -o $@ $(ALTFLAGS) $(CFLAGS) -I$(INC) $(LIB)
	@$(CC) -M $< > $(@:.o=.d) $(ALTFLAGS) $(CFLAGS) -I$(INC) $(LIB)
	
$(EXEC): $(OFILES)
	@echo 'Linking { $(OFILES) } -> $(EXEC)'
	@$(CC) $(OFILES) -o $@ $(ALTFLAGS) $(CFLAGS) -I$(INC) $(LIB)

-include $(DEPENDS)

clean:
	@rm -f $(ODIR)/*
	@rm -f $(EXEC)
	