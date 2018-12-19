#
# Copyright (C) 2016 IS ImageStream Internet Solutions
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ipt-netflow
PKG_VERSION:=2.3+20180512
PKG_MAINTAINER:=Scott Yoder<syoder@imagestream.com>
PKG_LICENSE:=GPL-2.0+
PKG_LICENSE_FILES:=COPYING
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://github.com/aabc/ipt-netflow.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=9898b2cc3700290763b22f0b56b793e367725b6b
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

PKG_CONFIG_DEPENDS := \
    IPT_NETFLOW_NATEVENTS \
    IPT_NETFLOW_SAMPLER \
    IPT_NETFLOW_SNMP_AGENT \
    IPT_NETFLOW_MACADDRESS \
    IPT_NETFLOW_VLAN \
    IPT_NETFLOW_DIRECTION \
    IPT_NETFLOW_PHYSDEV

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define KernelPackage/ipt-netflow
  SUBMENU:=Netfilter Extensions
  DEPENDS:=+iptables +kmod-ipt-core +libxtables +kmod-ipt-conntrack
  TITLE:=NetFlow iptables module
  URL:=https://sourceforge.net/projects/ipt-netflow
  FILES:=$(PKG_BUILD_DIR)/ipt_*.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoProbe,ipt_NETFLOW)
endef

define Package/ipt-netflow/description
ipt-netflow is high performance NetFlow exporting module for Linux kernel (up to 4.15). Designed for Linux router with heavy network load. This is netfilter/iptables module adding support for -j NETFLOW target. Designed to work efficiently w/o conntrack. Supporting NetFlow protocols v5, v9, and IPFIX. Accounting for IPv4, IPv6 traffic, and NAT translation events (NEL). Additional options is SNMP-index translation rules, aggregation rules, Ethernet type, MPLS, VLAN, and MAC addresses exporting.
endef

define KernelPackage/ipt-netflow/config
  source "$(SOURCE)/Config.in"
endef

TARGET_CFLAGS += $(FPIC)

MAKE_FLAGS += \
       ARCH="$(LINUX_KARCH)" \
       KERNEL_DIR="$(LINUX_DIR)" \
       CC="$(TARGET_CC)"

define Build/Configure
        (cd $(PKG_BUILD_DIR); \
                ./configure \
			--kdir=$(LINUX_DIR) \
			--ipt-ver=1.6.2 \
			--ipt-inc=$(STAGING_DIR)/usr/include \
			--ipt-lib=/usr/lib/iptables \
			$(if $(CONFIG_IPT_NETFLOW_NATEVENTS),--enable)-natevents \
			$(if $(CONFIG_IPT_NETFLOW_SAMPLER),--enable-sampler) \
			$(if $(CONFIG_IPT_NETFLOW_SNMP_AGENT),,--disable-snmp-agent) \
			$(if $(CONFIG_IPT_NETFLOW_MACADDRESS),--enable)-macaddress \
			$(if $(CONFIG_IPT_NETFLOW_VLAN),--enable)-vlan \
			$(if $(CONFIG_IPT_NETFLOW_DIRECTION),--enable)-direction \
			$(if $(CONFIG_IPT_NETFLOW_PHYSDEV),--enable)-physdev \
	);
endef

define Package/ipt-netflow/install
	$(INSTALL_DIR) $(1)/usr/lib/iptables; \
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/iptables/lib*.so $(1)/usr/lib/iptables/
endef

$(eval $(call KernelPackage,ipt-netflow))

