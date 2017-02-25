# Figure out the containing dir of this Makefile
OVERLAY_DIR:=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Declare custom installation commands
define custom_install_commands
	$(INSTALL_DATA) $(OVERLAY_DIR)/banner $(1)/etc/banner
endef

# Append custom commands to install recipe,
# and make sure to include a newline to avoid syntax error
Package/base-files/install += $(newline)$(custom_install_commands)
