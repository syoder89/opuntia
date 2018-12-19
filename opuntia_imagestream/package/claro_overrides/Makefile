include $(TOPDIR)/rules.mk

PKG_NAME:=claro-overrides
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@claro.com>

include $(INCLUDE_DIR)/package.mk

define Package/claro-overrides
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=Claro file/script modifications
  DEFAULT:=y
endef

define Package/claro-overrides/description
  ImageStream file/script modifications
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/claro-overrides/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,claro-overrides))
