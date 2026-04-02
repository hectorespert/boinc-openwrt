# Copyright 2026
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pla-util
PKG_VERSION:=2.1.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/serock/pla-util/archive/refs/tags
PKG_HASH:=66374b0b17bce5cd30969f547e9727e48fac742744740a2d1a4ac75604c50580
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_LICENSE:=GPL-3.0-or-later
PKG_LICENSE_FILES:=LICENSE
PKG_BUILD_PARALLEL:=1
PKG_BUILD_FLAGS:=no-lto

include $(INCLUDE_DIR)/package.mk

define Package/pla-util
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Power line adapter utility for Broadcom chipsets
  URL:=https://github.com/serock/pla-util
  DEPENDS:=+libpcap
endef

define Package/pla-util/description
  pla-util is a command line utility for managing HomePlug AV2
  power line adapters based on supported Broadcom chipsets.
endef

define Build/Configure
endef

define Build/Compile
	+$(MAKE_VARS) $(MAKE) -C $(PKG_BUILD_DIR) \
		SHELL="/bin/sh" \
		GCC="$(TARGET_CC)" \
		GNATMAKE="$(TARGET_CROSS)gnatmake" \
		GNATBIND="$(TARGET_CROSS)gnatbind" \
		GNATLINK="$(TARGET_CROSS)gnatlink" \
		ADAFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/pla-util/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/pla-util $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/share/bash-completion/completions
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/completions/pla-util $(1)/usr/share/bash-completion/completions/
endef

$(eval $(call BuildPackage,pla-util))
