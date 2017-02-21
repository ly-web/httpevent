#!/bin/sh

make clean && make && sudo make install && sudo systemctl daemon-reload && sleep 4 && make demo-clean && make demo && sudo make demo-install

