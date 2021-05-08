#! /bin/bash

#----------------------------------------------------------------
#   Linux System Programming HW#1 - 에라토스테네스의 체
#두 개의 정수 a, b를 입력 받은 후, a와 b 사이에 속하는 모든 소수의 합을 구하시오.
#----------------------------------------------------------------
#   Major : 컴퓨터학부                                         
#   ID : 20172612  
#   Name : 김영도
#----------------------------------------------------------------

PRIME=1
NON_PRIME=0

declare -a Primes
# Primes[] 는 배열.

seek () # 2부터 $1 까지 소수의 합
{
    # Reset Sum
    sum=0
    # 배열 초기화.
    i=1
    input=$1
    while [ $i -le $input ]
    do
        Primes[i]=$PRIME
        let "i += 1"
    done

    # 정수 2 부터 시작
    i=2

    while [ "${i}" -le "$input" ]
    do
        if [ "${Primes[i]}" -eq "$PRIME" ]
        then    
            # 해당 소수를 저장
            let "sum += i"
            temp=$i
            #i의 모든 배수는 삭제
            while [ "$temp" -le "$input" ]
            do
                let "temp += $i"
                Primes[temp]=$NON_PRIME
            done
        fi
        let "i += 1"
    done

    echo "$sum"
    return "$sum"
    
}
print_ans()
{
# 정답 출력
echo "Enter Input A && Input B : "
read A B
ans_A=`seek $A`
ans_B=`seek $B`
if [ $B -ge $A ] # 2번째 입력값이 더 클경우
    then 
        if [  "${Primes[B]}" == "$PRIME"  ]
        then # 입력값이 소수 일 경우, 사이값이므로 그 값을 한번 뺴줌
            echo `expr $ans_B - $ans_A - $B`
        else
            echo `expr $ans_B - $ans_A`
        fi
    else # 2번쨰 입력값이 더 작을 경우
        if [ "${Primes[A]}" == "$PRIME" ]
        then # 입력값이 소수 일 경우, 사이값이므로 그 값을 한번 뺴줌
            echo `expr $ans_A - $ans_B - $A`
        else
            echo `expr $ans_A - $ans_B`
        fi
fi
}
# 해당 함수 출력
print_ans