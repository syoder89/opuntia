# Gobi3k

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gobi3k
PKG_VERSION:=2012-06-18-1054
PKG_RELEASE:=1
GOBI_FIRMWARE="Gobi3kfirmware.tar.bz2"

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
#PKG_SOURCE_URL:=http://install1.imagestream.com/
#PKG_MD5SUM:=9768fa3595b6a591bc762f3c8e53a852

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/gobi3k
  SECTION:=net
  CATEGORY:=ImageStream
  TITLE:=Gobi3k
#  FILES:=$(PKG_BUILD_DIR)/GobiNet/GobiNet.ko $(PKG_BUILD_DIR)/GobiSerial/GobiSerial.ko
#  AUTOLOAD:=$(call AutoLoad,61,GobiSerial GobiNet)
  DEPENDS:=$(ICONV_DEPENDS) +libpthread +librt +libstdcpp +glib2 +chat +kmod-usb-serial +kmod-usb-net
  DEFAULT:=n
endef

define Package/gobi3k/description
Gobi3k provides the drivers for the Option GTM689W 3G wireless modem.
endef

ENVARGS:=export AS=i486-openwrt-linux-as && export CC=i486-openwrt-linux-gcc && export CPP=i486-openwrt-linux-cpp && export RANLIB=i486-openwrt-linux-ranlib && export AR=i486-openwrt-linux-ar && export CXX=i486-openwrt-linux-g++ && export LD=i486-openwrt-linux-ld

define	Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
#	$(MAKE) CROSS_COMPILE=i486-openwrt-linux- -C $(PKG_BUILD_DIR)/GobiNet KARCH=$(LINUX_KARCH) KDIR=${LINUX_DIR}
#	$(MAKE) CROSS_COMPILE=i486-openwrt-linux- -C $(PKG_BUILD_DIR)/GobiSerial KARCH=$(LINUX_KARCH) KDIR=${LINUX_DIR}
	$(ENVARGS) && $(MAKE) -C $(PKG_BUILD_DIR)/GobiAPI/GobiConnectionMgmt
	$(ENVARGS) && $(MAKE) -C $(PKG_BUILD_DIR)/GobiAPI/GobiImageMgmt
	$(ENVARGS) && $(MAKE) LDFLAGS="$(LDFLAGS) -lpthread" -C $(PKG_BUILD_DIR)/GobiAPI/GobiQDLService
	$(ENVARGS) && $(MAKE) CXXFLAGS="$(TARGET_CFLAGS) $(ICONV_CPPFLAGS)" LXXFLAGS="$(TARGET_LDFLAGS) -lpthread" -C $(PKG_BUILD_DIR)/util
endef

define Package/gobi3k/install
#	$(MAKE) CROSS_COMPILE=i486-openwrt-linux- -C $(PKG_BUILD_DIR)/GobiNet KDIR=${LINUX_DIR} OUTPUTDIR=$(1)/lib/modules/$(LINUX_VERSION)/ install
#	$(MAKE) CROSS_COMPILE=i486-openwrt-linux- -C $(PKG_BUILD_DIR)/GobiSerial KDIR=${LINUX_DIR} OUTPUTDIR=$(1)/lib/modules/$(LINUX_VERSION)/ install
	$(MAKE) CROSS_COMPILE=i486-openwrt-linux- -C $(PKG_BUILD_DIR)/GobiAPI/GobiConnectionMgmt DESTDIR=$(1) install
	$(MAKE) CROSS_COMPILE=i486-openwrt-linux- -C $(PKG_BUILD_DIR)/GobiAPI/GobiImageMgmt DESTDIR=$(1) install
	$(MAKE) CROSS_COMPILE=i486-openwrt-linux- -C $(PKG_BUILD_DIR)/GobiAPI/GobiQDLService DESTDIR=$(1) install
	$(MAKE) CROSS_COMPILE=i486-openwrt-linux- -C $(PKG_BUILD_DIR)/util DESTDIR=$(1) install
	mkdir -p $(1)/usr/lib
	mv $(1)/opt/Qualcomm/Gobi/*so $(1)/usr/lib
	cd $(1)/opt; tar xjf $(PKG_BUILD_DIR)/$(GOBI_FIRMWARE)
	$(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,gobi3k))
