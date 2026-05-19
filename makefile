nock: window.c auth.c
	gcc window.c auth.c -o nock -lxcb -lxcb-keysyms -lpam -lxcb-util 

clean:
	rm -f nock
