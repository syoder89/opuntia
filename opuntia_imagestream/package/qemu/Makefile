#
# Copyright (C) 2009, 2010 Stefan Monnier
#
# This is free software, licensed under the GNU General Public License v3+.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
 
PKG_NAME:=qemu
PKG_VERSION:=2.0.0
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://wiki.qemu.org/download/
#PKG_REV:=f5c85c229b780bc39268845b6f365fc1

PKG_FIXUP:=autoreconf
# libtool


PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/qemu
  $(call Package/qemu/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=KVM Hypervisor
  #DEPENDS:=+zlib +libpthread +librt +libc +libgcc +libc +kmod-kvm-intel +kmod-kvm-amd +glib2
  DEPENDS:=+zlib +libpthread +librt +libc +libgcc +libc +glib2 +libcurl +libintl
endef

define Package/qemu/description
  This is qemu, the Hypervisor
endef

#EXTRA_CPPFLAGS:=\
#        -I$(ICONV_PREFIX)/include \
#        -I$(INTL_PREFIX)/include \

EXTRA_LDFLAGS:=\
	-L$(STAGING_DIR)/usr/lib/libintl-full/lib \
	-L$(STAGING_DIR)/usr/lib/gettext-full/lib \
	-L$(STAGING_DIR)/usr/lib/libiconv-full/lib \
	-lrt -lm \
	-Wl,-rpath-link=$(STAGING_DIR)/lib:$(STAGING_DIR)/usr/lib:$(STAGING_DIR)/usr/lib/libintl-full/lib:$(STAGING_DIR)/usr/lib/libiconv-full/lib


# TARGET_LDFLAGS += $(FPIC)

TARGET_LDFLAGS += -L$(STAGING_DIR)/usr/lib/libintl-full/lib -L$(STAGING_DIR)/usr/lib/libiconv-full/lib -L$(STAGING_DIR)/usr/lib -lc -lrt -lavcodec -lm

MAKE_FLAGS += \
	CPPFLAGS="$(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDPFLAGS) $(EXTRA_LDFLAGS)"


CONFIGURE_ARGS= \
        --cross-prefix="$(CROSS_PREFIX)" \
       --disable-linux-aio \
       --disable-pie \
       --disable-fdt \
       --disable-attr \
        --enable-kvm  \
        --disable-werror \
        --target-list=i386-softmmu,x86_64-softmmu \
        --prefix=/usr \
        --sysconfdir=/etc \
        --disable-docs \
        --disable-brlapi \
        --disable-vnc-sasl \
        --extra-ldflags="$(EXTRA_LDFLAGS)" \
        --with-coroutine=gthread \
        --disable-sdl \
        --disable-bluez

define Package/qemu/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/qemu* $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/share/qemu
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/share/qemu/*.bin $(1)/usr/share/qemu/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/share/qemu/*.rom $(1)/usr/share/qemu/
	$(INSTALL_DIR) $(1)/usr/share/qemu/keymaps
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/share/qemu/keymaps/* $(1)/usr/share/qemu/keymaps/
	$(INSTALL_DIR) $(1)/etc/qemu
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/etc/qemu/target*  $(1)/etc/qemu/
endef

define Package/qemu/conffiles
/etc/lvm/lvm.conf
endef

$(eval $(call BuildPackage,qemu))
