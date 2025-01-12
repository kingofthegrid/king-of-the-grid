all: kotg

kotg:
	mkdir -p build
	cmake -S . -B ./build/kotg
	cmake --build ./build/kotg

kotg-web:
	mkdir -p build/kotg-web && cd build/kotg-web && emcmake cmake ../.. && make

test-bot-z88dk:
	mkdir -p build
	cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -S examples/test-bot-z88dk -B ./build/test-bot-z88dk
	cmake --build ./build/test-bot-z88dk

test-bot-sdcc:
	make -C examples/test-bot-sdcc

clean:
	@rm -rf bin/kotg*
	@rm -rf build
	@rm -rf cmake-build-*
