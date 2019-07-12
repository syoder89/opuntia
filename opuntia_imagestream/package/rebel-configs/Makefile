include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-rebel-configs
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

include $(INCLUDE_DIR)/package.mk

define Package/imagestream-rebel-configs/config
	config IS_ROUTER_FULL
		bool "Full ImageStream router configs"
		default n
endef

define Package/imagestream-rebel-configs
  SECTION:=sys
  CATEGORY:=ImageStream
  TITLE:=ImageStream Rebel Default Configuration
endef

define Package/imagestream-rebel-configs/description
  ImageStream Rebel Default Configuration
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/imagestream-rebel-configs/install
        $(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,imagestream-rebel-configs))
