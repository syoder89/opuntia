# stats

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=stats
PKG_VERSION:=3.5
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/stats
  SECTION:=net
  CATEGORY:=ImageStream
  TITLE:=Stats
  DEPENDS:=+libncurses
  DEFAULT:=y
endef

define Package/stats/description
ImageStream's Network Stats Utility
endef

define	Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/stats/install
	mkdir -p $(1)/bin
	$(CP) -a $(PKG_BUILD_DIR)/src/stats $(1)/bin
endef

$(eval $(call BuildPackage,stats))
