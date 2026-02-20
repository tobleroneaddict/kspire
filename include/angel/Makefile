DEBUG = FALSE

GCC = nspire-gcc
AS  = nspire-as
GXX = nspire-g++
LD  = nspire-ld
GENZEHN = genzehn


GCCFLAGS = -Wall -W -marm
LDFLAGS =
ZEHNFLAGS = --name "AnGL-example"

ifeq ($(DEBUG),FALSE)
	GCCFLAGS += -Os
else
	GCCFLAGS += -O0 -g
endif

OBJS = $(patsubst %.c, %.o, $(shell find . -name \*.c))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -name \*.cpp))
OBJS += $(patsubst %.S, %.o, $(shell find . -name \*.S))
EXE = AnGL-example
DISTDIR = .
vpath %.tns $(DISTDIR)
vpath %.elf $(DISTDIR)

all: $(EXE).tns

#cool rules
%.o: %.c
	@mkdir -p $(dir $@)
	$(GCC) $(GCCFLAGS) -c $< -o $@

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(GXX) -DKSPIRE_PLATFORM_NSPIRE=1 $(GCCFLAGS) -c $< -o $@

%.o: %.S
	@mkdir -p $(dir $@)
	$(AS) -c $< -o $@

$(EXE).elf: $(OBJS)
	mkdir -p $(DISTDIR)
	$(LD) $^ -o $(DISTDIR)/$@ $(LDFLAGS)

$(EXE).tns: $(EXE).elf
	$(GENZEHN) --input $(DISTDIR)/$^ --output $(DISTDIR)/$@.zehn $(ZEHNFLAGS)
	make-prg $(DISTDIR)/$@.zehn $(DISTDIR)/$@
	rm $(DISTDIR)/$@.zehn
clean_some:
	rm -f $(DISTDIR)/$(EXE).tns $(DISTDIR)/$(EXE).elf $(DISTDIR)/$(EXE).zehn
clean:
	find . -name '*.o' -type f -delete
	rm -f $(DISTDIR)/$(EXE).tns $(DISTDIR)/$(EXE).elf $(DISTDIR)/$(EXE).zehn
