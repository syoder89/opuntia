#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=modemmanager
PKG_VERSION:=0.7.991

PKG_SOURCE:=ModemManager-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://cgit.freedesktop.org/ModemManager/ModemManager/snapshot/
PKG_SOURCE_SUBDIR:=ModemManager-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/ModemManager-$(PKG_VERSION)

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/modemmanager
  SECTION:=net
  CATEGORY:=Network
  TITLE:=modemmanager
  URL:=http://cgit.freedesktop.org/ModemManager/ModemManager
  DEFAULT:=y
  DEPENDS:= \
	+glib2 \
	+libqmi \
	+udev \
	+dbus \
	+kmod-usb-net-qmi-wwan \
	+kmod-usb-net-cdc-ncm \
	+kmod-usb-net-hso \
	+kmod-usb-net-rndis \
	+kmod-usb-net-sierrawireless \
	+kmod-usb-acm \
	+kmod-usb-serial-option \
	+kmod-usb-serial-qualcomm \
	+kmod-usb-serial-sierrawireless \
	+kmod-usb-serial-option \
	+kmod-usb-serial-wwan
endef

define Package/modemmanager/description
  ModemManager is a DBus-activated service which allows controlling mobile
  broadband modems.
endef

CONFIGURE_ARGS += \
	--prefix=/usr \
	--sysconfdir=/etc \
	--localstatedir=/var \
	--without-docs \
	--without-tests \
	--without-polkit

define Build/Configure
	( cd "$(PKG_BUILD_DIR)"; ./autogen.sh )
	$(call Build/Configure/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/ModemManager
	$(CP) $(PKG_INSTALL_DIR)/usr/include/ModemManager/*.h $(1)/usr/include/ModemManager
	$(INSTALL_DIR) $(1)/usr/include/libmm-glib
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libmm-glib/*.h $(1)/usr/include/libmm-glib
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmm-glib.so* $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/ModemManager.pc $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/mm-glib.pc $(1)/usr/lib/pkgconfig
endef

define Package/modemmanager/install
	$(INSTALL_DIR) $(1)/lib/udev/rules.d
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/lib/udev/rules.d/*.rules $(1)/lib/udev/rules.d

	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/ModemManager $(1)/usr/sbin

	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/mmcli $(1)/usr/bin

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmm-glib.so* $(1)/usr/lib

	$(INSTALL_DIR) $(1)/usr/lib/ModemManager
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/ModemManager/libmm-plugin-*.so* $(1)/usr/lib/ModemManager
endef

$(eval $(call BuildPackage,modemmanager))
