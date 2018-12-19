#
# Copyright (C) 2018 Lucian Cristian <lucian.cristian@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
PKG_NAME:=frr
PKG_VERSION:=5.0.1
PKG_RELEASE:=1
PKG_HASH:=b2d94960a248f953fef5ee34a95229ff6426ceac27f4d187179f0146275dbf14

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/FRRouting/frr/archive/
PKG_BUILD_DIR = $(BUILD_DIR)/$(PKG_NAME)-$(PKG_NAME)-$(PKG_VERSION)

PKG_CONFIG_DEPENDS:= \
	CONFIG_IPV6 \
	CONFIG_PACKAGE_frr-babeld \
	CONFIG_PACKAGE_frr-bgpd \
	CONFIG_PACKAGE_frr-eigrpd \
	CONFIG_PACKAGE_frr-isisd \
	CONFIG_PACKAGE_frr-ldpd \
	CONFIG_PACKAGE_frr-libfrr \
	CONFIG_PACKAGE_frr-nhrpd \
	CONFIG_PACKAGE_frr-ospfd \
	CONFIG_PACKAGE_frr-ospf6d \
	CONFIG_PACKAGE_frr-pbrd \
	CONFIG_PACKAGE_frr-pimd \
	CONFIG_PACKAGE_frr-ripd \
	CONFIG_PACKAGE_frr-ripngd \
	CONFIG_PACKAGE_frr-vtysh \
	CONFIG_PACKAGE_frr-watchfrr \
	CONFIG_PACKAGE_frr-zebra

PKG_BUILD_PARALLEL:=1
PKG_BUILD_DEPENDS:=python/host
PKG_FIXUP:=autoreconf
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/frr/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Routing and Redirection
  DEPENDS:=frr
  TITLE:=The FRRouting (FRR) Software Routing Suite
  URL:=https://www.frrouting.org/
  MAINTAINER:=Lucian Cristian <lucian.cristian@gmail.com>
endef

define Package/frr
  $(call Package/frr/Default)
  DEPENDS:=+librt
  MENU:=1
endef

define Package/frr/description
  FRRouting is free software that implements and manages various IPv4 and IPv6
  routing protocols.

  Currently FRRouting supports BGP4, BGP4+, OSPFv2, OSPFv3, RIPv1, RIPv2, RIPng,
  IS-IS, PIM-SM/MSDP, LDP and Babel as well as very early support for EIGRP and
  NHRP.
endef

define Package/frr-babeld
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr
  TITLE:=BABEL daemon
endef

define Package/frr-bgpd
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr
  TITLE:=BGPv4, BGPv4+, BGPv4- daemon
endef

define Package/frr-eigrpd
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr
  TITLE:=EIGRPD daemon
endef

define Package/frr-isisd
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr
  TITLE:=IS-IS daemon
endef

define Package/frr-ldpd
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr
  TITLE:=LDP daemon
endef

define Package/frr-libfrr
  $(call Package/frr/Default)
  TITLE:=zebra library
  DEPENDS+=+libjson-c
endef

define Package/frr-nhrpd
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr +libcares
  TITLE:=NHRP daemon
endef

define Package/frr-ospfd
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr
  TITLE:=OSPFv2 daemon
endef

define Package/frr-ospf6d
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr @IPV6
  TITLE:=OSPFv3 daemon
endef

define Package/frr-pbrd
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr
  TITLE:=PBRD daemon
endef

define Package/frr-pimd
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr
  TITLE:=PIM daemon
endef

define Package/frr-ripd
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr
  TITLE:=RIP daemon
endef

define Package/frr-ripngd
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr @IPV6
  TITLE:=RIPNG daemon
endef

define Package/frr-vtysh
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr +libreadline +libncurses
  TITLE:=integrated shell for frr routing software
endef

define Package/frr-watchfrr
  $(call Package/frr/Default)
  TITLE:=frr watchdog
  DEPENDS+=+frr-libfrr
  DEFAULT:=y if PACKAGE_frr
endef

define Package/frr-zebra
  $(call Package/frr/Default)
  TITLE:=Zebra daemon
  DEPENDS+=+frr-libfrr
  DEFAULT:=y if PACKAGE_frr
endef

define Package/frr-zebra/conffiles
/etc/frr/zebra.conf
endef


define Package/frr-babeld/conffiles
/etc/frr/babeld.conf
endef

define Package/frr-bgpd/conffiles
/etc/frr/bgpd.conf
endef

define Package/frr-eigrpd/conffiles
/etc/frr/eigrpd.conf
endef

define Package/frr-isisd/conffiles
/etc/frr/isisd.conf
endef

define Package/frr-ldpd/conffiles
/etc/frr/ldpd.conf
endef

define Package/frr-nhrpd/conffiles
/etc/frr/nhrpd.conf
endef

define Package/frr-ospfd/conffiles
/etc/frr/ospfd.conf
endef

define Package/frr-ospf6d/conffiles
/etc/frr/ospf6d.conf
endef

define Package/frr-pbrd/conffiles
/etc/frr/pbrd.conf
endef

define Package/frr-pimd/conffiles
/etc/frr/pimd.conf
endef

define Package/frr-ripd/conffiles
/etc/frr/ripd.conf
endef

define Package/frr-ripngd/conffiles
/etc/frr/ripngd.conf
endef

define Package/frr-vtysh/conffiles
/etc/frr/vtysh.conf
endef

define Build/Configure
    ( cd $(PKG_BUILD_DIR)/ ; \
    mkdir build/ ; \
    cd build ; \
    ../configure \
	--host="$(GNU_TARGET_NAME)" \
	--build="$(GNU_HOST_NAME)" \
	CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
	CXXFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS)" \
	HOST_CFLAGS="$(HOST_CFLAGS)" \
	HOST_LDFLAGS="$(HOST_LDFLAGS)" \
	BUILD_CPPFLAGS="$(TARGET_CPPLAGS)" \
	BUILD_CFLAGS="$(TARGET_CFLAGS)" \
	BUILD_LDFLAGS="$(TARGET_LDFLAGS)" \
	--localstatedir=/var/run/frr \
	--sysconfdir=/etc/frr/ \
	--enable-shared \
	--disable-static \
	--enable-user=root \
	--enable-group=root \
	--enable-multipath=8 \
	--with-vtysh-pager=cat \
	--disable-capabilities \
	--disable-ospfclient \
	--disable-doc \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-babeld,babeld) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-bgpd,bgpd) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-eigrpd,eigrpd) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-isisd,isisd) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-ldpd,ldpd) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-nhrpd,nhrpd) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-ospfd,ospfd) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-ospf6d,ospf6d) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-pbrd,pbrd) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-pimd,pimd) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-ripd,ripd) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-ripngd,ripngd) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-vtysh,vtysh) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-libfrr,zebra) \
)
endef

#	just speed it up, maybe not a good approach ?
NUM_CORES ?= $(shell grep -c "vendor_id" /proc/cpuinfo)

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/build -j$(NUM_CORES)
endef

define Package/frr/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/frr $(1)/usr/sbin/frr.init
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/frr.init $(1)/etc/init.d/frr
endef

define Package/frr-watchfrr/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/watchfrr/.libs/watchfrr $(1)/usr/sbin/
endef

define Package/frr-zebra/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/zebra/.libs/zebra $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/zebra.conf
endef

define Package/frr-babeld/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/babeld/.libs/babeld $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/babeld.conf
endef

define Package/frr-bgpd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/bgpd/.libs/bgpd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/bgpd.conf
endef

define Package/frr-eigrpd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/eigrpd/.libs/eigrpd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/eigrpd.conf
endef

define Package/frr-isisd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/isisd/.libs/isisd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/isisd.conf
endef

define Package/frr-ldpd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/ldpd/.libs/ldpd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/ldpd.conf
endef

define Package/frr-nhrpd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/nhrpd/.libs/nhrpd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/nhrpd.conf
endef

define Package/frr-ospfd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/ospfd/.libs/ospfd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/ospfd.conf
endef

define Package/frr-ospf6d/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/ospf6d/.libs/ospf6d $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/ospf6d.conf
endef

define Package/frr-pbrd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/pbrd/.libs/pbrd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/pbrd.conf
endef

define Package/frr-pimd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/pimd/.libs/pimd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/pimd.conf
endef

define Package/frr-ripd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/ripd/.libs/ripd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/ripd.conf
endef

define Package/frr-ripngd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/ripngd/.libs/ripngd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/frr.conf $(1)/etc/frr/ripngd.conf
endef

define Package/frr-vtysh/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/vtysh/.libs/vtysh $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/frr
	chmod 0750 $(1)/etc/frr
	$(INSTALL_CONF) ./files/vtysh.conf $(1)/etc/frr/vtysh.conf
endef

define Package/frr-libfrr/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/build/lib/.libs/libfrr.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,frr))
$(eval $(call BuildPackage,frr-babeld))
$(eval $(call BuildPackage,frr-bgpd))
$(eval $(call BuildPackage,frr-eigrpd))
$(eval $(call BuildPackage,frr-isisd))
$(eval $(call BuildPackage,frr-ldpd))
$(eval $(call BuildPackage,frr-libfrr))
$(eval $(call BuildPackage,frr-nhrpd))
$(eval $(call BuildPackage,frr-ospfd))
$(eval $(call BuildPackage,frr-ospf6d))
$(eval $(call BuildPackage,frr-pbrd))
$(eval $(call BuildPackage,frr-pimd))
$(eval $(call BuildPackage,frr-ripd))
$(eval $(call BuildPackage,frr-ripngd))
$(eval $(call BuildPackage,frr-vtysh))
$(eval $(call BuildPackage,frr-watchfrr))
$(eval $(call BuildPackage,frr-zebra))
