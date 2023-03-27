CXXFLAGS = -I ./inc
ASMFLAGS = -o output2
ASMFILE = test1_main.s

LINKERFILES = ./src/linker.cpp ./src/util.cpp ./src/elf.cpp

ASMFILES = ./src/assembler.cpp ./src/util.cpp ./src/elf.cpp

EMUFILES = ./src/emulator.cpp ./src/util.cpp ./src/elf.cpp

emu: emulator
	./$<  test.hex

emulator: $(EMUFILES)
	g++ -o $@ $(CXXFLAGS) $^

relLink: linker
	./$< -o test ./output -relocatable

exeLink: linker
	./$< ./output1 ./output2 -o test -hex

linker: $(LINKERFILES)
	g++ -o $@ $(CXXFLAGS) $^

asm: asembler
	./$< $(ASMFLAGS) ./tests/$(ASMFILE)

asembler: $(ASMFILES)
	g++ -o $@ $(CXXFLAGS) $^

assembler: $(ASMFILES)
	g++ -o $@ $(CXXFLAGS) $^

./src/lexAsm.yy.cc: ./misc/asmLexer.l
	flex -o $@ $< 

./src/lexEmu.yy.cc: ./misc/emuLexer.l
	flex -o $@ $< 

clean:
	rm *.txt *.o *.hex *.s