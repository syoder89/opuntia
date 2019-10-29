include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-wifi-ap-configs.ap1000
PKG_VERSION:=2.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-wifi-ap-configs.ap1000
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream WIFI AP1000 Default Configuration
endef

define Package/imagestream-wifi-ap-configs.ap1000/description
  ImageStream WIFI AP1000 Default Configuration
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-wifi-ap-configs.ap1000/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-wifi-ap-configs.ap1000))
