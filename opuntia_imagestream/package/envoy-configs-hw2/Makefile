include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-envoy-configs-hw2
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-envoy-configs-hw2
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream Envoy default configs
endef

define Package/imagestream-envoy-configs-hw2/description
  ImageStream Envoy default configuration
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-envoy-configs-hw2/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-envoy-configs-hw2))
