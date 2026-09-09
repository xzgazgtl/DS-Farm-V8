.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "DEVKITARM não está configurado")
endif

include $(DEVKITARM)/ds_rules

TARGET := ds_farm_v8
BUILD := build

ARCH := -march=armv5te -mtune=arm946e-s
CFLAGS := -g -Wall -O2 $(ARCH)
CFLAGS += -DARM9

ASFLAGS := -g $(ARCH)
LDFLAGS := -specs=ds_arm9.specs -g $(ARCH)

LIBS := -lnds9
LIBDIRS := $(LIBNDS)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH := $(CURDIR)/source
export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES := main.c
export OFILES := main.o

export INCLUDE := -I$(CURDIR)/include \
                  $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                  -I$(CURDIR)/$(BUILD)

export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export LD := $(CC)

.PHONY: all clean build

all: build

build:
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $@ -f $(CURDIR)/Makefile

clean:
	@rm -rf $(BUILD) $(TARGET).elf $(TARGET).nds

else

$(OUTPUT).nds: $(OUTPUT).elf

$(OUTPUT).elf: $(OFILES)

-include $(DEPSDIR)/*.d

endif
