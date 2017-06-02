# Figure out the containing dir of this Makefile
OVERLAY_DIR:=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Declare custom installation commands
define custom_install_commands
	$(CP) -a $(OVERLAY_DIR)/files/* $(1)/ 2>/dev/null || true
endef

# Append custom commands to install recipe,
# and make sure to include a newline to avoid syntax error
Package/dnsmasq/install += $(newline)$(custom_install_commands)
