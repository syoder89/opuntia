#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=ktt30
BOARDNAME:=Kontron KTT30
FEATURES:=squashfs

LINUX_VERSION:=3.9.5
#LINUX_VERSION:=3.1.10

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += wpad-mini kmod-ath5k

define Target/Description
	Build firmware images for Tegra3 SoC boards
endef

$(eval $(call BuildTarget))
