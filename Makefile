CC = gcc


LIB_LIB     = $(shell pkg-config --libs gthread-2.0 gobject-2.0 json-glib-1.0 gstreamer-1.0  )
LIB_INCLUDE = $(shell pkg-config --cflags glib-2.0 json-glib-1.0 gstreamer-1.0)

# C++ 컴파일러 옵션
CXXFLAGS = -Wall -O2
 
# 링커 옵션
LDFLAGS = $(LIB_LIB) -lm -lpthread -lcurl  -g

# 헤더파일 경로
INCLUDE = -Iinclude/  . $(LIB_INCLUDE)

# 소스 파일 디렉토리
SRC_DIR = .

# 오브젝트 파일 디렉토리
OBJ_DIR = ./obj

# 생성하고자 하는 실행 파일 이름
TARGET = main

# Make 할 소스 파일들
# wildcard 로 SRC_DIR 에서 *.c 로 된 파일들 목록을 뽑아낸 뒤에
# notdir 로 파일 이름만 뽑아낸다.
# (e.g SRCS 는 foo.c bar.c main.c 가 된다.)
SRCS = $(notdir $(wildcard $(SRC_DIR)/*.c))

$(info  소스파일 : $(SRCS)) 


OBJS = $(SRCS:.c=.o)

$(info  OBJS : $(OBJS)) 


# OBJS 안의 object 파일들 이름 앞에 $(OBJ_DIR)/ 을 붙인다.
OBJECTS = $(patsubst %.o,$(OBJ_DIR)/%.o,$(OBJS))
DEPS = $(OBJECTS:.o=.d)

all: main

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CXXFLAGS) $(INCLUDE)  -c $< -o $@ -MD $(LDFLAGS)

ALL_SRC = $(SRC_DIR)/%.c
$(info  ALL_SRC : $(ALL_SRC)) 

$(TARGET) : $(OBJECTS)
	$(CC) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

.PHONY: clean all
clean:
	rm -f $(OBJECTS) $(DEPS) $(TARGET)

-include $(DEPS)