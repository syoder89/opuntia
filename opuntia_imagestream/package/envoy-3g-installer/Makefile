include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-envoy-3g-installer
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-envoy-3g-installer
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream Envoy install script
endef

define Package/imagestream-envoy-3g-installer/description
  ImageStream Envoy 3G install script
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-envoy-3g-installer/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-envoy-3g-installer))
