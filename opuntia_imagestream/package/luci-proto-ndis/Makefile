#
# Copyright (C) 2007-2013 OpenWrt.org
# Copyright (C) 2010 Vertical Communications
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-proto-ndis
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_MAINTAINER:=Scott Yoder
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)-$(PKG_RELEASE)

include $(INCLUDE_DIR)/package.mk

define Package/luci-proto-ndis/Default
  VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
  URL:=http://openwrt.org/
  MAINTAINER:=Scott Yoder
endef

define Package/luci-proto-ndis
$(call Package/luci-proto-ndis/Default)
  SECTION:=net
  CATEGORY:=ImageStream
  TITLE:=Support for ndis
  DEFAULT:=y
endef

define Package/luci-proto-ndis/description
 This package contains LuCI support for ndis
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile/Default
endef

Build/Compile = $(Build/Compile/Default)

define Package/luci-proto-ndis/install
	$(INSTALL_DIR) $(1)/usr
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/network
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi/admin_network
	$(INSTALL_DATA) ./files/usr/lib/lua/luci/model/network/proto_ndis.lua $(1)/usr/lib/lua/luci/model/network/
	$(INSTALL_DATA) ./files/usr/lib/lua/luci/model/cbi/admin_network/proto_ndis.lua $(1)/usr/lib/lua/luci/model/cbi/admin_network/
endef

$(eval $(call BuildPackage,luci-proto-ndis))
