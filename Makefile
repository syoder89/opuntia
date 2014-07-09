OPENWRT_GIT:=git://git.openwrt.org/openwrt.git
OPENWRT_COMMIT:=eda2c487b0ec35e0153c144f8988bf426e38debd
#OPENWRT_COMMIT:=132abb4f6851eeb32c68fa2b62af6893d1cc57ce
OPENWRT_DEPTH:=20

BUILD_DIR=build_dir
IMAGE_DIR=images

pre_patches=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_pre_patched
patches=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_patched
feeds=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_feeds
built=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_built
configured=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_configured
buildid:=1

build: prepare $(built)
	$(MAKE) -j 32 -C $(BUILD_DIR) defconfig world || ($(MAKE) -C $(BUILD_DIR) package/bash/clean package/bash/compile && $(MAKE) -j 32 -C $(BUILD_DIR) world || $(MAKE) -C $(BUILD_DIR) world V=s) && make copybin

copybin:
	@if [ ! -d $(IMAGE_DIR) ] ; then \
		mkdir $(IMAGE_DIR); \
	fi; \
	conf=`cat $(configured)` && \
	image=`find build_dir/bin/ -name '*combined*.img' | head -n 1` && \
	ver=`grep CONFIG_VERSION_NUMBER $(BUILD_DIR)/.config | cut -d '"' -f 2` && \
	cp -f $${image} $(IMAGE_DIR)/opuntia-$${conf}-$${ver}_$(buildid).img
	
checkout_openwrt:
	@if [ ! -d $(BUILD_DIR) ] ; then \
		git clone --depth $(OPENWRT_DEPTH) $(OPENWRT_GIT) $(BUILD_DIR) && cd $(BUILD_DIR) && git checkout -b commit_$(OPENWRT_COMMIT) $(OPENWRT_COMMIT) && cd - ; \
	fi

$(pre_patches):
	@cd $(BUILD_DIR); \
	for patch in ../patches.pre_feed/* ; do \
		patch -p1  < $$patch || (echo "$$patch failed to apply!" && exit 1); \
	done; \
	cd ..; \
	touch $@

$(patches):
	@cp -a patches $(BUILD_DIR)/ ;
	cd $(BUILD_DIR); \
	quilt push -a || (echo "$$patch failed to apply!" && exit 1); \
	cd ..; \
	cp -a patches.luci $(BUILD_DIR)/feeds/luci/patches ; \
	cd $(BUILD_DIR)/feeds/luci; \
	quilt push -a || (echo "$$patch failed to apply!" && exit 1); \
	cd ../../..; \
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
	make $(pre_patches) $(feeds)

clean:
	rm -f $(built)

distclean:
	rm -rf $(BUILD_DIR)

.$(OPENWRT_COMMIT)_%:
	
.DEFAULT:
	@if [ -f configs/$@ ] ; then \
		echo "Configuring for $@"; \
		$(MAKE) checkout_openwrt; \
		echo $@ > $(configured); \
		$(MAKE) configure prepare build; \
	else \
		echo "Configuration file for $@ not found!"; \
		exit 1; \
	fi

.PHONY: clean distclean checkout_openwrt configure
