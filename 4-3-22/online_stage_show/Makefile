
all : stage p a j1 j2 j3

stage : s.c
	gcc s.c -o s.exe

p : p.c
	gcc p.c -o p.exe

a : a.c
	gcc a.c -o a.exe

j1 : j1.c
	gcc j1.c -o j1.exe

j2 : j2.c
	gcc j2.c -o j2.exe

j3 : j3.c
	gcc j3.c -o j3.exe

clean :
	rm -f ./s.exe ./p.exe ./a.exe ./j1.exe ./j2.exe ./j3.exe
