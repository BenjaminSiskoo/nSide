ifndef name
  name := laevateinn
endif

processors := arm gsu hg51b lr35902 r65816 spc700 upd96050
include processor/GNUmakefile

include sfc/GNUmakefile
include gb/GNUmakefile

ui_objects := laevateinn-main laevateinn-settings
ui_objects += laevateinn-interface laevateinn-debugger laevateinn-tracer
ui_objects += laevateinn-window
ui_objects += laevateinn-console laevateinn-presentation
ui_objects += laevateinn-cpu laevateinn-smp laevateinn-memory
ui_objects += laevateinn-breakpoint laevateinn-properties laevateinn-vram
ui_objects += phoenix ruby
ui_objects += $(if $(call streq,$(platform),windows),laevateinn-resource)

ifeq ($(platform),windows)
  ruby := video.wgl audio.xaudio2 input.windows
else ifeq ($(platform),macosx)
  ruby := video.cgl audio.openal input.carbon
else ifeq ($(platform),linux)
  ruby := video.glx audio.alsa input.udev
else ifeq ($(platform),bsd)
  ruby := video.glx audio.openal input.x
endif

include ruby/GNUmakefile
link += $(rubylink)

include phoenix/GNUmakefile
link += $(phoenixlink)

objects := $(ui_objects) $(objects)
objects := $(patsubst %,obj/%.o,$(objects))

obj/ruby.o: ruby/ruby.cpp $(call rwildcard,ruby/)
	$(compiler) $(rubyflags) -c $< -o $@

obj/phoenix.o: phoenix/phoenix.cpp $(call rwildcard,phoenix/)
	$(compiler) $(phoenixflags) -c $< -o $@

obj/laevateinn-main.o: $(ui)/main.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-settings.o: $(ui)/settings/settings.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-interface.o: $(ui)/interface/interface.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-debugger.o: $(ui)/debugger/debugger.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-tracer.o: $(ui)/tracer/tracer.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-window.o: $(ui)/window/window.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-console.o: $(ui)/console/console.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-cpu.o: $(ui)/cpu/cpu.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-smp.o: $(ui)/smp/smp.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-presentation.o: $(ui)/presentation/presentation.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-memory.o: $(ui)/memory/memory.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-breakpoint.o: $(ui)/breakpoint/breakpoint.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-properties.o: $(ui)/properties/properties.cpp $(call rwildcard,$(ui)/)
obj/laevateinn-vram.o: $(ui)/vram/vram.cpp $(call rwildcard,$(ui)/)

obj/laevateinn-resource.o: $(ui)/resource.rc
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
