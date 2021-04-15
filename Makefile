OPENWRT_GIT:=https://github.com/openwrt/openwrt.git
OPENWRT_COMMIT:=7f703716ae0e4cb4810eff37605b7594cef1edb8
#OPENWRT_COMMIT:=d92a9c97bf3700e90af1d3c9157502af660365c0
#OPENWRT_COMMIT:=9ac47ee46918c45b91f4e4d1fa76b1e26b9d57fe
#OPENWRT_COMMIT:=e12ac405525c29a6b6195e6259d769715919560c = 4.8.17
#OPENWRT_COMMIT:=e238c85e571f5260a55a4c85fe92c68699e23521 = 4.8.16
#OPENWRT_COMMIT:=7936cb94a930dcff0d30d294efb693648e1768ff = 4.8.16
#OPENWRT_COMMIT:=7449a39c0e45179a874e4b0f48766a9e5a584d73 = 4.8.15
#OPENWRT_COMMIT:=5ef9e4f107a94c502908403fdf56cf6bcdc08dd2 = 4.8.14
#OPENWRT_COMMIT:=7ec092e64125b920aee6d1767dacea3f61b2fa6f = 4.8.13
#OPENWRT_COMMIT:=d6643aca34cb2f425ea7c5d7a725c97166b3363d = 4.8.12
#OPENWRT_COMMIT:=bd3a18bbe433cc53b6f86dd708477f97ac406abc = 4.8.12
#OPENWRT_COMMIT:=b2bf3745ff7e5e2fbf3b7b0e488cfaa5b3cca87c = 4.8.11
#OPENWRT_COMMIT:=0b373bf4d6a1a7a53e06946972ebb812b4cc2f0f = 4.8.11
#OPENWRT_COMMIT:=9b8ea3623b22509b7c5090dc50e18e9af1f13405 = 4.8.11
#OPENWRT_COMMIT:=32bc733796e1d409db656fbadd9ca5e2a2468ae9 = 4.8.10
#OPENWRT_COMMIT:=78ca6a5578d6c7b06ca520b0aac965a1babf5417 = 4.8.10
#OPENWRT_COMMIT:=be3e69d99189636d5f0854bd3a91e004b2c370e0
#OPENWRT_COMMIT:=15f16bf05b8441aed0e0c6000740996b75724ac1
#OPENWRT_COMMIT:=1b8f3d9c2ec3dd89dda524c37e4d69c3d213918e = 4.8.9
#OPENWRT_COMMIT:=a8b023272d334e47ee82449eadda01b96e9c2a90 = 4.8.8
#OPENWRT_COMMIT:=6aa4b97a8a4e4d07895682e47184c5d49441b1bb = 4.8.7
#OPENWRT_COMMIT:=254f0da6d2d9a7dac5e1281de8dcf450aff7bacd = 4.8.6
#OPENWRT_COMMIT:=8322dba029034b0a8a2c2dd57250cd225aa5abc9 = 4.8.5
#OPENWRT_COMMIT:=92b5b360fe39a4e446742ab3de9d0b2f91991d46 = 4.8.5
#OPENWRT_COMMIT:=d49ddcdfd27ba3d171b856f223712b88d5fc2046
#OPENWRT_COMMIT:=19720a6f035107b596814dd0de6b402096809ab4
#OPENWRT_COMMIT:=b964196c68d6727d14bfae868ff428d240e011c9  = 4.8.2 / 4.8.3
#OPENWRT_COMMIT:=d175c09bea24b0a52d15350f025ca418f5f066c5
#OPENWRT_COMMIT:=cac971dad98c83fd25223286330e757218f19e1b
#OPENWRT_COMMIT:=faa984a84d1bb72c262857a71ee14bccd1ae6d41
#OPENWRT_COMMIT:=3b54f5cb9c4a5789f0eec4a3e0d1ac05236cd092
#OPENWRT_COMMIT:=18ad89f4c0919ec5834b52459c211306c0e1d75a
#OPENWRT_COMMIT:=d90f5dd729e2ec4963f5bf6a5103e9c9ee3d0206
#OPENWRT_COMMIT:=14ad97cf60f9b50b1321c34c819063a9b9535635
#OPENWRT_COMMIT:=bce56f0f97952b18d75a245a147d2e9915e93b2f
#OPENWRT_COMMIT:=d0b22067afe3b78a48d3d65a783576549497f951
#OPENWRT_COMMIT:=94f447ad03964e9c1140a5fec29eada733cc054b
#OPENWRT_COMMIT:=33b7bc6d7a8417692f0e0818f93215c09a2dcdea
#OPENWRT_COMMIT:=6c049d04b6be1e953865f1361f8b6481a0ba9558
#OPENWRT_COMMIT:=5899ac4d0d971fb46077915153f844d44089726b
OPENWRT_DEPTH:=1500

BUILD_DIR?=build_dir
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

# Currently iputils / meson fails to build with ccache enabled so if the build fails try building it without ccache
build: prepare setup_cache $(built)
	@if [ ! -d $(CACHE_DIR) ] ; then \
		mkdir $(CACHE_DIR); \
	fi; \
	(export CCACHE_DIR=$(CACHE_DIR) && make $(PARALLEL_MAKE) -C $(BUILD_DIR) DESTDIR= $(BUILD_OPTS) defconfig world || (\
		make $(PARALLEL_MAKE) -C $(BUILD_DIR) DESTDIR= $(BUILD_OPTS) CONFIG_CCACHE= package/iputils/compile && \
		make $(PARALLEL_MAKE) -C $(BUILD_DIR) DESTDIR= $(BUILD_OPTS) world || \
		make -C $(BUILD_DIR) DESTDIR= $(BUILD_OPTS) world V=s )) \
		&& touch $(built)
	make setup_cache
#	(make $(PARALLEL_MAKE) -C $(BUILD_DIR) DESTDIR= defconfig world || make -C $(BUILD_DIR) DESTDIR= world V=s) && touch $(built)

install: $(built)
	@if [ ! -d $(DESTDIR) ] ; then \
		mkdir $(DESTDIR); \
	fi; \
	conf=`cat $(configured) | awk '{print toupper($$0)}'`; \
	combined=`find ${BUILD_DIR}/bin/ -name '*combined*' | head -n 1`; \
	sysup=`find ${BUILD_DIR}/bin/ -name '*sysupgrade*' | head -n 1`; \
	hwver=`grep CONFIG_VERSION_HWREV $(BUILD_DIR)/.config | cut -d '"' -f 2`; \
	ver=`grep CONFIG_VERSION_NUMBER $(BUILD_DIR)/.config | cut -d '"' -f 2`; \
	rel=`cd ${BUILD_DIR} && ./scripts/getver.sh`; \
	factory=`find ${BUILD_DIR}/bin/ -name '*factory*' | head -n 1`; \
	if [ ! -z $${factory} ]; then \
		cp -f $${factory} $(DESTDIR)/opuntia-$${conf}-v$${hwver}-$${ver}-$${rel}-factory.img; \
	elif [ "$${combined}" != "" ] ; then \
		cp -f $${combined} $(DESTDIR)/opuntia-$${conf}-v$${hwver}-$${ver}-$${rel}-factory.img; \
	fi; \
	if [ "$${combined}" != "" ] ; then \
		cp -f $${combined} $(DESTDIR)/opuntia-$${conf}-v$${hwver}-$${ver}-$${rel}-sysupgrade.bin; \
	elif [ "$${sysup}" != "" ] ; then \
		cp -f $${sysup} $(DESTDIR)/opuntia-$${conf}-v$${hwver}-$${ver}-$${rel}-sysupgrade.bin; \
	fi
#		gzip -dc $${combined} > $(DESTDIR)/opuntia-$${conf}-$${ver}-$${rel}-factory.img; \

docker_build:
	sudo apt-get update && sudo apt-get install -y docker.io && \
	sudo docker build --tag opuntia:latest docker/

docker_run: docker_build
	sudo docker run -d --rm --name opuntia -it -v /home/ubuntu/opuntia:/opuntia opuntia:latest /bin/bash

setup_cache:
	@if [ ! -d dl.cache ] ; then \
		mkdir dl.cache; \
	fi; \
	if [ -d $(BUILD_DIR)/dl ] ; then \
		cp -afn $(BUILD_DIR)/dl/* dl.cache || true; \
	fi

#	packages=`find build_dir/bin/ -name 'packages' | head -n 1` && \
#	cp -af $${packages} $(DESTDIR)/
	
#		git clone --depth $(OPENWRT_DEPTH) $(OPENWRT_GIT) $(BUILD_DIR) && cd $(BUILD_DIR) && git checkout -b commit_$(OPENWRT_COMMIT) $(OPENWRT_COMMIT) && cd - ; \
checkout_openwrt:
	@if [ ! -d $(BUILD_DIR) ] ; then \
		git clone $(OPENWRT_GIT) $(BUILD_DIR) && cd $(BUILD_DIR) && git checkout $(OPENWRT_COMMIT) && cd - ; \
		cp -a dl.cache $(BUILD_DIR)/dl ; \
		cp -a overlay $(BUILD_DIR); \
	fi
#		cp -a local-development.mk $(BUILD_DIR)/include/; \
#		cp -a qca $(BUILD_DIR); \
#		git clone $(OPENWRT_GIT) $(BUILD_DIR) && cd $(BUILD_DIR) && git checkout -b commit_$(OPENWRT_COMMIT) $(OPENWRT_COMMIT) && cd - ; \
#		git clone --depth $(OPENWRT_DEPTH) $(OPENWRT_GIT) $(BUILD_DIR) && cd $(BUILD_DIR) && git checkout -b commit_$(OPENWRT_COMMIT) $(OPENWRT_COMMIT) && cd - ; \

$(pre_patches):
	@cd $(BUILD_DIR); \
	for patch in $(find -type f ../patches.pre_feed/*) ; do \
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
		make setup_cache checkout_openwrt && \
		echo $@ > $(configured) && \
		make configure prepare build; \
	else \
		echo "Configuration file for $@ not found!"; \
		exit 1; \
	fi

.PHONY: clean distclean checkout_openwrt configure
