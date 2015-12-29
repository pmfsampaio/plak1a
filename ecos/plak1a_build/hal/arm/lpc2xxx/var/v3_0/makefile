# eCos makefile

# This is a generated file - do not edit

export PREFIX := /home/psampaio/Develop/plak1a/ecos/plak1a_install
export COMMAND_PREFIX := arm-eabi-
export CC := $(COMMAND_PREFIX)gcc
export OBJCOPY := $(COMMAND_PREFIX)objcopy
export HOST := UNIX
export AR := $(COMMAND_PREFIX)ar

export REPOSITORY := /home/psampaio/Develop/ecos/ecos-3.0/packages
PACKAGE := hal/arm/lpc2xxx/var/v3_0
OBJECT_PREFIX := hal_arm_lpc2xxx_var
CFLAGS := -Wall -Wpointer-arith -Wstrict-prototypes -Wundef -Woverloaded-virtual -Wno-write-strings -mno-thumb-interwork -mcpu=arm7tdmi -g -O2 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions
LDFLAGS := -mno-thumb-interwork -mcpu=arm7tdmi -Wl,--gc-sections -Wl,-static -g -nostdlib
VPATH := $(REPOSITORY)/$(PACKAGE)
INCLUDE_PATH := $(INCLUDE_PATH) -I$(PREFIX)/include $(foreach dir,$(VPATH),-I$(dir) -I$(dir)/src -I$(dir)/tests) -I.
MLT := $(wildcard $(REPOSITORY)/$(PACKAGE)/include/pkgconf/mlt*.ldi $(REPOSITORY)/$(PACKAGE)/include/pkgconf/mlt*.h)
TESTS := tests/iap_test

build: headers libtarget.a.stamp

LIBRARY := libtarget.a
COMPILE := src/hal_diag.c src/lpc2xxx_misc.c src/lpc2xxx_iap.c
OBJECTS := $(COMPILE:.cxx=.o.d)
OBJECTS := $(OBJECTS:.cpp=.o.d)
OBJECTS := $(OBJECTS:.c=.o.d)
OBJECTS := $(OBJECTS:.S=.o.d)

$(LIBRARY).stamp: $(OBJECTS)
	$(AR) rcs $(PREFIX)/lib/$(@:.stamp=) $(foreach obj,$?,$(if $(obj:%.o=),$(dir $(obj))$(OBJECT_PREFIX)_$(notdir $(obj:.o.d=.o)),$(obj)))
	@cat $(foreach obj,$^,$(obj:.o=.o.d)) > $(@:.stamp=.deps)
	@touch $@

headers: mlt_headers $(PREFIX)/include/cyg/hal/lpc2xxx_misc.h $(PREFIX)/include/cyg/hal/var_io.h $(PREFIX)/include/cyg/hal/hal_cache.h $(PREFIX)/include/cyg/hal/hal_diag.h $(PREFIX)/include/cyg/hal/hal_var_ints.h $(PREFIX)/include/cyg/hal/plf_stub.h $(PREFIX)/include/cyg/hal/lpc2xxx_iap.h $(PREFIX)/include/cyg/hal/var_arch.h

$(PREFIX)/include/cyg/hal/lpc2xxx_misc.h: $(REPOSITORY)/$(PACKAGE)/include/lpc2xxx_misc.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/var_io.h: $(REPOSITORY)/$(PACKAGE)/include/var_io.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_cache.h: $(REPOSITORY)/$(PACKAGE)/include/hal_cache.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_diag.h: $(REPOSITORY)/$(PACKAGE)/include/hal_diag.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/hal_var_ints.h: $(REPOSITORY)/$(PACKAGE)/include/hal_var_ints.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/plf_stub.h: $(REPOSITORY)/$(PACKAGE)/include/plf_stub.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/lpc2xxx_iap.h: $(REPOSITORY)/$(PACKAGE)/include/lpc2xxx_iap.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/hal/var_arch.h: $(REPOSITORY)/$(PACKAGE)/include/var_arch.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

include $(REPOSITORY)/pkgconf/rules.mak

