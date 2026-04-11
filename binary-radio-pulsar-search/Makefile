# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=binary-radio-pulsar-search
PKG_VERSION:=20260411
PKG_RELEASE:=1

PKG_SOURCE:=brp-src-release.zip
PKG_SOURCE_URL:=https://einsteinathome.org
PKG_HASH:=4355f84421b8806dd10b9050a578de1fc9c226997453e44ebe7b2177326a468c
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_LICENSE:=GPL-2.0-or-later
PKG_BUILD_PARALLEL:=1
PKG_BUILD_DEPENDS:=boinc fftw3 gsl libxml2 zlib
BOINC_API_VERSION:=$(shell awk '/^#define BOINC_MAJOR_VERSION/ {maj=$$3} /^#define BOINC_MINOR_VERSION/ {min=$$3} /^#define BOINC_RELEASE/ {rel=$$3} END { if (maj != "" && min != "" && rel != "") printf "%s.%s.%s", maj, min, rel; else printf "7.21.0" }' $(STAGING_DIR)/usr/include/boinc/version.h 2>/dev/null)

include $(INCLUDE_DIR)/package.mk

define Package/binary-radio-pulsar-search
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Einstein@Home Binary Radio Pulsar Search custom BOINC app
  URL:=https://einsteinathome.org/es/source-license
	DEPENDS:=+boinc +libstdcpp +zlib
endef

define Package/binary-radio-pulsar-search/description
  Builds the Einstein@Home Binary Radio Pulsar Search CPU application
  and installs anonymous-platform files for BOINC.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cd $(PKG_BUILD_DIR) && unzip -o $(DL_DIR)/brp-src-release.zip
	$(CP) ./files/erp_git_version.h $(PKG_BUILD_DIR)/src/
	$(SED) 's|fprintf(output, levelBuffer)|fprintf(output, "%s", levelBuffer)|g' $(PKG_BUILD_DIR)/src/erp_utilities.cpp
endef

define Build/Configure
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/build
	$(TARGET_CXX) \
		$(TARGET_CXXFLAGS) \
		$(TARGET_CPPFLAGS) \
		-O3 -Wall -DNDEBUG \
		-Wno-error -Wno-format-security \
		-DHAVE_INLINE -DBOINCIFIED -DUSE_CPU_RESAMP -DUSE_FFTW_FFT -DBRP_FFT_INPLACE -DLOGLEVEL=info \
		-I$(PKG_BUILD_DIR)/src \
		-I$(STAGING_DIR)/usr/include \
		-I$(STAGING_DIR)/usr/include/libxml2 \
		-I$(STAGING_DIR)/usr/include/boinc \
		$(PKG_BUILD_DIR)/src/demod_binary.c \
		$(PKG_BUILD_DIR)/src/demod_binary_resamp_cpu.c \
		$(PKG_BUILD_DIR)/src/demod_binary_fft_fftw.c \
		$(PKG_BUILD_DIR)/src/demod_binary_hs_cpu.c \
		$(PKG_BUILD_DIR)/src/rngmed.c \
		$(PKG_BUILD_DIR)/src/erp_boinc_ipc.cpp \
		$(PKG_BUILD_DIR)/src/erp_getopt.c \
		$(PKG_BUILD_DIR)/src/erp_getopt1.c \
		$(PKG_BUILD_DIR)/src/erp_utilities.cpp \
		$(PKG_BUILD_DIR)/src/hs_common.c \
		$(PKG_BUILD_DIR)/src/erp_boinc_wrapper.cpp \
		$(TARGET_LDFLAGS) -Wl,--as-needed \
		-L$(STAGING_DIR)/usr/lib \
		-lboinc_api -lboinc \
		-Wl,-Bstatic -lfftw3f -lgsl -lgslcblas -lxml2 -Wl,-Bdynamic \
		-lz -ldl -lm -lpthread \
		-o $(PKG_BUILD_DIR)/build/einsteinbinary_openwrt_$(ARCH_PACKAGES)
endef

define Package/binary-radio-pulsar-search/install
	$(INSTALL_DIR) $(1)/opt/boinc/projects/einstein.phys.uwm.edu
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/einsteinbinary_openwrt_$(ARCH_PACKAGES) $(1)/opt/boinc/projects/einstein.phys.uwm.edu/
	$(CP) ./files/app_info.xml $(PKG_BUILD_DIR)/app_info.xml
	$(SED) 's|einsteinbinary_openwrt_aarch64|einsteinbinary_openwrt_$(ARCH_PACKAGES)|g' $(PKG_BUILD_DIR)/app_info.xml
	$(SED) 's|@BOINC_API_VERSION@|$(BOINC_API_VERSION)|g' $(PKG_BUILD_DIR)/app_info.xml
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/app_info.xml $(1)/opt/boinc/projects/einstein.phys.uwm.edu/
endef

$(eval $(call BuildPackage,binary-radio-pulsar-search))
