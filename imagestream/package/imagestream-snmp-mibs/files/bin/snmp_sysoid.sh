#!/bin/bash

PRODUCT_IDS=("GR1000" "ER1000" "RR1000" "R1-1000" "TR1000" "EV1000" "GE1000" "SE1000" "EV1000-3G" )

SYSOID=".1.3.6.1.2.1.1.2"
SYSOID_INSTANCE="$SYSOID.0"
PRODUCTS_OID=".1.3.6.1.4.1.15425.1.2"
PRODUCTS_OIDS_REGEX="\.1\.3\.6\.1\.4\.1\.15425\.1\.2.[1-9][0-9]*$"

function get_product_code() {
    local product_id="`get_license router_build product_id`"
    local index=0

    for i in ${PRODUCT_IDS[@]}; do
        (( index++ ))

        if [ $i == $product_id ]; then
            product_code=$index
            break
        fi
    done
}

if [[ ( $1 == "-n" && $2 == $SYSOID ) || ( $1 == "-g" && $2 == $SYSOID_INSTANCE ) ]]; then
    product_code=0
    get_product_code

    if [ $product_code -gt 0 ]; then
        echo $SYSOID_INSTANCE
        echo "objectid"
        echo "$PRODUCTS_OID.$product_code"
    fi
elif [[ ( $1 == "-g" && $2 == "`echo $2 | grep $PRODUCTS_OIDS_REGEX`" ) ]]; then
    product_code="`echo $2 | cut -d '.' -f11`"
    index=$(($product_code - 1))

    if [ $index -le ${#PRODUCT_IDS[@]} ]; then
        echo $2
        echo "string"
        echo ${PRODUCT_IDS[$index]}
    fi
fi
