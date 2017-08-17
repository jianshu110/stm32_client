###########################################  
#Makefile for stm32_client
###########################################  
INC=  
LIB=-lmicrohttpd -lcurl -pthread -lm


CC=gcc  
CC_FLAG=-Wall  
  
PRG=stm32_client_bin

OBJ+=file_process.o json_data.o heartbeat.o
OBJ+=i2c_adaptor.o cjson.o common.o
OBJ+=stm32_client.o
  
$(PRG):$(OBJ)  
	$(CC) $(INC) $(LIB) -o $@ $(OBJ)  
      
.SUFFIXES: .c .o .cpp  
.cpp.o:  
	$(CC) $(CC_FLAG) $(INC) -c $*.cpp -o $*.o  
  
.PRONY:clean  
clean:  
	@echo "Removing linked and compiled files......"  
	rm -f $(OBJ) $(PRG)
