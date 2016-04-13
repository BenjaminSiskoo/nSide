ifndef name
  name := star-rod
endif

flags += -DDEBUGGER

include sfc/GNUmakefile
include processor/GNUmakefile

ui_objects := star-rod-laevateinn star-rod-program star-rod-settings
ui_objects += star-rod-debugger star-rod-tracer
ui_objects += star-rod-window
ui_objects += star-rod-console star-rod-presentation
ui_objects += star-rod-cpu star-rod-smp star-rod-memory
ui_objects += star-rod-breakpoint star-rod-properties
ui_objects += star-rod-vram star-rod-bg star-rod-palette
ui_objects += ruby hiro
ui_objects += $(if $(call streq,$(platform),windows),star-rod-resource)

# platform
ifeq ($(platform),windows)
  ruby := video.wgl audio.xaudio2 input.windows
else ifeq ($(platform),macosx)
  ruby := video.cgl audio.openal input.quartz
else ifeq ($(platform),linux)
  ruby := video.xshm audio.openal input.sdl
else ifeq ($(platform),bsd)
  ruby := video.xshm audio.oss input.sdl
endif

include ../ruby/GNUmakefile
link += $(rubylink)

include ../hiro/GNUmakefile
link += $(hirolink)

# rules
objects := $(ui_objects) $(objects)
objects := $(patsubst %,obj/%.o,$(objects))

obj/ruby.o: ../ruby/ruby.cpp $(call rwildcard,../ruby/)
	$(compiler) $(rubyflags) -c $< -o $@

obj/hiro.o: ../hiro/hiro.cpp $(call rwildcard,../hiro/)
	$(compiler) $(hiroflags) -c $< -o $@

obj/star-rod-laevateinn.o: $(ui)/laevateinn.cpp $(call rwildcard,$(ui)/)
obj/star-rod-program.o: $(ui)/program/program.cpp $(call rwildcard,$(ui)/)
obj/star-rod-settings.o: $(ui)/settings/settings.cpp $(call rwildcard,$(ui)/)
obj/star-rod-debugger.o: $(ui)/debugger/debugger.cpp $(call rwildcard,$(ui)/)
obj/star-rod-tracer.o: $(ui)/tracer/tracer.cpp $(call rwildcard,$(ui)/)
obj/star-rod-window.o: $(ui)/window/window.cpp $(call rwildcard,$(ui)/)
obj/star-rod-console.o: $(ui)/console/console.cpp $(call rwildcard,$(ui)/)
obj/star-rod-cpu.o: $(ui)/cpu/cpu.cpp $(call rwildcard,$(ui)/)
obj/star-rod-smp.o: $(ui)/smp/smp.cpp $(call rwildcard,$(ui)/)
obj/star-rod-presentation.o: $(ui)/presentation/presentation.cpp $(call rwildcard,$(ui)/)
obj/star-rod-memory.o: $(ui)/memory/memory.cpp $(call rwildcard,$(ui)/)
obj/star-rod-breakpoint.o: $(ui)/breakpoint/breakpoint.cpp $(call rwildcard,$(ui)/)
obj/star-rod-properties.o: $(ui)/properties/properties.cpp $(call rwildcard,$(ui)/)
obj/star-rod-vram.o: $(ui)/vram/vram.cpp $(call rwildcard,$(ui)/)
obj/star-rod-bg.o: $(ui)/bg/bg.cpp $(call rwildcard,$(ui)/)
obj/star-rod-palette.o: $(ui)/palette/palette.cpp $(call rwildcard,$(ui)/)

obj/star-rod-resource.o:
	windres data/resource.rc obj/star-rod-resource.o

# targets
build: $(objects)
	$(call unique,$(compiler) -o out/$(name) $(objects) $(link))
ifeq ($(platform),macosx)
	@if [ -d out/$(name).app ]; then rm -r out/$(name).app; fi
	mkdir -p out/$(name).app/Contents/MacOS/
	mkdir -p out/$(name).app/Contents/Resources/
	mv out/$(name) out/$(name).app/Contents/MacOS/$(name)
	cp data/$(name).plist out/$(name).app/Contents/Info.plist
	sips -s format icns data/$(name).png --out out/$(name).app/Contents/Resources/$(name).icns
endif

install:
ifeq ($(shell id -un),root)
	$(error "make install should not be run as root")
else ifeq ($(platform),windows)
else ifeq ($(platform),macosx)
	mkdir -p ~/Library/Application\ Support/$(name)/
	mkdir -p ~/Emulation/System/
	cp -R out/$(name).app /Applications/$(name).app
	cp -R profile/* ~/Library/Application\ Support/$(name)/
else ifneq ($(filter $(platform),linux bsd),)
	mkdir -p $(prefix)/bin/
	mkdir -p $(prefix)/share/icons/
	mkdir -p $(prefix)/share/$(name)/
	cp out/$(name) $(prefix)/bin/$(name)
	cp data/$(name).desktop $(prefix)/share/applications/$(name).desktop
	cp data/$(name).png $(prefix)/share/icons/$(name).png
	cp -R profile/* $(prefix)/share/$(name)/
endif

uninstall:
ifeq ($(shell id -un),root)
	$(error "make uninstall should not be run as root")
else ifeq ($(platform),windows)
else ifeq ($(platform),macosx)
	if [ -d /Applications/$(name).app ]; then rm -r /Applications/$(name).app; fi
else ifneq ($(filter $(platform),linux bsd),)
	if [ -f $(prefix)/bin/$(name) ]; then rm $(prefix)/bin/$(name); fi
	if [ -f $(prefix)/share/applications/$(name).desktop ]; then rm $(prefix)/share/applications/$(name).desktop; fi
	if [ -f $(prefix)/share/icons/$(name).png ]; then rm $(prefix)/share/icons/$(name).png; fi
endif
