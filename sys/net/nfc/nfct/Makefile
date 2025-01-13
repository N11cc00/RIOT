MODULE = nfct

# Check if one tag type module is available
ifeq (,$(filter nfc_t2t,$(USEMODULE)))
	$(error "No NFCT tag type module used.")
endif

include $(RIOTBASE)/Makefile.base