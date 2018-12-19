include $(TOPDIR)/rules.mk

PKG_NAME:=hardware-watchdog
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/hardware-watchdog
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=Hardware Watchdog
  DEFAULT:=y
endef

define Package/hardware-watchdog/description
  ImageStream file/script modifications
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/hardware-watchdog/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,hardware-watchdog))
