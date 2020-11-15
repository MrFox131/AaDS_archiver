.PHONY: all

all: get_directory_structure.o main.c haffman_tree_builder.o haffman_tree_packer.o stack.o pack.o unpack.o haffman_tree_restorer.o archiver.o unarchiver.o
	gcc -L./ -lget_directory_structure -g ./unarchiver.o ./haffman_tree_restorer.o ./haffman_tree_builder.o ./archiver.c ./haffman_tree_packer.o ./stack.o ./pack.o ./unpack.o ./main.c -o ./main.exe -D _DEBUG

archiver.o: archiver.c haffman_tree_builder.o haffman_tree_packer.o stack.o get_directory_structure.o
	gcc -L./ -lget_directory_structure -c archiver.c

unarchiver.o: unarchiver.c haffman_tree_restorer.o
	gcc -c -g haffman_tree_restorer.o unarchiver.c 

haffman_tree_restorer.o: haffman_tree_restorer.c 
	gcc -c -g haffman_tree_restorer.c

haffman_tree_packer.o: haffman_tree_builder.o haffman_tree_packer.c
	gcc -c -g haffman_tree_packer.c

haffman_tree_builder.o: haffman_tree_builder.c
	gcc -c -g haffman_tree_builder.c

stack.o: stack.c
	gcc -c -g stack.c
unpack.o: unpack.c
	gcc -c -g unpack.c

pack.o: pack.c
	gcc -c -g pack.c

get_directory_structure.o: get_directory_structure.cpp
	g++ -g -lstdc++fs -std=c++17 -c ./get_directory_structure.cpp
	g++ -g -shared -o libget_directory_structure.dll ./get_directory_structure.o