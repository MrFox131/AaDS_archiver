.PHONY: all

all: check_file_existance.o main.c haffman_tree_builder.c haffman_tree_packer.c stack.c pack.c unpack.c
	gcc -g .\main.c .\haffman_tree_builder.c .\haffman_tree_packer.c .\stack.c .\pack.c .\unpack.c -o .\main.exe -D _DEBUG


check_file_existance.o: check_file_existance.cpp
	g++ -c .\check_file_existance.cpp
	g++ -shared -o check_file_existance.dll .\check_file_existance.o