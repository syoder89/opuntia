include $(TOPDIR)/rules.mk

PKG_NAME:=uqmi
PKG_VERSION:=2013-06-23
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/uqmi.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=35201737484008ac802649cbe9fb5f7ab38a4ad2
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/uqmi
  SECTION:=net
  CATEGORY:=ImageStream
  DEPENDS:=+ubusd +ubus
  TITLE:=QMI utility
  DEFAULT:=y
endef

TARGET_CFLAGS += -ffunction-sections -fdata-sections -DDEBUG -DDEBUG_PACKET
TARGET_LDFLAGS += -Wl,--gc-sections

define Package/uqmi/install
	$(INSTALL_DIR) $(1)/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/uqmi $(1)/sbin/
endef

$(eval $(call BuildPackage,uqmi))
