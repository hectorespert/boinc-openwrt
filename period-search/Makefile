# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=period-search
PKG_VERSION:=20260813
PKG_RELEASE:=1

PKG_SOURCE:=main.tar.gz
PKG_SOURCE_URL:=https://github.com/AsteroidsAtHome/PeriodSearch/archive/refs/heads
PKG_HASH:=83ebea305ad16c627425f06237c9c0da46b54eb9c7e7dbd96139ea0aecafc746
PKG_BUILD_DIR:=$(BUILD_DIR)/PeriodSearch-main

PKG_LICENSE:=GPL-3.0-or-later
PKG_LICENSE_FILES:=LICENSE
PKG_BUILD_PARALLEL:=1
PKG_BUILD_DEPENDS:=boinc
BOINC_API_VERSION:=$(shell awk '/^#define BOINC_MAJOR_VERSION/ {maj=$$3} /^#define BOINC_MINOR_VERSION/ {min=$$3} /^#define BOINC_RELEASE/ {rel=$$3} END { if (maj != "" && min != "" && rel != "") printf "%s.%s.%s", maj, min, rel; else printf "7.21.0" }' $(STAGING_DIR)/usr/include/boinc/version.h 2>/dev/null)

include $(INCLUDE_DIR)/package.mk

define Package/period-search
  SECTION:=net
  CATEGORY:=Network
	TITLE:=Asteroids@home Period Search OpenWrt application
	URL:=https://github.com/AsteroidsAtHome/PeriodSearch/tree/main
  DEPENDS:=+boinc +libstdcpp
endef

define Package/period-search/description
	Builds the OpenWrt Period Search executable from Asteroids@home
	main branch snapshot and installs files for anonymous-platform usage in BOINC.
endef

define Build/Configure
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/build
	$(TARGET_CXX) \
		$(TARGET_CXXFLAGS) \
		$(TARGET_CPPFLAGS) \
		-std=gnu++2a -O3 -Wall -DNDEBUG \
		-Wno-error \
		-I$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search \
		-I$(STAGING_DIR)/usr/include/boinc \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/areanorm.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/blmatrix.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/bright.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/conv.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/covsrt.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/curv.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/dot_product.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/ellfit.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/gauss_errc.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/lubksb.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/ludcmp.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/matrix.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/memory.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/mrqcof.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/mrqmin.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/phasec.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/sphfunc.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/trifac.c \
		$(PKG_BUILD_DIR)/period_search_optimization_cpu/period_search/period_search_BOINC.cpp \
		$(TARGET_LDFLAGS) -Wl,--as-needed \
		-L$(STAGING_DIR)/usr/lib \
		-lboinc_api -lboinc -lm -lpthread \
		-o $(PKG_BUILD_DIR)/build/period_search_openwrt_$(ARCH_PACKAGES)
endef

define Package/period-search/install
	$(INSTALL_DIR) $(1)/opt/period-search
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/period_search_openwrt_$(ARCH_PACKAGES) $(1)/opt/period-search/
	$(CP) ./files/app_info.xml $(PKG_BUILD_DIR)/app_info.xml
	$(SED) 's|period_search_openwrt_aarch64|period_search_openwrt_$(ARCH_PACKAGES)|g' $(PKG_BUILD_DIR)/app_info.xml
	$(SED) 's|@BOINC_API_VERSION@|$(BOINC_API_VERSION)|g' $(PKG_BUILD_DIR)/app_info.xml
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/app_info.xml $(1)/opt/period-search/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/period-search.init $(1)/etc/init.d/period-search
endef

define Package/period-search/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] && exit 0
/etc/init.d/period-search enable
/etc/init.d/period-search start
exit 0
endef

$(eval $(call BuildPackage,period-search))
