include $(TOPDIR)/rules.mk

PKG_NAME:=netifd-qmi
PKG_VERSION:=1.0
PKG_RELEASE:=3

PKG_MAINTAINER:=André Valentin <avalentin@marcant.net>

include $(INCLUDE_DIR)/package.mk

define Package/netifd-qmi
  SECTION:=net
  CATEGORY:=ImageStream
  DEPENDS:=+libqmi +iputils-arping @(PACKAGE_imagestream-uqmi)
  TITLE:=QMI support for netifd
endef

define Package/netifd-qmi/description
  Helper scripts to enable netifd to manage qmi interfaces
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/netifd-qmi/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true

endef

$(eval $(call BuildPackage,netifd-qmi))
