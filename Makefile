.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "DEVKITARM não está configurado")
endif

include $(DEVKITARM)/ds_rules

TARGET := ds_farm_v8
BUILD := build
SOURCES := source
INCLUDES := include

ARCH := -march=armv5te -mtune=arm946e-s -mthumb

CFLAGS := -g -Wall -O2 -ffunction-sections -fdata-sections $(ARCH)
CFLAGS += $(INCLUDE) -DARM9

CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS := -g $(ARCH)

LDFLAGS := -specs=ds_arm9.specs -g $(ARCH)

LIBS := -lnds9
LIBDIRS := $(LIBNDS)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH := $(CURDIR)/$(SOURCES)
export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES := $(notdir $(wildcard $(CURDIR)/$(SOURCES)/*.c))
export OFILES := $(CFILES:.c=.o)

export LD := $(CC)

export INCLUDE := $(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir)) \
                  $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                  -I$(CURDIR)/$(BUILD)

export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: all clean build

all: $(BUILD)

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
