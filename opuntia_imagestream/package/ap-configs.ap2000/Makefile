include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-wifi-ap-configs.ap2000
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-wifi-ap-configs.ap2000
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream WIFI AP2000 Default Configuration
endef

define Package/imagestream-wifi-ap-configs.ap2000/description
  ImageStream WIFI AP2000 Default Configuration
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-wifi-ap-configs.ap2000/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-wifi-ap-configs.ap2000))
