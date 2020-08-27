include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-ap-configs.ap2100.v3
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-ap-configs.ap2100.v3
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream AP2100 Default Configuration
endef

define Package/imagestream-ap-configs.ap2100.v3/description
  ImageStream AP2100 Default Configuration
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-ap-configs.ap2100.v3/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-ap-configs.ap2100.v3))
