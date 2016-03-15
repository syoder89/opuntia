OPENWRT_GIT:=git://git.openwrt.org/openwrt.git
OPENWRT_COMMIT:=18ad89f4c0919ec5834b52459c211306c0e1d75a
#OPENWRT_COMMIT:=d90f5dd729e2ec4963f5bf6a5103e9c9ee3d0206
#OPENWRT_COMMIT:=14ad97cf60f9b50b1321c34c819063a9b9535635
#OPENWRT_COMMIT:=bce56f0f97952b18d75a245a147d2e9915e93b2f
#OPENWRT_COMMIT:=d0b22067afe3b78a48d3d65a783576549497f951
#OPENWRT_COMMIT:=94f447ad03964e9c1140a5fec29eada733cc054b
#OPENWRT_COMMIT:=33b7bc6d7a8417692f0e0818f93215c09a2dcdea
#OPENWRT_COMMIT:=6c049d04b6be1e953865f1361f8b6481a0ba9558
#OPENWRT_COMMIT:=5899ac4d0d971fb46077915153f844d44089726b
OPENWRT_DEPTH:=1500

BUILD_DIR=build_dir
DESTDIR?=images

SHELL := $(SHELL) -e

pre_patches=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_pre_patched
patches=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_patched
feeds=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_feeds
built=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_built
configured=$(BUILD_DIR)/.$(OPENWRT_COMMIT)_configured
BUILD_NUMBER?=1
NUM_CPU=$(shell grep '^processor' /proc/cpuinfo | wc -l)
PARALLEL_MAKE=-j $(shell echo $$(( $(NUM_CPU) * 2 )))
CACHE_DIR?=$(shell pwd)/ccache
BUILD_OPTS=

build: prepare setup_cache $(built)
	@if [ ! -d $(CACHE_DIR) ] ; then \
		mkdir $(CACHE_DIR); \
	fi; \
	(export CCACHE_DIR=$(CACHE_DIR) && make $(PARALLEL_MAKE) -C $(BUILD_DIR) DESTDIR= $(BUILD_OPTS) defconfig world || \
		((for retries in 1 2 3 4 5; do \
			echo "Building bash attempt $$retries / 5"; \
			make -C $(BUILD_DIR) DESTDIR= $(BUILD_OPTS) package/bash/clean && make -C $(BUILD_DIR) DESTDIR= -j1 $(BUILD_OPTS) package/bash/compile && break; \
		done;) \
		&& make $(PARALLEL_MAKE) -C $(BUILD_DIR) DESTDIR= $(BUILD_OPTS) world || make -C $(BUILD_DIR) DESTDIR= $(BUILD_OPTS) world V=s)) \
		&& touch $(built)
#	(make $(PARALLEL_MAKE) -C $(BUILD_DIR) DESTDIR= defconfig world || make -C $(BUILD_DIR) DESTDIR= world V=s) && touch $(built)

install: $(built)
	@if [ ! -d $(DESTDIR) ] ; then \
		mkdir $(DESTDIR); \
	fi; \
	conf=`cat $(configured) | awk '{print toupper($$0)}'` && \
	combined=`find build_dir/bin/ -name '*combined*' | head -n 1` && \
	sysup=`find build_dir/bin/ -name '*sysupgrade*' | head -n 1` && \
	ver=`grep CONFIG_VERSION_NUMBER $(BUILD_DIR)/.config | cut -d '"' -f 2` && \
	rel=`cd build_dir && ./scripts/getver.sh` && \
	factory=`find build_dir/bin/ -name '*factory*' | head -n 1` && \
		cp -f $${factory} $(DESTDIR)/opuntia-$${conf}-$${ver}-$${rel}-factory.img; \
	if [ "$${combined}" != "" ] ; then \
		cp -f $${combined} $(DESTDIR)/opuntia-$${conf}-$${ver}-$${rel}-sysupgrade.bin; \
	elif [ "$${sysup}" != "" ] ; then \
		cp -f $${sysup} $(DESTDIR)/opuntia-$${conf}-$${ver}-$${rel}-sysupgrade.bin; \
	fi

setup_cache:
	[ ! -d dl.cache ] && mkdir dl.cache
	[ -d $(BUILD_DIR)/dl ] && cp -an $(BUILD_DIR)/dl/* dl.cache

#	packages=`find build_dir/bin/ -name 'packages' | head -n 1` && \
#	cp -af $${packages} $(DESTDIR)/
	
checkout_openwrt:
	@if [ ! -d $(BUILD_DIR) ] ; then \
		git clone --depth $(OPENWRT_DEPTH) $(OPENWRT_GIT) $(BUILD_DIR) && cd $(BUILD_DIR) && git checkout -b commit_$(OPENWRT_COMMIT) $(OPENWRT_COMMIT) && cd - ; \
		cp -a dl.cache $(BUILD_DIR)/dl ; \
	fi
#		git clone $(OPENWRT_GIT) $(BUILD_DIR) && cd $(BUILD_DIR) && git checkout -b commit_$(OPENWRT_COMMIT) $(OPENWRT_COMMIT) && cd - ; \
#		git clone --depth $(OPENWRT_DEPTH) $(OPENWRT_GIT) $(BUILD_DIR) && cd $(BUILD_DIR) && git checkout -b commit_$(OPENWRT_COMMIT) $(OPENWRT_COMMIT) && cd - ; \

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
	quilt push -af || (echo "$$patch failed to apply!" && exit 1); \
	cd ..; \
	cp -a patches.luci $(BUILD_DIR)/feeds/luci/patches ; \
	cd $(BUILD_DIR)/feeds/luci; \
	quilt push -af || (echo "$$patch failed to apply!" && exit 1); \
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
		ver=`cat version` && \
		cat configs/base configs/$$conf > $(BUILD_DIR)/.config && \
		sed -i "s/^CONFIG_VERSION_NUMBER=.*/CONFIG_VERSION_NUMBER=\"$$ver\"/" $(BUILD_DIR)/.config; \
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
		echo "Configuring for $@" && \
		make checkout_openwrt && \
		echo $@ > $(configured) && \
		make configure prepare build; \
	else \
		echo "Configuration file for $@ not found!"; \
		exit 1; \
	fi

.PHONY: clean distclean checkout_openwrt configure
