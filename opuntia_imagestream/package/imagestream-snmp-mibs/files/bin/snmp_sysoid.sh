#!/bin/bash

SYSOID=".1.3.6.1.2.1.1.2"
SYSOID_INSTANCE="$SYSOID.0"
PRODUCTS_OID=".1.3.6.1.4.1.15425.1.2"
PRODUCTS_OIDS_REGEX="\.1\.3\.6\.1\.4\.1\.15425\.1\.2.[1-9][0-9]*$"

. /etc/opuntia_release

if [[ ( $1 == "-n" && $2 == $SYSOID ) || ( $1 == "-g" && $2 == $SYSOID_INSTANCE
) ]]; then
        echo $SYSOID_INSTANCE
        echo "objectid"
        echo "${PRODUCTS_OID}.1"
elif [[ ( $1 == "-g" && $2 == "`echo $2 | grep $PRODUCTS_OIDS_REGEX`" ) ]]; then
        echo $2
        echo "string"
        echo "$DISTRIB_PRODUCT"
fi
