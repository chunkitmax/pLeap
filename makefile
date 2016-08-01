FLAGS=-Wall -O2 -g0
LDFLAGS=$(LEAP_PATH)/LeapSDK/lib/x64
ALTFLAGS=`pkg-config gtkmm-3.0 --cflags --libs`

# $(LEAP_PATH): where LeapSDK folder are located

SHAREDLIB=$(LEAP_PATH)/LeapSDK/lib/x64/libLeap.so
LIBS=X11 Xtst
LIBPATH=/usr/X11/lib

INCDIR=inc/Leap /usr/include inc
SRCDIR=src
OBJDIR=obj
BINDIR=bin

EXEC=LinuxApp

####################################################################

$(shell mkdir -p $(OBJDIR))
$(shell mkdir -p $(BINDIR))

LIBRARIES=

INCFILES=$(foreach d, $(INCDIR), -I$d)
SRCFILES=$(wildcard $(SRCDIR)/*.cpp)
OBJFILES=$(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCFILES))

LIBRARIES+=$(foreach d, $(LIBS), -l$d)
LIBRARIES+=$(foreach d, $(LIBPATH), -L$d)
LIBRARIES+=$(foreach d, $(LDFLAGS), -Wl,-rpath,$d)
LIBRARIES+=$(SHAREDLIB)

DEPENDS:=$(OBJFILES:.o=.d)

all: $(BINDIR)/$(EXEC)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo '$< -> $@ $(@:.o=.d)'
	@$(CXX) -c $< -o $@ $(ALTFLAGS) $(FLAGS) $(INCFILES) $(LIBRARIES)
	@$(CXX) -M $< > $(@:.o=.d) $(ALTFLAGS) $(FLAGS) $(INCFILES) $(LIBRARIES)
	
$(BINDIR)/$(EXEC): $(OBJFILES)
	@echo 'Linking { $(OBJFILES) } -> $(EXEC)'
	@$(CXX) $(OBJFILES) -o $@ $(ALTFLAGS) $(FLAGS) $(INCFILES) $(LIBRARIES)
	
-include $(DEPENDS)

clean:
	@rm -f $(OBJDIR)/*
	@rmdir $(OBJDIR)
	@rm -f $(BINDIR)/*
	@rmdir $(BINDIR)
	