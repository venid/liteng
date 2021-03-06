#компилятор g++
#имя исполняемого файла
FNAME= $(shell basename `pwd`)
#имя отладочного исполняемого файла
FNAME_DBG= $(FNAME)_dbg
#директории проекта
PATHS= util core lua mod glext res comp base inter math sys
#путь к директориям проекта
VPATH= $(addprefix src/, $(PATHS))
#пути включения заголовочных файлов
CPATH= $(addprefix -I, $(VPATH) src)
#подключаемые библиотеки
LIB= -lrt -llua5.2 -lpthread -lX11 -lGL -lpng12 -lz -lm -lmeta
#директория статических библиотек
LPATH= -Lsrc/lib
#файлы кода
OBJFILE= $(notdir $(patsubst %.cpp, %.o, $(wildcard $(addsuffix /*cpp, $(VPATH)))))
#директории для объектных файлов
R_DIR= bin/release/
D_DIR= bin/debug/
#файлы для линковки
R_FILE= $(addprefix $(R_DIR), $(OBJFILE))
D_FILE= $(addprefix $(D_DIR), $(OBJFILE))
#имя выходного файла
TARGET=$< -o $@
#файл версии
FVERSION= $(shell find . -type f -name main.cpp)


all: $(FNAME)
all: CFLAGS= -c -MMD -Wall -std=c++11 -DGLM_FORCE_RADIANS $(EXT)

$(FNAME): $(R_FILE)
	$(CXX) -Wl,--no-as-needed -o $@ $(LPATH) $(R_FILE) $(LIB)
	lua .build $(FVERSION)

$(R_DIR)%.o: %.cpp
	$(CXX) $(CFLAGS) $(CPATH) $(TARGET)

# ----------------------------------------------------------------------------------------------------

dbg: $(FNAME_DBG)
dbg: CFLAGS= -c -O0 -g -MD -Weffc++ -std=c++11 -DGLM_FORCE_RADIANS -DDEBUG $(EXT)

$(FNAME_DBG): $(D_FILE)
	$(CXX) -Wl,--no-as-needed -o $@ $(LPATH) $(D_FILE) $(LIB)

$(D_DIR)%.o: %.cpp
	$(CXX) $(CFLAGS) $(CPATH) $(TARGET)

# -----------------------------------------------------------------------------------------------------

#загружаем файлы с зависимостями
include $(wildcard $(R_DIR)*.d)
include $(wildcard $(D_DIR)*.d)

.PHONY: all dbg clean check patch deploy

# -----------------------------------------------------------------------------------------------------

clean:
	rm -rf $(R_DIR)*.o $(R_DIR)*.d $(D_DIR)*.o $(D_DIR)*.d $(FNAME) $(FNAME_DBG)

check:
	cppcheck -j4 -q --enable=performance,warning,style $(VPATH)/ 2> err.txt

deploy:
	mkdir bin
	mkdir bin/release bin/debug

