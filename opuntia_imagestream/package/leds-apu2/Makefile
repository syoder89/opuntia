#
# Copyright (C) 2017 Chris Blake <chrisrblake93@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=leds-apu2
PKG_RELEASE:=1

PKG_MAINTAINER:=Scott Yoder <syoder@imagestream.com>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/leds-apu2
  SUBMENU:=LED modules
  TITLE:= ImageStream APU2 / PoE LED support
  DEPENDS:= @GPIO_SUPPORT @TARGET_x86
  FILES:=$(PKG_BUILD_DIR)/leds-apu2.ko
  AUTOLOAD:=$(call AutoLoad,41,leds-apu2,1)
  KCONFIG:=
endef

define KernelPackage/leds-apu2/description
  Driver for the ImageStream AP2100 PoE board LEDs.
endef

EXTRA_KCONFIG:= \
	CONFIG_LEDS_APU2=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	M="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Compile
	 $(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,leds-apu2))

