OPENWRT_GIT:=git://git.openwrt.org/openwrt.git
OPENWRT_COMMIT:=132abb4f6851eeb32c68fa2b62af6893d1cc57ce
OPENWRT_DEPTH:=20

BUILD_DIR=build_dir

pre_patches=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_pre_patched
patches=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_patched
feeds=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_feeds
built=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_built
configured=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_configured

build: prepare $(built)
	$(MAKE) -j 32 -C $(BUILD_DIR) defconfig package/bash/clean package/bash/compile world || make -C $(BUILD_DIR) defconfig package/bash/compile world V=s

checkout_openwrt:
	@if [ ! -d $(BUILD_DIR) ] ; then \
		git clone --depth $(OPENWRT_DEPTH) $(OPENWRT_GIT) $(BUILD_DIR) && cd $(BUILD_DIR) && git checkout -b commit_$(OPENWRT_COMMIT) $(OPENWRT_COMMIT) && cd - ; \
	fi

$(pre_patches):
	@cd $(BUILD_DIR); \
	for patch in ../patches.pre_feed/* ; do \
		patch -p1  < $$patch ; \
	done; \
	cd ..; \
	touch $@

$(patches):
	@cp -a patches $(BUILD_DIR)/ ;
	cd $(BUILD_DIR); \
	quilt push -a; \
	cd ..; \
	cp -a patches.luci $(BUILD_DIR)/feeds/luci/patches ; \
	touch $@

$(feeds):
	@cp feeds.conf $(BUILD_DIR); \
	cd $(BUILD_DIR); \
	./scripts/feeds update -a; \
	cd ..; \
	make configure; \
	make $(patches); \
	cd $(BUILD_DIR); \
	./scripts/feeds install -a; \
	cd .. ; \
	touch $@

configure:
	@if [ -d $(BUILD_DIR) ] ; then \
		conf=`cat $(configured)` && \
		cp configs/$$conf $(BUILD_DIR)/.config; \
	fi

prepare:
	make checkout_openwrt $(pre_patches) $(feeds)

clean:
	echo rm -f $(built)

distclean:
	echo rm -rf $(BUILD_DIR)

.$(OPENWRT_COMMIT)_%:
	
.DEFAULT:
	@if [ -f configs/$@ ] ; then \
		echo "Configuring for $@"; \
		echo $@ > $(configured); \
		$(MAKE) configure prepare build; \
	else \
		echo "Configuration file for $@ not found!"; \
		exit 1; \
	fi

.PHONY: clean distclean checkout_openwrt configure
