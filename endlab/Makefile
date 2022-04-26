all : client.exe client_stub.exe server.exe server_stub.exe

%.exe: %.c
	gcc $< -o $@ -lpthread -lpcap

clean:
	rm -f *.exe *.uds
