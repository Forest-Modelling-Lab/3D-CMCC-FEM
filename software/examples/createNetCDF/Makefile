NAME=createNetCDF
CXX=g++
FLAGS=-Wall -pedantic -g -O3 -DDEBUG -march=native -lpthread -std=c++0x
GDAL_CFLAGS  += `gdal-config --cflags`
GDAL_LDFLAGS += `gdal-config --libs`
BINDIR=.

all: $(NAME)

clean:
		rm -f $(NAME)
			rm -f createNetCDF.o

install: $(NAME)
		cp $(NAME) $(BINDIR)

uninstall:
		rm -f $(BINDIR)/$(NAME)

$(NAME): createNetCDF.o
		$(CXX) createNetCDF.o $(GDAL_LDFLAGS) -o $(NAME)

createNetCDF.o: createNetCDF.cpp
		$(CXX) $(FLAGS) $(GDAL_CFLAGS) -c createNetCDF.cpp

# g++ -Wall -pedantic -g -O3 -DDEBUG -march=native -lpthread -std=c++0x `gdal-config --cflags` -c createNetCDF.cpp
# g++ createNetCDF.o `gdal-config --libs` -o createNetCDF
