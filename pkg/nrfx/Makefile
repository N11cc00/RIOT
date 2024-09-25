PKG_NAME=nrfx
PKG_URL=https://github.com/NordicSemiconductor/nrfx
# Release 2.7.0
PKG_VERSION=3521c97df0b9549daecf867fb588f62819c317b4
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

PSRC = $(PKGDIRBASE)/nrfx/drivers/src

# this will build the nrfx_nfct.c file
# this target needs a different name later on
all:
	$(QQ)"$(MAKE)" -C $(PSRC) -f $(CURDIR)/nrfx_nfct.mk
