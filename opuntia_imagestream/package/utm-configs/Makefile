include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-utm-configs
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-utm-configs
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream UTM default configs
endef

define Package/imagestream-utm-configs/description
  ImageStream UTM default configuration
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-utm-configs/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-utm-configs))
