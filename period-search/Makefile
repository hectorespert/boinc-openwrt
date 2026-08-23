# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=period-search
PKG_VERSION:=20260814
PKG_RELEASE:=2

PKG_SOURCE:=dev.tar.gz
PKG_SOURCE_URL:=https://github.com/AsteroidsAtHome/PeriodSearch/archive/refs/heads
PKG_HASH:=534202a0c6bbfa739f806c0df6b36b37d7f9dbbfb5ce8bb97a2894bd16d8d7b6
PKG_BUILD_DIR:=$(BUILD_DIR)/PeriodSearch-dev

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
	URL:=https://github.com/AsteroidsAtHome/PeriodSearch/tree/dev
  DEPENDS:=+boinc +libstdcpp
endef

define Package/period-search/description
	Builds the OpenWrt Period Search executable from Asteroids@home
	dev branch snapshot and installs files for anonymous-platform usage in BOINC.
endef

define Build/Configure
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/build
	$(CP) ./files/FallbackCpuInfo.cpp $(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/FallbackCpuInfo.cpp
	if [ "$(ARCH)" = "aarch64" ]; then \
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
			-o $(PKG_BUILD_DIR)/build/period_search_openwrt_$(ARCH_PACKAGES); \
	elif [ "$(ARCH)" = "x86_64" ]; then \
		$(TARGET_CXX) \
			$(TARGET_CXXFLAGS) \
			$(TARGET_CPPFLAGS) \
			-std=gnu++2a -O3 \
			-I$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search \
			-I$(PKG_BUILD_DIR)/period_search_common \
			-I$(STAGING_DIR)/usr/include/boinc \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/areanorm.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/blmatrix.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/bright.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/bright_avx.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/bright_avx512.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/bright_fma.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/bright_sse2.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/bright_sse3.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/conv.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/conv_avx.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/conv_avx512.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/conv_fma.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/conv_sse2.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/conv_sse3.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/covsrt.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/CpuInfo.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/curv.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/curv_avx.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/curv_avx512.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/curv_fma.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/curv_sse2.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/curv_sse3.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/ellfit.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/EnumHelpers.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/gauss_errc.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/gauss_errc_avx.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/gauss_errc_avx512.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/gauss_errc_fma.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/gauss_errc_sse2.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/gauss_errc_sse3.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/lubksb.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/ludcmp.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/matrix.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/mrqcof.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/mrqcof_avx.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/mrqcof_avx512.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/mrqcof_fma.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/mrqcof_sse2.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/mrqcof_sse3.cpp \
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
			-o $(PKG_BUILD_DIR)/build/period_search_openwrt_$(ARCH_PACKAGES); \
	else \
	$(TARGET_CXX) \
		$(TARGET_CXXFLAGS) \
		$(TARGET_CPPFLAGS) \
		-std=gnu++2a -O3 \
		-I$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search \
		-I$(PKG_BUILD_DIR)/period_search_common \
		-I$(STAGING_DIR)/usr/include/boinc \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/areanorm.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/blmatrix.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/bright.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/conv.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/covsrt.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/FallbackCpuInfo.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/curv.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/ellfit.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/EnumHelpers.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/gauss_errc.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/lubksb.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/ludcmp.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/matrix.cpp \
			$(PKG_BUILD_DIR)/period_search_optimization_simd/period_search/mrqcof.cpp \
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
			-o $(PKG_BUILD_DIR)/build/period_search_openwrt_$(ARCH_PACKAGES); \
	fi
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
