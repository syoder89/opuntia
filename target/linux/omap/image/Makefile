#
# Copyright (C) 2012-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE=128k
JFFS2OPTS += --little-endian --pagesize=0x800 --no-cleanmarkers --pad

FAT32_SIZE=65536


define Image/BuildKernel
	cp $(KDIR)/zImage $(BIN_DIR)/opuntia-$(BOARD)-zImage
	-mkdir $(BIN_DIR)/dtbs
	-$(CP) $(LINUX_DIR)/arch/arm/boot/dts/am335x*.dtb $(BIN_DIR)/dtbs/
	-$(CP) $(LINUX_DIR)/arch/arm/boot/dts/omap3*.dtb $(BIN_DIR)/dtbs/
	-$(CP) $(LINUX_DIR)/arch/arm/boot/dts/omap4*.dtb $(BIN_DIR)/dtbs/
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	rm -f $(KDIR)/boot.img

	mkdosfs $(KDIR)/boot.img -C $(FAT32_SIZE)
	mcopy -i $(KDIR)/boot.img $(BIN_DIR)/dtbs/am335x-boneblack.dtb ::
	mcopy -i $(KDIR)/boot.img $(KDIR)/zImage ::
	mcopy -i $(KDIR)/boot.img $(BIN_DIR)/uboot-omap-am335x_evm/MLO ::
	mcopy -i $(KDIR)/boot.img $(BIN_DIR)/uboot-omap-am335x_evm/u-boot.img ::
	mcopy -i $(KDIR)/boot.img uEnv.txt ::

	./gen_rpi_sdcard_img.sh $(BIN_DIR)/$(IMG_PREFIX)-sdcard-vfat-$(1).img $(KDIR)/boot.img $(KDIR)/root.$(1) 64 $(CONFIG_TARGET_ROOTFS_PARTSIZE)
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=65536 conv=sync
endef

define Image/Build/jffs2-128k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=131072 conv=sync
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=131072 conv=sync
endef

$(eval $(call BuildImage))
