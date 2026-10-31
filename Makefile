#---------------------------------------------------------------
#             CONFIGURE THESE VARIABLES IF NEEDED
#---------------------------------------------------------------

# CDK21 and RTS7 are installed under $(ROOT)/usr (ROOT is empty in the
# Docker image, where they live in /usr).
ROOT =
CDK_INC_DIR = $(ROOT)/usr/include
CDK_LIB_DIR = $(ROOT)/usr/lib
CDK_BIN_DIR = $(ROOT)/usr/bin

LANGUAGE=at

#---------------------------------------------------------------
# PROBABLY, THERE'S NO NEED TO CHANGE ANYTHING BEYOND THIS POINT
#---------------------------------------------------------------

L_NAME=$(LANGUAGE)_scanner
Y_NAME=$(LANGUAGE)_parser

LFLAGS   =
YFLAGS   = -dtv
CXXFLAGS = -std=c++23 -DYYDEBUG=1 -pedantic -Wall -Wextra -ggdb -I. -I$(CDK_INC_DIR) -Wno-unused-parameter
LDFLAGS  = -L$(CDK_LIB_DIR) -lcdk
COMPILER = $(LANGUAGE)

CDK  = $(CDK_BIN_DIR)/cdk
LEX  = flex
YACC = bison

SRC_CPP = $(wildcard targets/*.cpp) $(filter-out ./$(L_NAME).cpp, $(wildcard ./*.cpp))
OFILES  = $(SRC_CPP:%.cpp=%.o)
AUTO    = .auto/all_nodes.h .auto/visitor_decls.h

#---------------------------------------------------------------
#                DO NOT CHANGE AFTER THIS LINE
#---------------------------------------------------------------

all: $(AUTO) $(COMPILER)

%.tab.o:: %.tab.c
	$(CXX) $(CXXFLAGS) -c $< -o $@ -Wno-class-memaccess

%.o:: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o:: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(L_NAME).cpp: $(L_NAME).l
	$(LEX) $(LFLAGS) $<

# one bison run makes both files (a grouped target, so "make -j" runs it once)
$(Y_NAME).tab.c $(Y_NAME).tab.h &: $(Y_NAME).y $(AUTO)
	$(YACC) $(YFLAGS) -b $(Y_NAME) $<

# the scanner and the code generator need the parser's token numbers
$(L_NAME).o: $(L_NAME).cpp $(Y_NAME).tab.h
targets/postfix_writer.o: $(Y_NAME).tab.h
$(OFILES): $(AUTO)

.auto/all_nodes.h: $(wildcard ast/*.h)
	mkdir -p .auto
	$(CDK) ast --decls ast --language $(LANGUAGE) > $@

.auto/visitor_decls.h: $(wildcard ast/*.h)
	mkdir -p .auto
	$(CDK) ast --decls target --language $(LANGUAGE) > $@

$(COMPILER): $(L_NAME).o $(Y_NAME).tab.o $(OFILES)
	$(CXX) -o $@ $^ $(LDFLAGS)

check: all
	tests/run.sh

clean:
	$(RM) $(AUTO) *.tab.[ch] *.o $(OFILES) $(L_NAME).cpp $(Y_NAME).output $(COMPILER)
	$(RM) -r tests/out

.PHONY: all check clean

#---------------------------------------------------------------
#                           THE END
#---------------------------------------------------------------
