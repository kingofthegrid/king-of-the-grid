all: sdl2-sdl

sdl2-sdl:
	mkdir -p build
	cmake -S . -B ./build/sdl2
	cmake --build ./build/sdl2

test-bot:
	mkdir -p build
	cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -S examples/test-bot -B ./build/test-bot
	cmake --build ./build/test-bot

sdl2-test-bot: sdl2-sdl
	mkdir -p build
	cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -S examples/test-bot -B ./build/test-bot
	cmake --build ./build/test-bot
	cd ./build/test-bot && ../../bin/kotg_sdl2 test-bot test-bot 1337

clean:
	@rm -rf bin/kotg*
	@rm -rf build
	@rm -rf cmake-build-*
