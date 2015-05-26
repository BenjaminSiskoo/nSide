include ../nall/GNUmakefile

ifeq ($(arch),x86)
  flags += -m32
  link += -m32
endif

include ../phoenix/GNUmakefile

path := /usr/local/lib
flags := $(flags) -O3 -fomit-frame-pointer -I..

all:
	$(compiler) $(cppflags) $(flags) -fPIC -o obj/auto_purify.o -c auto_purify.cpp
ifeq ($(platform),windows)
	$(compiler) $(phoenixflags) -fPIC -o obj/phoenix.o -c ../phoenix/phoenix.cpp
	$(compiler) $(link) -shared -o phoenix.dll obj/phoenix.o $(phoenixlink)
	$(compiler) $(link) -shared -o auto_purify.dll obj/auto_purify.o -L. -lphoenix
else ifeq ($(platform),macosx)
	$(compiler) $(link) -shared -dynamiclib -undefined suppress -flat_namespace -o libauto_purify.dylib obj/auto_purify.o
else
	$(compiler) $(link) -shared -Wl,-soname,libauto_purify.so.1 -o libauto_purify.so obj/auto_purify.o
endif

resource: force
	sourcery resource/resource.bml resource/resource.cpp resource/resource.hpp

clean:
	-@$(call delete,obj/*.o)
	-@$(call delete,*.dll)
	-@$(call delete,*.so)

install: uninstall
ifeq ($(platform),windows)
else ifeq ($(platform),macosx)
	if [ ! -d ~/Library/Application\ Support/auto_purify ]; then mkdir ~/Library/Application\ Support/auto_purify; fi
	sudo cp libauto_purify.dylib $(path)/libauto_purify.1.dylib
	sudo ln -s $(path)/libauto_purify.1.dylib $(path)/libauto_purify.dylib
else
	if [ ! -d ~/.config/auto_purify ]; then mkdir ~/.config/auto_purify; fi
	sudo cp libauto_purify.so $(path)/libauto_purify.so.1
	sudo ln -s $(path)/libauto_purify.so.1 $(path)/libauto_purify.so
endif

uninstall:
ifeq ($(platform),windows)
else ifeq ($(platform),macosx)
	if [ -f $(path)/libauto_purify.dylib ]; then sudo rm $(path)/libauto_purify.dylib; fi
	if [ -f $(path)/libauto_purify.1.dylib ]; then sudo rm $(path)/libauto_purify.1.dylib; fi
else
	if [ -f $(path)/libauto_purify.so ]; then sudo rm $(path)/libauto_purify.so; fi
	if [ -f $(path)/libauto_purify.so.1 ]; then sudo rm $(path)/libauto_purify.so.1; fi
endif

force:
