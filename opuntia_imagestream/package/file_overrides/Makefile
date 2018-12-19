include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-overrides
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-overrides
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream file/script modifications
  DEFAULT:=y
endef

define Package/imagestream-overrides/description
  ImageStream file/script modifications
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-overrides/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-overrides))
