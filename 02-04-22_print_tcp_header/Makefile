all : s.exe r.exe

%.exe: %.c
	gcc $< -o $@ -lpthread -lpcap

clean:
	rm -f *.exe *.uds