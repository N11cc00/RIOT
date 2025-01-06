BOARD ?= nrf52840dk
include ../Makefile.sys_common

# this needs to be outsourced to the respective makefiles
USEPKG += nrfx
USEMODULE += nrfx_nfct
USEMODULE += nrfx_nfct_t2t_emulator
USEMODULE += nfc_t2t
USEMODULE += nfct
USEMODULE += event

CFLAGS += -DISR_STACKSIZE=1024*2
CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=2048
CFLAGS += -Wno-error=cast-align
CFLAGS += -Wno-error=unused-variable



include $(RIOTBASE)/Makefile.include
