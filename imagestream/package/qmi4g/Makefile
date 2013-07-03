#
# Copyright (C) 2012 Lanedo GmbH <aleksander@lanedo.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=qmi4g
PKG_VERSION:=0.0.1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/qmi4g
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+kmod-usb-net-qmi-wwan +libqmi
  TITLE:=QMI-based net interface configuration support
  MAINTAINER:=Aleksander Morgado <aleksander@lanedo.com>
  PKGARCH:=all
endef

define Package/qmi4g/description
Provides support for bringing up QMI-based WWAN interfaces.
This package enables a new 'qmi4g' network protocol.
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/qmi4g/install
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_DATA) ./files/qmi4g.sh $(1)/lib/network/qmi4g.sh
endef

$(eval $(call BuildPackage,qmi4g))
