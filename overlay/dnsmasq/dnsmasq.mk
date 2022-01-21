# Figure out the containing dir of this Makefile
# The TR1000 uses the same config as the EV1000
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
HWREV := $(firstword $(subst -, ,$(CONFIG_VERSION_HWREV)))
ifeq ($(PRODUCT_VER), "RR1000")
Package/dnsmasq/install += $(newline)$(custom_install_commands_rebel)
else ifeq ($(PRODUCT_VER), "EV1000")
ifeq ($(HWREV), "2")
Package/dnsmasq/install += $(newline)$(custom_install_commands)
else
Package/dnsmasq/install += $(newline)$(custom_install_commands_envoy)
endif
else ifeq ($(PRODUCT_VER), "TR1000")
Package/dnsmasq/install += $(newline)$(custom_install_commands_envoy)
else ifeq ($(PRODUCT_VER), "AP2100")
ifeq ($(HWREV), "3")
Package/dnsmasq/install += $(newline)$(custom_install_commands_envoy)
else
Package/dnsmasq/install += $(newline)$(custom_install_commands)
endif
else
Package/dnsmasq/install += $(newline)$(custom_install_commands)
endif
