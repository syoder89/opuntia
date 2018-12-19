include $(TOPDIR)/rules.mk

PKG_NAME:=compex-production
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/compex-production
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=Files needed for Compex production automation
  DEFAULT:=y
endef

define Package/compex-production/description
  Files needed for Compex production automation
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/compex-production/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,compex-production))
