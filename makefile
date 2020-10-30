.PHONY: all

all: check_file_existance.o main.c haffman_tree_builder.c haffman_tree_packer.c stack.c pack.c unpack.c haffman_tree_restorer.c
	gcc -g .\haffman_tree_restorer.c .\haffman_tree_builder.c .\archiver.c .\haffman_tree_packer.c .\stack.c .\pack.c .\unpack.c .\main.c -o .\main.exe -D _DEBUG


check_file_existance.o: check_file_existance.cpp
	g++ -c .\check_file_existance.cpp
	g++ -shared -o check_file_existance.dll .\check_file_existance.o