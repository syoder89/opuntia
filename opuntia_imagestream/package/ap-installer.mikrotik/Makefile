include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-wifi-ap-installer-mikrotik
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-wifi-ap-installer-mikrotik
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream WIFI AP install script
endef

define Package/imagestream-wifi-ap-installer-mikrotik/description
  ImageStream WIFI AP install script
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-wifi-ap-installer-mikrotik/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-wifi-ap-installer-mikrotik))
