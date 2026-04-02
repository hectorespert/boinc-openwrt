# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=period-search
PKG_VERSION:=20260402
PKG_RELEASE:=1

PKG_SOURCE:=dev.tar.gz
PKG_SOURCE_URL:=https://github.com/AsteroidsAtHome/PeriodSearch/archive/refs/heads
PKG_HASH:=7d84cb833ad7a8bc9e48e4da798bc43847477e45147edac0dc9c0703563f06b7
PKG_BUILD_DIR:=$(BUILD_DIR)/PeriodSearch-dev

PKG_LICENSE:=GPL-3.0-or-later
PKG_LICENSE_FILES:=LICENSE
PKG_BUILD_PARALLEL:=1
PKG_BUILD_DEPENDS:=boinc

include $(INCLUDE_DIR)/package.mk

define Package/period-search
  SECTION:=net
  CATEGORY:=Network
	TITLE:=Asteroids@home Period Search custom BOINC app (dev)
	URL:=https://github.com/AsteroidsAtHome/PeriodSearch/tree/dev
  DEPENDS:=+boinc +libstdcpp
endef

define Package/period-search/description
  Builds the ARM64 OpenWrt Period Search executable from Asteroids@home
	dev branch snapshot and installs files for anonymous-platform usage in BOINC.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	python3 -c 'from pathlib import Path; p = Path("$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/gauss_errc_asimd.cpp"); s = p.read_text(); start = s.find("#if defined __GNUG__ && !defined __clang__\n"); end = s.find("#endif\n", start); p.write_text(s if start == -1 or end == -1 else s[:start] + s[end + len("#endif\n"):])'
endef

define Build/Configure
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/build
	$(TARGET_CXX) \
		$(TARGET_CXXFLAGS) \
		$(TARGET_CPPFLAGS) \
		-std=gnu++2a -O3 -DARM64 \
		-I$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search \
		-I$(PKG_BUILD_DIR)/period_search_common \
		-I$(STAGING_DIR)/usr/include/boinc \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/areanorm.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/blmatrix.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/bright.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/bright_asimd.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/conv.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/conv_asimd.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/covsrt.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/CpuInfoArm64.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/curv.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/curv_asimd.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/ellfit.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/EnumHelpers.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/gauss_errc.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/gauss_errc_asimd.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/lubksb.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/ludcmp.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/matrix.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/mrqcof.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/mrqcof_asimd.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/mrqmin.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/phasec.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/sphfunc.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/systeminfo.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/trifac.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/VersionInfo.cpp \
		$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/period_search_BOINC.cpp \
		$(PKG_BUILD_DIR)/period_search_common/arrayHelpers.cpp \
		$(PKG_BUILD_DIR)/period_search_common/dot_product.cpp \
		$(PKG_BUILD_DIR)/period_search_common/LcHelpers.cpp \
		$(TARGET_LDFLAGS) -Wl,--as-needed \
		-L$(STAGING_DIR)/usr/lib \
		-lboinc_api -lboinc -lm -lpthread \
		-o $(PKG_BUILD_DIR)/build/period_search_openwrt_aarch64
endef

define Package/period-search/install
	$(INSTALL_DIR) $(1)/usr/share/boinc/custom-apps/period-search
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/period_search_openwrt_aarch64 $(1)/usr/share/boinc/custom-apps/period-search/
	$(INSTALL_DATA) ./files/app_info.xml $(1)/usr/share/boinc/custom-apps/period-search/
endef

$(eval $(call BuildPackage,period-search))
