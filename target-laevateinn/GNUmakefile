ifndef name
  name := laevateinn
endif

processors := arm gsu hg51b lr35902 r65816 spc700 upd96050
include processor/GNUmakefile

include sfc/GNUmakefile
include gb/GNUmakefile

ui_objects := ui-main ui-settings ui-interface ui-debugger ui-tracer ui-window
ui_objects += ui-console ui-presentation ui-cpu ui-smp ui-memory ui-breakpoint ui-properties ui-vram
ui_objects += phoenix ruby
ui_objects += $(if $(call streq,$(platform),win),resource)

# platform
ifeq ($(platform),x)
  ruby := audio.alsa
else ifeq ($(platform),osx)
  ruby := audio.openal
else ifeq ($(platform),win)
  ruby := audio.xaudio2
endif

# phoenix
include phoenix/GNUmakefile
link += $(phoenixlink)

# ruby
include ruby/GNUmakefile
link += $(rubylink)

# rules
objects := $(ui_objects) $(objects)
objects := $(patsubst %,obj/%.o,$(objects))

obj/ui-main.o: $(ui)/main.cpp $(call rwildcard,$(ui)/)
obj/ui-settings.o: $(ui)/settings/settings.cpp $(call rwildcard,$(ui)/*)
obj/ui-interface.o: $(ui)/interface/interface.cpp $(call rwildcard,$(ui)/*)
obj/ui-debugger.o: $(ui)/debugger/debugger.cpp $(call rwildcard,$(ui)/*)
obj/ui-tracer.o: $(ui)/tracer/tracer.cpp $(call rwildcard,$(ui)/*)
obj/ui-window.o: $(ui)/window/window.cpp $(call rwildcard,$(ui)/*)
obj/ui-console.o: $(ui)/console/console.cpp $(call rwildcard,$(ui)/*)
obj/ui-cpu.o: $(ui)/cpu/cpu.cpp $(call rwildcard,$(ui)/*)
obj/ui-smp.o: $(ui)/smp/smp.cpp $(call rwildcard,$(ui)/*)
obj/ui-presentation.o: $(ui)/presentation/presentation.cpp $(call rwildcard,$(ui)/*)
obj/ui-memory.o: $(ui)/memory/memory.cpp $(call rwildcard,$(ui)/*)
obj/ui-breakpoint.o: $(ui)/breakpoint/breakpoint.cpp $(call rwildcard,$(ui)/*)
obj/ui-properties.o: $(ui)/properties/properties.cpp $(call rwildcard,$(ui)/*)
obj/ui-vram.o: $(ui)/vram/vram.cpp $(call rwildcard,$(ui)/*)

obj/ruby.o: ruby/ruby.cpp $(call rwildcard,ruby/*)
	$(call compile,$(rubyflags))

obj/phoenix.o: phoenix/phoenix.cpp $(call rwildcard,phoenix/*)
	$(call compile,$(phoenixflags))

obj/resource.o: $(ui)/resource.rc
ifeq ($(arch),x86)
	windres --target=pe-i386 data/resource.rc obj/laevateinn-resource.o
else
	windres data/resource.rc obj/laevateinn-resource.o
endif

# targets
build: $(objects)
ifeq ($(shell id -un),root)
	$(error "make install should not be run as root")
else ifeq ($(platform),windows)
	$(strip $(compiler) -shared -o out/phoenix.dll obj/phoenix.o $(phoenixlink))
	$(strip $(compiler) -o out/$(name) $(subst obj/phoenix.o,,$(objects)) $(link) -Lout -lphoenix)
else ifeq ($(platform),macosx)
	test -d ../$(name).app || mkdir -p ../$(name).app/Contents/MacOS
	$(strip $(compiler) -o ../$(name).app/Contents/MacOS/$(name) $(objects) $(link))
else
	$(strip $(compiler) -o out/$(name) $(objects) $(link))
endif

install:
ifeq ($(shell id -un),root)
	$(error "make install should not be run as root")
else ifeq ($(platform),windows)
else ifeq ($(platform),macosx)
	sudo mkdir -p /Library/Application\ Support/$(name)
	sudo cp -R profile/* /Library/Application\ Support/$(name)
	sudo cp data/cheats.bml /Library/Application\ Support/$(name)/cheats.bml
	sudo chmod -R 777 /Library/Application\ Support/$(name)
else
	sudo install -D -m 755 out/$(name) $(DESTDIR)$(prefix)/bin/$(name)

	sudo mkdir -p /usr/share/$(name)
	sudo cp -R profile/* /usr/share/$(name)
	sudo cp data/cheats.bml /usr/share/$(name)/cheats.bml
	sudo chmod -R 777 /usr/share/$(name)
endif

uninstall:
ifeq ($(shell id -un),root)
	$(error "make uninstall should not be run as root")
else ifeq ($(platform),windows)
else ifeq ($(platform),macosx)
else
	sudo rm $(DESTDIR)$(prefix)/bin/$(name)
endif
