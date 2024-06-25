#!/bin/bash

rename 'y/A-Z/a-z/' *.c *.h

A_FL_H=`ls *.h`
A_FL_SRC=`ls *.h *.c`

# замена во всех файлах заголовочные файлы в правильном регистре
#for src_ in $A_FL_SRC
#do
#	for inc_ in $A_FL_H
#	do
#		sed -i -e "s/\(#include.*\)\($inc_\)\(.*\)/\1$inc_\3/I"  $src_
#	done
#done

for src_ in $A_FL_SRC
do
#	sed -i -e "s/\(#include.*\)\(UserCAPI.h\)\(.*\)/\1UserCAPI.h\3/I"  $src_
#	sed -i -e "s/\(#include.*\)\(UserCAPI.h\)\(.*\)/\1UserCAPI.h\3/I"  $src_
#	sed -i -e "s/^ \(\*.*\)/\1/"  $src_		## убрать пробелы перед * блочных коментарий
#	sed -i -e "s/.$//" $src_				## \r\n -> \n
	sed -i -e "/./,/^$/!d" $src_			## свернуть пустые строки до одной
done
#sed -i -e 's/\(#include.*\)\(usercapi.h\)\(.*\)/\1usercapi.h\3/I'  bnso.c
