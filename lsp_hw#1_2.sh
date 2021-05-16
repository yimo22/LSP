#! /bin/bash

#----------------------------------------------------------------
#   Linux System Programming HW#1
#   2020년 12월 31일은 목요일이다. 21년의 하루를 월과 일을 나타내는 두개의 정수로 입력받은후,
#   해당일이 무슨 요일인지 출력하시오.
#----------------------------------------------------------------
#   Major : 컴퓨터학부                                         
#   ID : 20172612  
#   Name : 김영도
#----------------------------------------------------------------
declare -i M
declare -i D
echo "Enter the date"
read M D

if [  $M -lt 10 ]
then   
  if [ $D -lt 10 ]
  then
    pivot_date="20210${M}0$D"
  else
    pivot_date="20210${M}$D"
  fi  
else
    if [ $D -lt 10 ]
  then
    pivot_date="2021${M}0$D"
  else
    pivot_date="2021${M}$D"
  fi  
fi
date -d "$pivot_date" '+%A'
