# wifidump

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/package.mk

PKG_NAME:=wifidump
PKG_VERSION:=1.0
PKG_RELEASE:=1

define Package/wifidump
  SECTION:=net
  CATEGORY:=ImageStream
  TITLE:=WifiDump
  DEPENDS:=+libncurses
  DEFAULT:=n
endef

define Package/wifidump/description
ImageStream's WiFi dump / scan Utility
endef

define	Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/wifidump/install
	mkdir -p $(1)/bin
	$(CP) -a $(PKG_BUILD_DIR)/src/wifidump $(1)/bin
endef

$(eval $(call BuildPackage,wifidump))
