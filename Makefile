# ding network driver (c) Rishi Tharun
#
# Makefile for ding network driver
# Author: Rishi Tharun <vrishitharunj@gmail.com>
# License: GPLv3
#
# Just run 'make' from the project root

obj-m:=ding.o
obj-m+=dingdummy.o

ding-objs:= driver.o utils.o support.o
dingdummy-objs:= driverdummy.o utils.o support.o

all:
	@ make run --no-print-directory

dingcfg:dingcfg.o
	gcc dingcfg.o -o dingcfg

dingcfg.o:configparser.c
	gcc -c configparser.c -o dingcfg.o

run:dingcfg
	@cat /dev/null > dingcfgs.h
	@chmod +x dingcfg
	@./dingcfg
	@-rmmod ding.ko 2> /dev/null
	@-rmmod dingdummy.ko 2> /dev/null
	@make build
	insmod ding.ko
	insmod dingdummy.ko
	@make clean
	@make show

build:
	@make -C /lib/modules/`uname -r`/build M=`pwd` modules

clean:
	@rm -rf dingcfg.* ding.*o dingdummy.*o .tmp_versions .*.tmp *mod* .ding* Module* dingcfg dingcfgs.h *.o .*.*o*.cmd .*.*.cmd

show:
	@echo
	@echo "ding drivers"
	@echo "---- -------"
	@echo
	@ifconfig ding
	@ifconfig dingdummy

