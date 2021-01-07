vpath %.o ./bin/
vpath %.dll ./bin/

.PHONY: all

all: libcreate_parent_dirs.dll libcheck_directory_properties.dll libget_directory_structure.dll main.c haffman_tree_builder.o haffman_tree_packer.o stack.o pack.o unpack.o haffman_tree_restorer.o archiver.o unarchiver.o
	gcc -g -B ./bin/ -L./bin/ -lget_directory_structure -lcheck_directory_properties -llibcreate_parent_dirs  ./unarchiver.c ./bin/haffman_tree_restorer.o ./bin/haffman_tree_builder.o ./archiver.c ./bin/haffman_tree_packer.o ./bin/stack.o ./bin/pack.o ./bin/unpack.o main.c -o ./bin/main.exe -D _DEBUG

archiver.o: archiver.c libget_directory_structure.dll
	gcc -g -B ./bin/ -L./bin/ -lget_directory_structure -c archiver.c  -o ./bin/archiver.o

unarchiver.o: unarchiver.c 
	gcc -g -c  unarchiver.c  -o ./bin/unarchiver.o

haffman_tree_restorer.o: haffman_tree_restorer.c 
	gcc -g -c  haffman_tree_restorer.c -o ./bin/haffman_tree_restorer.o

haffman_tree_packer.o: haffman_tree_packer.c
	gcc -g -c  haffman_tree_packer.c -o ./bin/haffman_tree_packer.o

haffman_tree_builder.o: haffman_tree_builder.c
	gcc -g -c  haffman_tree_builder.c -o ./bin/haffman_tree_builder.o

stack.o: stack.c
	gcc -g -c  stack.c -o ./bin/stack.o
unpack.o: unpack.c
	gcc -g -c  unpack.c -o ./bin/unpack.o

pack.o: pack.c
	gcc -g -c  pack.c -o ./bin/pack.o

libget_directory_structure.dll: get_directory_structure.cpp
	g++  -g -lstdc++fs -std=c++17 -c ./get_directory_structure.cpp -o ./bin/get_directory_structure.o
	g++  -g -shared -o ./bin/libget_directory_structure.dll ./bin/get_directory_structure.o

libcheck_directory_properties.dll: check_directory_properties.cpp
	g++  -g -lstdc++fs -std=c++17 -c ./check_directory_properties.cpp -o ./bin/check_directory_properties.o
	g++  -g -shared -o ./bin/libcheck_directory_properties.dll ./bin/check_directory_properties.o

libcreate_parent_dirs.dll: create_parent_dirs.cpp
	g++  -g -lstdc++fs -std=c++17 -c ./create_parent_dirs.cpp -o ./bin/create_parent_dirs.o
	g++  -g -shared -o ./bin/libcreate_parent_dirs.dll ./bin/create_parent_dirs.o

libarchiver.dll: archiver.o
	gcc -g -L./bin/ -lget_directory_structure -shared -o ./bin/libarchiver.dll ./bin/stack.o ./bin/haffman_tree_builder.o ./bin/haffman_tree_packer.o ./bin/archiver.o

libunarchiver.dll: unarchiver.o haffman_tree_restorer.o
	gcc  -g -L./bin/ -lcheck_directory_properties -lcreate_parent_dirs -shared -o ./bin/libunarchiver.dll ./bin/haffman_tree_restorer.o ./bin/unarchiver.o
	