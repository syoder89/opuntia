include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-wifi-ap-bootstrap
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-wifi-ap-bootstrap
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream WIFI AP Cloud bootstrap script
endef

define Package/imagestream-wifi-ap-bootstrap/description
  ImageStream WIFI AP Cloud bootstrap script
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-wifi-ap-bootstrap/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-wifi-ap-bootstrap))
