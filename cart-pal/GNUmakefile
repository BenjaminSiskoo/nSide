include ../nall/GNUmakefile
include ../hiro/GNUmakefile

flags += -I.. -O3
link +=

ifeq ($(platform),windows)
  link += -mwindows
endif

objects := obj/hiro.o
objects += obj/cart-pal.o
objects += $(if $(call streq,$(platform),windows),obj/resource.o)

all: $(objects)
	$(compiler) -o out/cart-pal $(objects) $(link) $(hirolink)
ifeq ($(platform),macosx)
	@if [ -d out/cart-pal.app ]; then rm -r out/cart-pal.app; fi
	mkdir -p out/cart-pal.app/Contents/MacOS/
	mv out/cart-pal out/cart-pal.app/Contents/MacOS/
	cp cart-pal.plist out/cart-pal.app/Contents/Info.plist
endif

obj/hiro.o: ../hiro/hiro.cpp
	$(compiler) $(hiroflags) -o obj/hiro.o -c ../hiro/hiro.cpp

obj/cart-pal.o: cart-pal.cpp $(call rwildcard,core/) $(call rwildcard,heuristics/) $(call rwildcard,ui/)
	$(compiler) $(cppflags) $(flags) -o obj/cart-pal.o -c cart-pal.cpp

obj/resource.o:
	windres ../hiro/windows/hiro.rc obj/resource.o

clean:
ifeq ($(platform),macosx)
	@if [ -d out/cart-pal.app ]; then rm out/cart-pal.app; fi
endif
	$(call delete,obj/*)
	$(call delete,out/*)

install:
ifeq ($(platform),macosx)
	cp -r out/cart-pal.app /Applications/cart-pal.app
else
	if [ -f out/cart-pal ]; then cp out/cart-pal $(prefix)/bin/cart-pal; fi
endif

uninstall:
ifeq ($(platform),macosx)
	if [ -d /Applications/cart-pal.app ]; then rm -r /Applications/cart-pal.app; fi
else
	if [ -f $(prefix)/bin/cart-pal ]; then rm $(prefix)/bin/cart-pal; fi
endif
