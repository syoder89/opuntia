include $(TOPDIR)/rules.mk

PKG_NAME:=ktt30-graphics
PKG_VERSION:=1.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/ktt30-graphics
  SECTION:=multimedia
  CATEGORY:=ImageStream
  TITLE:=ktt30-graphics
  DEFAULT:=y
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/ktt30-graphics/install
	$(CP) -a ./files/* $(1)/ 2>/dev/null || true
endef

$(eval $(call BuildPackage,ktt30-graphics))

