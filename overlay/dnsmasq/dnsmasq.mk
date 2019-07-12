# Figure out the containing dir of this Makefile
OVERLAY_DIR:=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Declare custom installation commands
define custom_install_commands
	$(CP) -a $(OVERLAY_DIR)/files/* $(1)/ 2>/dev/null || true
endef

define custom_install_commands_envoy
	$(CP) -a $(OVERLAY_DIR)/files.envoy/* $(1)/ 2>/dev/null || true
endef

define custom_install_commands_rebel
	$(CP) -a $(OVERLAY_DIR)/files.rebel/* $(1)/ 2>/dev/null || true
endef

PRODUCT_VER := $(firstword $(subst -, ,$(CONFIG_VERSION_PRODUCT)))
ifeq ($(PRODUCT_VER), "RR1000")
Package/dnsmasq/install += $(newline)$(custom_install_commands_rebel)
else ifeq ($(PRODUCT_VER), "EV1000")
Package/dnsmasq/install += $(newline)$(custom_install_commands_envoy)
else
Package/dnsmasq/install += $(newline)$(custom_install_commands)
endif
