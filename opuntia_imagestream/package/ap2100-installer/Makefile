include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-ap2100-installer
PKG_VERSION:=2.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-ap2100-installer
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream AP2100 install script
endef

define Package/imagestream-ap2100-installer/description
  ImageStream AP2100 install script
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-ap2100-installer/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-ap2100-installer))
