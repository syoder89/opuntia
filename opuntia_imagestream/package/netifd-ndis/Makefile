include $(TOPDIR)/rules.mk

PKG_NAME:=netifd-ndis
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/netifd-ndis
  SECTION:=net
  CATEGORY:=ImageStream
  DEPENDS:=+comgt +man3g
  TITLE:=NDIS support for netifd
endef

define Package/netifd-ndis/description
  Helper scripts to enable netifd to manage ndis interfaces
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/netifd-ndis/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,netifd-ndis))
