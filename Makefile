#source file
SRC = command.cpp net_thread.cpp thread.cpp timer_thread.cpp main_thread.cpp protocol.cpp  http_protocol.cpp nsqd_protocol.cpp nsqlookup_protocol.cpp factory.cpp handler.cpp tcp_handler.cpp listen_handler.cpp nsqd_handler.cpp nsqlookup_handler.cpp 

#Application name
APP = nsqclient

#Application type BIN SO LIB
APP_TYPE = LIB 

#Include
INCLUDE = -I./3rd/libevent/lib/include/ -I./3rd/jsoncpp/lib/include/ 
#-I/usr/local/include/json
#Link lib 
LINK_LIB = 

#Install Path
INSTALL_PATH = 

#Bin Path
BIN_PATH = bin

#Obj path
OBJ_PATH = obj

#Compiler
CXX = g++

#Options
CFLAGS = -g  

#Obj and bin directory Judge
ifeq ($(strip $(BIN_PATH)),)
	BIN_PATH = .
endif

ifeq ($(strip $(OBJ_PATH)),)
	OBJ_PATH = .
endif

ifeq ($(strip $(INSTALL_PATH)),)
	INSTALL_PATH = .
endif

#Dest Splice
ifeq "$(strip $(APP_TYPE))" "SO"
	DEST=$(BIN_PATH)/lib$(APP).so
	LINKER = $(CXX) -o $(DEST)
	CFLAGS+= -fPIC -shared
else
	ifeq "$(strip $(APP_TYPE))" "LIB"
		DEST = $(BIN_PATH)/lib$(APP).a
		LINKER = ar crs $(DEST)
	else
		ifeq "$(strip $(APP_TYPE))" "BIN"
			DEST = $(BIN_PATH)/$(APP)
			LINKER = $(CXX)  -o $(DEST) 
		endif
	endif
endif


VPATH += $(foreach n,$(SRC),$(dir $(n)):)
						
VPATH += $(BIN_PATH):$(OBJ_PATH)

#Object file
OBJ = $(addprefix $(OBJ_PATH)/, $(notdir $(SRC:.cpp=.o)))

#Deps file
DEPS = $(addprefix $(OBJ_PATH)/, $(notdir $(SRC:.cpp=.d)))


.PHONY:clean

install_all : all
	@if [ -n "${INSTALL_PATH}" ];then \
		mkdir -p ${INSTALL_PATH} && \
		cp -rf ${DEST}  ${INSTALL_PATH}/; \
	fi


all:init  $(DEST) 

init:
	@mkdir -p $(BIN_PATH)
	@mkdir -p $(OBJ_PATH)

$(DEST):$(OBJ)
	$(LINKER)  $^ $(LINK_LIB)

$(OBJ):$(OBJ_PATH)/%.o:%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) $< -c -o  $@ 											

$(DEPS):$(OBJ_PATH)/%.d:%.cpp
	@mkdir -p $(BIN_PATH);\
	mkdir -p $(OBJ_PATH);\
	set -e;rm -f  $@;\
	$(CXX) $(CFLAGS) -MM $(INCLUDE) $< > $@.$$$$;\
	sed 's, \($*\)\.o[ :]*,\1.o $@ : , g'< $@.$$$$ > $@;\
	rm -f $@.$$$$	   

include $(DEPS)

clean:
	-rm -f $(OBJ_PATH)/*.o  $(OBJ_PATH)/*.d $(DEST) \
	${INSTALL_PATH}/$(notdir $(DEST))
