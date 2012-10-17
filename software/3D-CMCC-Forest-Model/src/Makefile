# Makefile for a "3D-CMCC Model" program
# all:		compile sources and create the "3D-CMCC Model" executable
# clean:	delete all object files and the executable
# install:	copy executable into BINDIR
# uninstall:	delete executable from BINDIR

NAME=3D-CMCCModel
SRC_DIR=.
OBJ_DIR=../obj/src
EXE_DIR=../bin
CC=gcc
CXX=g++
#FLAGS=-Wall -g -O3 -DDEBUG -march=native -lpthread
FLAGS=-Wall -g
BINDIR=.

all: $(NAME)

clean:
	rm -f $(EXE_DIR)/$(NAME)
	rm -f $(OBJ_DIR)/G-Function.o
	rm -f $(OBJ_DIR)/Light_dominance.o
	rm -f $(OBJ_DIR)/allometry.o
	rm -f $(OBJ_DIR)/canopy_cover.o
	rm -f $(OBJ_DIR)/crowding-competition.o
	rm -f $(OBJ_DIR)/dataset.o
	rm -f $(OBJ_DIR)/establishment.o
	rm -f $(OBJ_DIR)/light.o
	rm -f $(OBJ_DIR)/log.o
	rm -f $(OBJ_DIR)/main.o
	rm -f $(OBJ_DIR)/management.o
	rm -f $(OBJ_DIR)/matrix.o
	rm -f $(OBJ_DIR)/modifiers.o
	rm -f $(OBJ_DIR)/mortality.o
	rm -f $(OBJ_DIR)/partitioning-allocation.o
	rm -f $(OBJ_DIR)/treemodel.o
	rm -f $(OBJ_DIR)/common.o

install: $(NAME)
	cp $(EXE_DIR)/$(NAME) $(BINDIR)

uninstall:
	rm -f $(BINDIR)/$(NAME)


$(NAME): G-Function.o Light_dominance.o allometry.o canopy_cover.o crowding-competition.o dataset.o establishment.o light.o log.o main.o management.o matrix.o modifiers.o mortality.o partitioning-allocation.o treemodel.o common.o
	$(CXX) -o $(EXE_DIR)/$(NAME) $(OBJ_DIR)/G-Function.o $(OBJ_DIR)/Light_dominance.o $(OBJ_DIR)/allometry.o $(OBJ_DIR)/canopy_cover.o $(OBJ_DIR)/crowding-competition.o $(OBJ_DIR)/dataset.o $(OBJ_DIR)/establishment.o $(OBJ_DIR)/light.o $(OBJ_DIR)/log.o $(OBJ_DIR)/main.o $(OBJ_DIR)/management.o $(OBJ_DIR)/matrix.o $(OBJ_DIR)/modifiers.o $(OBJ_DIR)/mortality.o $(OBJ_DIR)/partitioning-allocation.o $(OBJ_DIR)/treemodel.o $(OBJ_DIR)/common.o

G-Function.o: $(SRC_DIR)/G-Function.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/G-Function.c -o $(OBJ_DIR)/G-Function.o

Light_dominance.o: $(SRC_DIR)/Light_dominance.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/Light_dominance.c -o $(OBJ_DIR)/Light_dominance.o

allometry.o: $(SRC_DIR)/allometry.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/allometry.c -o $(OBJ_DIR)/allometry.o

canopy_cover.o: $(SRC_DIR)/canopy_cover.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/canopy_cover.c -o $(OBJ_DIR)/canopy_cover.o

crowding-competition.o: $(SRC_DIR)/crowding-competition.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/crowding-competition.c -o $(OBJ_DIR)/crowding-competition.o

dataset.o: $(SRC_DIR)/dataset.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/dataset.c -o $(OBJ_DIR)/dataset.o

establishment.o: $(SRC_DIR)/establishment.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/establishment.c -o $(OBJ_DIR)/establishment.o

light.o: $(SRC_DIR)/light.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/light.c -o $(OBJ_DIR)/light.o

log.o: $(SRC_DIR)/log.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/log.c -o $(OBJ_DIR)/log.o

main.o: $(SRC_DIR)/main.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

management.o: $(SRC_DIR)/management.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/management.c -o $(OBJ_DIR)/management.o

matrix.o: $(SRC_DIR)/matrix.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/matrix.c -o $(OBJ_DIR)/matrix.o

modifiers.o: $(SRC_DIR)/modifiers.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/modifiers.c -o $(OBJ_DIR)/modifiers.o

mortality.o: $(SRC_DIR)/mortality.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/mortality.c -o $(OBJ_DIR)/mortality.o

partitioning-allocation.o: $(SRC_DIR)/partitioning-allocation.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/partitioning-allocation.c -o $(OBJ_DIR)/partitioning-allocation.o

treemodel.o: $(SRC_DIR)/treemodel.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/treemodel.c -o $(OBJ_DIR)/treemodel.o

common.o: $(SRC_DIR)/common.c
	$(CC) $(FLAGS) -c $(SRC_DIR)/common.c -o $(OBJ_DIR)/common.o
