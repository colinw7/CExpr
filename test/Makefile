CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = .
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CExprTest

SRC = \
CExprTest.cpp

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-std=c++11 \
-I$(INC_DIR) \
-I../../CFile/include \
-I../../CReadLine/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I.

LFLAGS = \
-L$(LIB_DIR) \
-L../../CExpr/lib \
-L../../CFile/lib \
-L../../COS/lib \
-L../../CRegExp/lib \
-L../../CReadLine/lib \
-L../../CStrUtil/lib \

LIBS = \
-lCExpr -lCReadLine -lCFile -lCOS -lCRegExp -lCStrUtil -lreadline -ltre

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CExprTest

.SUFFIXES: .cpp

.cpp.o:
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CExprTest: $(OBJS) $(LIB_DIR)/libCExpr.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CExprTest $(OBJS) $(LFLAGS) $(LIBS)
