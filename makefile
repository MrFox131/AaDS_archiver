.PHONY: all

all: check_file_existance.o main.c haffman_tree_builder.c haffman_tree_packer.c stack.c pack.c unpack.c haffman_tree_restorer.c
	gcc -g .\unarchiver.c .\haffman_tree_restorer.c .\haffman_tree_builder.c .\archiver.c .\haffman_tree_packer.c .\stack.c .\pack.c .\unpack.c .\main.c -o .\main.exe -D _DEBUG


check_file_existance.o: get_directory_structure.cpp
	g++ -lstdc++fs -std=c++17 -c .\get_directory_structure.cpp
	g++ -shared -o get_directory_structure.dll .\get_directory_structure.o