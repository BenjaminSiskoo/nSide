include nall/GNUmakefile

fc  := fc
sfc := sfc
gb  := gb
gba := gba

ifndef profile
  profile := accuracy
endif
ifndef target
  target := nSide-t
endif

ifeq ($(target),loki)
  options += debugger
else ifeq ($(target),star-rod)
  options += debugger
endif
# console := true

# compiler
flags += -I. -O3
objects := libco

# profile-guided optimization mode
# pgo := instrument
# pgo := optimize

ifeq ($(pgo),instrument)
  flags += -fprofile-generate
  link += -lgcov
else ifeq ($(pgo),optimize)
  flags += -fprofile-use
endif

# platform
ifeq ($(platform),windows)
  ifeq ($(console),true)
    link += -mconsole
  else
    link += -mwindows
  endif
  link += -mthreads -luuid -lkernel32 -luser32 -lgdi32 -lcomctl32 -lcomdlg32 -lshell32
  link += -Wl,-enable-auto-import
  link += -Wl,-enable-runtime-pseudo-reloc
else ifeq ($(platform),macosx)
  flags += -march=native
else ifeq ($(platform),linux)
  flags += -march=native -fopenmp
  link += -fopenmp
  link += -Wl,-export-dynamic
  link += -lX11 -lXext
else ifeq ($(platform),bsd)
  flags += -march=native -fopenmp
  link += -fopenmp
  link += -Wl,-export-dynamic
  link += -lX11 -lXext
else
  $(error unsupported platform.)
endif

ui := target-$(target)

# implicit rules
compile = \
  $(strip \
    $(if $(filter %.c,$<), \
      $(compiler) $(cflags) $(flags) $1 -c $< -o $@, \
      $(if $(filter %.cpp,$<), \
        $(compiler) $(cppflags) $(flags) $1 -c $< -o $@ \
      ) \
    ) \
  )

%.o: $<; $(call compile)

all: build;

obj/libco.o: libco/libco.c libco/*

include $(ui)/GNUmakefile
flags := $(flags) $(foreach o,$(call strupper,$(options)),-D$o)

# targets
clean:
	-@$(call delete,out/*)
	-@$(call delete,obj/*.o)
	-@$(call delete,obj/*.a)
	-@$(call delete,obj/*.so)
	-@$(call delete,obj/*.dylib)
	-@$(call delete,obj/*.dll)

archive:
	if [ -f nSide.tar.xz ]; then rm nSide.tar.xz; fi
	tar -cJf nSide.tar.xz `ls`

sync:
ifeq ($(shell id -un),byuu)
	if [ -d ./libco ]; then rm -r ./libco; fi
	if [ -d ./nall ]; then rm -r ./nall; fi
	if [ -d ./ruby ]; then rm -r ./ruby; fi
	if [ -d ./hiro ]; then rm -r ./hiro; fi
	cp -r ../libco ./libco
	cp -r ../nall ./nall
	cp -r ../ruby ./ruby
	cp -r ../hiro ./hiro
	rm -r libco/doc
	rm -r libco/-test
	rm -r nall/-test
	rm -r ruby/-test
	rm -r hiro/-test
endif

help:;
