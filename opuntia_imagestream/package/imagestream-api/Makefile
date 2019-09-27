include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-api
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=net
  CATEGORY:=ImageStream
  TITLE:=ImageStream Management REST API
  DEPENDS:=+snmpd +php7 +php7-cli
  DEFAULT:=y
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
