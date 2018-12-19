include $(TOPDIR)/rules.mk

PKG_NAME:=man3g
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/man3g
  SECTION:=net
  CATEGORY:=ImageStream
  DEPENDS:=
  TITLE:=Cellular modem support utility
endef

define Package/man3g/description
  Cellular modem support utility
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/man3g/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,man3g))
