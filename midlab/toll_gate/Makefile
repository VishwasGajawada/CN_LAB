all : v.exe FT.exe TM.exe g1.exe g2.exe g3.exe

%.exe: %.c
	gcc $< -o $@ -lpthread

clean:
	rm -f *.exe *.uds

