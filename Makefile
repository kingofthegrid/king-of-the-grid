all: kotg

kotg:
	mkdir -p build
	cmake -S . -B ./build/kotg
	cmake --build ./build/kotg

kotg-web:
	mkdir -p build/kotg-web && cd build/kotg-web && emcmake cmake ../.. && make

testbot:
	mkdir -p build
	cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -S examples/test-bot -B ./build/testbot
	cmake --build ./build/testbot

clean:
	@rm -rf bin/kotg*
	@rm -rf build
	@rm -rf cmake-build-*
