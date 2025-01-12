all: test-bot-sdcc

test-bot-sdcc:
	sdasz80 -o sdcc-backend.rel ../../bot-api/sdcc-backend.asm
	sdcc -mz80 --no-std-crt0 -I"../../bot-api/" test_bot.c sdcc-backend.rel -o test-bot-sdcc.ihx
	objcopy --input-target=ihex --output-target=binary test-bot-sdcc.ihx test-bot-sdcc.bin
