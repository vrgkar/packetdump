
BIN = Sweetfish

SRCDIR = src
INCDIR = include
OBJDIR = build
BINDIR = $(OBJDIR)/bin
DEPDIR = $(OBJDIR)/deps

vpath % $(BINDIR)
vpath %.cpp $(SRCDIR)
vpath %.o $(OBJDIR)
vpath %.d $(DEPDIR)

DSRC := $(wildcard $(SRCDIR)/*.cpp)
DBIN = $(BINDIR)/$(BIN)

SRC := $(notdir $(DSRC))
OBJ := $(SRC:.cpp=.o)
DEP := $(SRC:.cpp=.d)

DOBJ := $(addprefix $(OBJDIR)/, $(OBJ))
DDEP := $(addprefix $(DEPDIR)/, $(DEP))

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
COMPILE.cxx = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(TARGET_ARCH) -c
CXXFLAGS += -std=c++23

$(DBIN) : $(DOBJ) | $(BINDIR) $(INCDIR)
	$(CXX) $^ -I $(INCDIR) -o $@
	
# $(OBJDIR)/%.o : %.cpp | $(OBJDIR) 
#	$(COMPILE.c) -I $(INCDIR) -c $< -o $@

$(OBJDIR)/%.o : %.cpp %.d | $(OBJDIR)
	$(COMPILE.cxx) -I $(INCDIR) -c $< -o $@

-include $(wildcard $(DDEP))

$(BINDIR): | $(OBJDIR)
	mkdir $(BINDIR)
$(DEPDIR): | $(OBJDIR)
	mkdir $(DEPDIR)
$(OBJDIR): 
	mkdir $(OBJDIR)
$(INCDIR):
	mkdir $(INCDIR)

clean:
	-rm -f $(DBIN)
	-rm -f $(DOBJ)
	-rm -f $(DDEP)
run:
	./$(DBIN)


