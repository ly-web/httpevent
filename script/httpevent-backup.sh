#!/bin/bash

MAIN_CONF=/etc/httpevent/httpevent.properties
IPDENY_CONF=/etc/httpevent/ipdeny.conf
ROUTE_CONF=/etc/httpevent/route.conf
MIME_CONF=/etc/httpevent/mime.conf

case $1 in
    backup)
        for i in $MAIN_CONF $IPDENY_CONF $ROUTE_CONF $MIME_CONF;
        do
            if test -e $i;then
                cp $i $i.bk
            else
                echo $i 'not exists.'
            fi;
        done
    ;;
    restore)
        for i in $MAIN_CONF $IPDENY_CONF $ROUTE_CONF $MIME_CONF;
        do
            if test -e $i.bk;then
                cp $i.bk  $i
            else
                echo $i.bk 'not exists.'
            fi;
        done
    ;;
    *)
    echo "Usage: $0 {backup|restore}"
    ;;
esac