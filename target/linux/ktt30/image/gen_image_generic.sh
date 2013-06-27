#!/usr/bin/env bash
# Copyright (C) 2006-2012 OpenWrt.org
set -x
[ $# == 5 -o $# == 6 ] || {
    echo "SYNTAX: $0 <file> <kernel size> <kernel directory> <rootfs size> <rootfs image> [<align>]"
    exit 1
}

OUTPUT="$1"
KERNELSIZE="$2"
KERNELDIR="$3"
ROOTFSSIZE="$4"
ROOTFSIMAGE="$5"
ALIGN="$6"

rm -f "$OUTPUT"

head=16
sect=63
cyl=$(( ($KERNELSIZE + $ROOTFSSIZE) * 1024 * 1024 / ($head * $sect * 512)))

# create partition table
set `ptgen -o "$OUTPUT" -h $head -s $sect -t c -p ${KERNELSIZE}m -t 83 -p ${ROOTFSSIZE}m ${ALIGN:+-l $ALIGN}`

KERNELOFFSET="$(($1 / 512))"
KERNELSIZE="$(($2 / 512))"
ROOTFSOFFSET="$(($3 / 512))"
ROOTFSSIZE="$(($4 / 512))"

BLOCKS="$((($KERNELSIZE / 2)))"

[ -n "$PADDING" ] && dd if=/dev/zero of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc count="$ROOTFSSIZE"
dd if="$ROOTFSIMAGE" of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc

rm -f boot.img

mkdosfs boot.img -C $BLOCKS
mcopy -i boot.img ${KERNELDIR}/uImage ::

dd if="boot.img" of="$OUTPUT" bs=512 seek="512" conv=notrunc
rm -f "boot.img"
