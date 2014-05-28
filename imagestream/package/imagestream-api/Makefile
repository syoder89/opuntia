include $(TOPDIR)/rules.mk

PKG_NAME:=imagestream-api
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=net
  CATEGORY:=ImageStream
  TITLE:=ImageStream Management REST API
  DEPENDS:=+snmpd +php5 +php5-cli +php5-cgi
  DEFAULT:=y
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(CP) -a ./files/* $(1)/ 2>/dev/null || true
	$(INSTALL_DIR) $(1)/etc/uci-defaults

	echo "uci set uhttpd.main.index_page='index.html index.php'" >> $(1)/etc/uci-defaults/$(PKG_NAME)
	echo "uci set uhttpd.main.interpreter=.php=/usr/bin/php-cgi" >> $(1)/etc/uci-defaults/$(PKG_NAME)
	echo "uci commit" >> $(1)/etc/uci-defaults/$(PKG_NAME)
endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	( . /etc/uci-defaults/$(PKG_NAME) ) && rm -f /etc/uci-defaults/$(PKG_NAME)
}
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
