#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=liba52
PKG_VERSION:=0.7.4
PKG_RELEASE:=1

PKG_SOURCE:=a52dec-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://liba52.sourceforge.net/files/
PKG_MD5SUM:=caa9f5bc44232dc8aeea773fea56be80
PKG_SOURCE_SUBDIR:=a52dec-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/a52dec-$(PKG_VERSION)

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/liba52
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=AC-3/ATSC A52 decoding library
  URL:=http://liba52.sourceforge.net/
  MAINTAINER:=Michel Lespinasse <walken@zoy.org> 
endef

define Package/liba52/decription
  Liba52 is a library for decoding ATSC A52 audio streams
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/a52dec \
			$(1)/usr/include/
endef

define Package/liba52/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/liba52.a \
			$(1)/usr/lib/
endef

$(eval $(call BuildPackage,liba52))
