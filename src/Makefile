BIN=../bin/oj_server
CC=g++
DEBUG=-g
VERSION=-std=c++11
LINK_LIBRARY=-lpthread -lctemplate -ljsoncpp
CTEMPLATE_PATH=-L /root/workspace/OnlineJudge/src/ThirdPartLibForCpp/el7.x86_64/third_part/lib

.PHONY:$(BIN)
$(BIN):oj_server.cpp
	$(CC) $^ -o $@ $(DEBUG) $(VERSION) $(LINK_LIBRARY)  $(CTEMPLATE_PATH)

.PHONY:clean
clean:
	rm -f $(BIN)

.PHONY:clean_log
clean_log:
	rm -rf ./tmp_files/*
