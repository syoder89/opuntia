include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-envoy-configs
PKG_VERSION:=2.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-envoy-configs
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream Envoy Default Configuration
endef

define Package/imagestream-envoy-configs/description
  ImageStream Envoy LTE Default Configuration
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-envoy-configs/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-envoy-configs))
