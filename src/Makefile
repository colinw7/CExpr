CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib

all: $(LIB_DIR)/libCExpr.a

SRC = \
CExprBValue.cpp \
CExprCompile.cpp \
CExpr.cpp \
CExprExecute.cpp \
CExprFunction.cpp \
CExprInterp.cpp \
CExprIValue.cpp \
CExprOperator.cpp \
CExprParse.cpp \
CExprRValue.cpp \
CExprStrgen.cpp \
CExprSValue.cpp \
CExprToken.cpp \
CExprTokenStack.cpp \
CExprValue.cpp \
CExprVariable.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-std=c++11 \
-I$(INC_DIR) \
-I../../CFile/include \
-I../../CMath/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I../../COS/include \
-I.

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCExpr.a

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(LIB_DIR)/libCExpr.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCExpr.a $(OBJS)
