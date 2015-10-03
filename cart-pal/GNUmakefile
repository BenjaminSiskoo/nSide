include ../nall/GNUmakefile
include ../hiro/GNUmakefile

flags += -I.. -O3
link += -mwindows
objects := obj/hiro.o obj/cart-pal.o
objects += $(if $(call streq,$(platform),windows),obj/resource.o)

all: $(objects)
	$(compiler) -o cart-pal $(objects) $(link) $(hirolink)

obj/hiro.o: ../hiro/hiro.cpp
	$(compiler) $(hiroflags) -o obj/hiro.o -c ../hiro/hiro.cpp

obj/cart-pal.o: cart-pal.cpp $(call rwildcard,core/) $(call rwildcard,heuristics/) $(call rwildcard,ui/)
	$(compiler) $(cppflags) $(flags) -o obj/cart-pal.o -c cart-pal.cpp

obj/resource.o:
	windres ../hiro/windows/hiro.rc obj/resource.o

clean:
	if [ -f ./cart-pal ]; then rm ./cart-pal; fi
	$(call delete,obj/*.o)

install:
	if [ -f ./cart-pal ]; then cp ./cart-pal $(prefix)/bin/cart-pal; fi

uninstall:
	if [ -f $(prefix)/bin/cart-pal ]; then rm $(prefix)/bin/cart-pal; fi
