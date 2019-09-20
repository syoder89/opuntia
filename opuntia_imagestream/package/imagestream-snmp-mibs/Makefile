include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-snmp-mibs
PKG_VERSION:=1
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=net
  CATEGORY:=ImageStream
  TITLE:=ImageStream SNMP MIBS
  DEPENDS:=+snmpd
#  DEPENDS:=+snmpd +php7 +php7-cli
  DEFAULT:=y
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(CP) -a ./files/* $(1)/ 2>/dev/null || true
	$(INSTALL_DIR) $(1)/etc/uci-defaults

#	echo "uci add snmpd pass" > $(1)/etc/uci-defaults/$(PKG_NAME)
#	echo "uci set snmpd.@pass[-1].persist=true" >> $(1)/etc/uci-defaults/$(PKG_NAME)
#	echo "uci set snmpd.@pass[-1].miboid='1.3.6.1.4.1.15425.1.1.2.1'" >> $(1)/etc/uci-defaults/$(PKG_NAME)
#	echo "uci set snmpd.@pass[-1].prog='/bin/snmp_qos.sh'" >> $(1)/etc/uci-defaults/$(PKG_NAME)
#	echo "uci commit" >> $(1)/etc/uci-defaults/$(PKG_NAME)

#	echo "uci add snmpd pass" >> $(1)/etc/uci-defaults/$(PKG_NAME)
#	echo "uci set snmpd.@pass[-1].persist=true" >> $(1)/etc/uci-defaults/$(PKG_NAME)
#	echo "uci set snmpd.@pass[-1].miboid='1.3.6.1.4.1.15425.1.1.3.1'" >> $(1)/etc/uci-defaults/$(PKG_NAME)
#	echo "uci set snmpd.@pass[-1].prog='/bin/snmp_3g.sh'" >> $(1)/etc/uci-defaults/$(PKG_NAME)
#	echo "uci commit" >> $(1)/etc/uci-defaults/$(PKG_NAME)

	echo "uci add snmpd pass" >> $(1)/etc/uci-defaults/$(PKG_NAME)
	echo "uci set snmpd.@pass[-1].priority=0" >> $(1)/etc/uci-defaults/$(PKG_NAME)
	echo "uci set snmpd.@pass[-1].miboid='1.3.6.1.2.1.1.2'" >> $(1)/etc/uci-defaults/$(PKG_NAME)
	echo "uci set snmpd.@pass[-1].prog='/bin/snmp_sysoid.sh'" >> $(1)/etc/uci-defaults/$(PKG_NAME)
	echo "uci commit" >> $(1)/etc/uci-defaults/$(PKG_NAME)

	echo "uci add snmpd pass" >> $(1)/etc/uci-defaults/$(PKG_NAME)
	echo "uci set snmpd.@pass[-1].priority=0" >> $(1)/etc/uci-defaults/$(PKG_NAME)
	echo "uci set snmpd.@pass[-1].miboid='1.3.6.1.4.1.15425.1.2'" >> $(1)/etc/uci-defaults/$(PKG_NAME)
	echo "uci set snmpd.@pass[-1].prog='/bin/snmp_sysoid.sh'" >> $(1)/etc/uci-defaults/$(PKG_NAME)
	echo "uci commit" >> $(1)/etc/uci-defaults/$(PKG_NAME)
endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	( . /etc/uci-defaults/$(PKG_NAME) ) && rm -f /etc/uci-defaults/$(PKG_NAME)
}
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
