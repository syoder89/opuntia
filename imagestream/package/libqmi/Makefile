#
# Copyright (C) 2012 Aleksander Morgado <aleksander@lanedo.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libqmi
PKG_VERSION:=1.4.0
PKG_RELEASE:=1

PKG_SOURCE:=libqmi-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://cgit.freedesktop.org/libqmi/snapshot/

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/libqmi
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+glib2 +zlib
  TITLE:=libqmi
  URL:=http://www.lanedo.com/~aleksander
endef

define Package/libqmi/description
  Helper library and utils to talk to QMI enabled modems
endef

CONFIGURE_VARS+= CFLAGS="$$$$CFLAGS -DMESSAGE_ENABLE_TRACE"

#define Build/Configure
#	$(PKG_BUILD_DIR)/autogen.sh
#	$(call Build/Configure/Default,)
#endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/libqmi-glib \
		$(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.{so*,a,la} \
		$(1)/usr/lib/
endef

define Package/libqmi/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.so* \
		$(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/qmicli $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/qmi-network $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libqmi))
