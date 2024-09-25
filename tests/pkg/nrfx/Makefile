BOARD ?= nrf52dk
include ../Makefile.pkg_common

USEPKG += nrfx

# include nrfx_nfct support
USEMODULE += nrfx_nfct

CFLAGS += -Wno-error=cast-align
CFLAGS += -Wno-error=unused-variable
CFLAGS += -Wno-error=unused-parameter

include $(RIOTBASE)/Makefile.include