# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=gsl
PKG_VERSION:=2.7.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/gsl
PKG_HASH:=dcb0fbd43048832b757ff9942691a8dd70026d5da0ff85601e52687f6deeb34b

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libgsl
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=GNU Scientific Library
  URL:=https://www.gnu.org/software/gsl/
	DEPENDS:=+libpthread +libgslcblas
endef

define Package/libgslcblas
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=GNU Scientific Library (CBLAS)
	URL:=https://www.gnu.org/software/gsl/
	DEPENDS:=+libpthread
endef

define Package/libgsl/description
	The GNU Scientific Library provides a wide range of mathematical routines,
	including random number generators, special functions and fitting methods.
endef

define Package/libgslcblas/description
	CBLAS runtime library shipped by GSL.
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-static \
	--enable-shared

define Package/libgsl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgsl.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgsl.la $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/gsl.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libgslcblas/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgslcblas.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgslcblas.la $(1)/usr/lib/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/gsl $(1)/usr/lib $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/include/gsl/*.h $(1)/usr/include/gsl/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgsl.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgsl.a $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgsl.la $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgslcblas.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgslcblas.a $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgslcblas.la $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/gsl.pc $(1)/usr/lib/pkgconfig/
endef

$(eval $(call BuildPackage,libgsl))
$(eval $(call BuildPackage,libgslcblas))
