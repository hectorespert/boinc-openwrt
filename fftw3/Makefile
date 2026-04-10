# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=fftw3
PKG_VERSION:=3.3.10
PKG_RELEASE:=1

PKG_SOURCE:=fftw-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://www.fftw.org
PKG_HASH:=56c932549852cddcfafdab3820b0200c7742675be92179e59e6215b340e26467
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/fftw-$(PKG_VERSION)

PKG_LICENSE:=GPL-2.0-or-later
PKG_LICENSE_FILES:=COPYING
PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--prefix=/usr \
	--enable-shared \
	--enable-static \
	--enable-threads \
	--enable-type-prefix \
	--disable-debug \
	--disable-fortran \
	--disable-doc

ifeq ($(BUILD_VARIANT),single)
  CONFIGURE_ARGS += \
	$(if $(findstring neon,$(CONFIG_TARGET_OPTIMIZATION)),--enable-neon) \
	--enable-single
endif

ifeq ($(BUILD_VARIANT),long-double)
  CONFIGURE_ARGS += --enable-long-double
endif

define Package/libfftw3/Default
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Fast Fourier transform library
  URL:=https://www.fftw.org/
  DEPENDS:=+libpthread
endef

define Package/libfftw3
	$(call Package/libfftw3/Default)
  VARIANT:=double
endef

define Package/libfftw3f
	$(call Package/libfftw3/Default)
  VARIANT:=single
  TITLE+= (single precision)
endef

define Package/libfftw3l
	$(call Package/libfftw3/Default)
  VARIANT:=long-double
  TITLE+= (long-double precision)
endef

define Package/libfftw3/description
	FFTW is a collection of fast C routines for computing the
	Discrete Fourier Transform in one or more dimensions.
endef

define Package/libfftw3f/description
	$(call Package/libfftw3/description)
	This package contains the single precision library.
endef

define Package/libfftw3l/description
	$(call Package/libfftw3/description)
	This package contains the long-double precision library.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR="$(PKG_INSTALL_DIR)" install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/fftw3.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfftw3*.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfftw3*.a $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/* $(1)/usr/lib/pkgconfig/
endef

define Package/libfftw3/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfftw3.so* $(1)/usr/lib/
endef

define Package/libfftw3f/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfftw3f.so* $(1)/usr/lib/
endef

define Package/libfftw3l/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfftw3l.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libfftw3))
$(eval $(call BuildPackage,libfftw3f))
$(eval $(call BuildPackage,libfftw3l))
