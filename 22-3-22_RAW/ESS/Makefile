all : ess.exe first_client.exe C.exe cleaner.exe

%.exe: %.c
	gcc $< -o $@ -lpthread

clean:
	rm -f *.exe *.uds