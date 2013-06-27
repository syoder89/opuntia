#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

ROOTPART:=$(call qstrip,$(CONFIG_TARGET_ROOTFS_PARTNAME))

define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/uImage $(KDIR)/uImage
endef

define Image/BuildKernel
	mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n 'Boot Image' -d boot.script $(BIN_DIR)/boot.scr
	$(INSTALL_DIR) $(KDIR)/root.kernel
	cp $(KDIR)/uImage $(KDIR)/root.kernel/uImage
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/opuntia-$(BOARD)-$(1).img bs=65536 conv=sync
endef

define Image/Build/jffs2-128k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/opuntia-$(BOARD)-$(1).img bs=131072 conv=sync
endef

define Image/cmdline/squashfs
	block2mtd.block2mtd=$(ROOTPART),65536,rootfs root=/dev/mtdblock0 rootfstype=squashfs rootwait
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/opuntia-$(BOARD)-$(1).img bs=131072 conv=sync
	PADDING="$(CONFIG_TARGET_IMAGES_PAD)" PATH="$(TARGET_PATH)" ./gen_image_generic.sh \
		$(BIN_DIR)/opuntia-$(BOARD)-combined-$(1).img \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) $(KDIR)/root.kernel \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) $(KDIR)/root.$(1) \
		256
endef

$(eval $(call BuildImage))
