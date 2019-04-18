include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-envoy-installer
PKG_VERSION:=2.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-envoy-installer
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream Envoy install script
endef

define Package/imagestream-envoy-installer/description
  ImageStream Envoy install script
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-envoy-installer/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-envoy-installer))
