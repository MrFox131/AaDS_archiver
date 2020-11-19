.PHONY: all

all: libcreate_parent_dirs.dll libcheck_directory_properties.dll libget_directory_structure.dll main.c haffman_tree_builder.o haffman_tree_packer.o stack.o pack.o unpack.o haffman_tree_restorer.o archiver.o unarchiver.c
	gcc -L./ -lget_directory_structure -lcheck_directory_properties -llibcreate_parent_dirs -g ./unarchiver.o ./haffman_tree_restorer.o ./haffman_tree_builder.o ./archiver.o ./haffman_tree_packer.o ./stack.o ./pack.o ./unpack.o ./main.c -o ./main.exe -D _DEBUG

archiver.o: archiver.c 
	gcc -L./ -lget_directory_structure -c archiver.c

unarchiver.o: unarchiver.c 
	gcc -c -g unarchiver.c 

haffman_tree_restorer.o: haffman_tree_restorer.c 
	gcc -c -g haffman_tree_restorer.c

haffman_tree_packer.o: haffman_tree_packer.c
	gcc -c -g haffman_tree_packer.c

haffman_tree_builder.o: haffman_tree_builder.c
	gcc -c -g haffman_tree_builder.c

stack.o: stack.c
	gcc -c -g stack.c
unpack.o: unpack.c
	gcc -c -g unpack.c

pack.o: pack.c
	gcc -c -g pack.c

libget_directory_structure.dll: get_directory_structure.cpp
	g++ -g -lstdc++fs -std=c++17 -c ./get_directory_structure.cpp
	g++ -g -shared -o libget_directory_structure.dll ./get_directory_structure.o

libcheck_directory_properties.dll: check_directory_properties.cpp
	g++ -g -lstdc++fs -std=c++17 -c ./check_directory_properties.cpp
	g++ -g -shared -o libcheck_directory_properties.dll ./check_directory_properties.o

libcreate_parent_dirs.dll: create_parent_dirs.cpp
	g++ -g -lstdc++fs -std=c++17 -c ./create_parent_dirs.cpp
	g++ -g -shared -o libcreate_parent_dirs.dll ./create_parent_dirs.o