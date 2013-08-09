include $(TOPDIR)/rules.mk

PKG_NAME:=netifd-qmi
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MAINTAINER:=André Valentin <avalentin@marcant.net>

include $(INCLUDE_DIR)/package.mk

define Package/netifd-qmi
  SECTION:=net
  CATEGORY:=ImageStream
  DEPENDS:=+libqmi +iputils-arping +uqmi
  TITLE:=QMI support for netifd
  DEFAULT:=y
endef

define Package/netifd-qmi/description
  Helper scripts to enable netifd to manage qmi interfaces
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/netifd-qmi/install
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_DIR) $(1)/lib/netifd
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/network
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi/admin_network

	$(INSTALL_BIN) ./files/lib/netifd/dhcp-qmi.script $(1)/lib/netifd/
	$(INSTALL_BIN) ./files/lib/netifd/proto/qmi.sh $(1)/lib/netifd/proto/
	$(INSTALL_BIN) ./files/usr/lib/lua/luci/model/network/proto_qmi.lua $(1)/usr/lib/lua/luci/model/network/
	$(INSTALL_BIN) ./files/usr/lib/lua/luci/model/cbi/admin_network/proto_qmi.lua $(1)/usr/lib/lua/luci/model/cbi/admin_network/

endef

$(eval $(call BuildPackage,netifd-qmi))
