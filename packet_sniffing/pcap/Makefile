all : packet_sniff.exe packet_send.exe

%.exe: %.c
	gcc $< -o $@ -lpthread -lpcap

clean:
	rm -f *.exe *.uds