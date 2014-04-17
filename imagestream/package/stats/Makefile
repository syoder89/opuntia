# stats

include $(TOPDIR)/rules.mk

PKG_NAME:=stats
PKG_VERSION:=3.5
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/stats
  SECTION:=net
  CATEGORY:=ImageStream
  TITLE:=Stats
  DEPENDS:=+libncurses +libpanel
  DEFAULT:=y
endef

PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES:=aclocal.m4 libtool.m4

define Package/stats/description
ImageStream's Network Stats Utility
endef

CONFIGURE_ARGS += \
        --prefix=/

define	Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
	cd $(PKG_BUILD_DIR) && autoreconf
	cd $(PKG_BUILD_DIR) && ./configure --prefix=/
endef

MAKE_FLAGS += \
	FPIC="$(FPIC)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \

define Package/stats/install
	mkdir -p $(1)/bin
	$(CP) -a $(PKG_BUILD_DIR)/src/stats $(1)/bin
endef

$(eval $(call BuildPackage,stats))
