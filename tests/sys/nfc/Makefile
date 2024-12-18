BOARD ?= nrf52840dk
include ../Makefile.sys_common

USEPKG += nrfx
USEMODULE += nrfx_nfct
USEMODULE += nrfx_nfct_t2t_emulator
USEMODULE += nfc_t2t
USEMODULE += nfct

CFLAGS += -DISR_STACKSIZE=1024*2
CFLAGS += -Wno-error=cast-align
CFLAGS += -Wno-error=unused-variable



include $(RIOTBASE)/Makefile.include
