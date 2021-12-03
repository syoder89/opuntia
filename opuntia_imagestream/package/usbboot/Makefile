#
# Copyright (C) 2021-2021 IS ImageStream Internet Solutions, Inc.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=usbboot
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_SOURCE_DATE:=2021-11-19

PKG_SOURCE:=$(PKG_NAME)-$(PKG_SOURCE_DATE).tar.gz
PKG_SOURCE_VERSION:=69fa6083b75185f2add0ac8d9e0e9ddbd957fcbd
PKG_SOURCE_URL:=https://codeload.github.com/imagestream/usbboot/tar.gz/$(PKG_SOURCE_VERSION)?
PKG_HASH:=080065e5dd14ef09072b956821119388dc67cd3b24d68f69738ad563f3f9fee5

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_SOURCE_VERSION)

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/usbboot/default
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Utility for booting / flashing Raspberry PI systems via USB
  DEPENDS:=+libusb
  URL:=https://github.com/raspberrypi/usbboot
endef

define Package/usbboot
  $(Package/usbboot/default)
endef

define Package/usbboot/install
	$(INSTALL_DIR) $(1)/usr/share/usbboot
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/rpiboot $(1)/usr/share/usbboot/
endef

$(eval $(call BuildPackage,usbboot))

