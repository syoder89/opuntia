include $(TOPDIR)/rules.mk

PKG_NAME:=kismet-remotecap
PKG_VERSION:=2020-02-20
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/imagestream/kismet.git
PKG_SOURCE_VERSION:=ac98b69b239e2e7ad15e17e9a93659b2d33d03cf
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

PKG_USE_MIPS16:=0

HOST_BUILD_DEPENDS:=protobuf/host 

define Package/kismet-remotecap
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Kismet Remote Capture
  URL:=https://www.kismetwireless.net/
  DEPENDS:=+libpthread +libpcap +libpcre +libmicrohttpd +libnl +libcap +libstdcpp +libncurses +libsqlite3 +zlib +protobuf-lite +libprotobuf-c +libdw +libusb-1.0
  SUBMENU:=wireless
endef

define Package/kismet-remotecap/description
	Kismet Wireless Remote Capture Tools.
endef

define Build/Configure
  $(call Build/Configure/Default,--with-linux-headers=$(LINUX_DIR))
endef

CONFIGURE_ARGS += \
	--sysconfdir=/etc/kismet \
	--bindir=/usr/bin \
	--disable-python-tools \
	--with-protoc=$(STAGING_DIR_HOSTPKG)/bin/protoc \
	--enable-capture-tools-only

# Override libs to force lpthread because we don't detect it properly on openwrt
CONFIGURE_VARS += \
	CXXFLAGS="$$$$CXXFLAGS"  \
	LIBS="-lm -lpthread" 

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LD="$(TARGET_CXX)" \
		datasources
endef

define Package/kismet-remotecap/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/capture_linux_wifi/kismet_cap_linux_wifi $(1)/usr/bin/
endef

$(eval $(call BuildPackage,kismet-remotecap))

