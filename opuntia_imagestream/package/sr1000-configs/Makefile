include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-sr1000-configs
PKG_VERSION:=2.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Joshua Snyder <josh@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-sr1000-configs
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream Samurai Default Configuration
endef

define Package/imagestream-sr1000-configs/description
  ImageStream Samurai Default Configuration
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-sr1000-configs/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-sr1000-configs))
