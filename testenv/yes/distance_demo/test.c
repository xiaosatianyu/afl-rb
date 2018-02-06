#include <stdio.h>


int add(int a, int b){
    return a+b;
}

int minus(int a, int b){
    return a-b;
}

int multiple(int a, int b){
    return a*b;
}

int cal1(int a, int b){
    int c, d,e;
    c=add(a,b);
    d=minus(a,b);
    e= multiple(d,c);
    return e;
}

int cal2(int a, int b){
    int c,d,e;
    c=cal1(a,b);
    d=add(a,b);
    e=minus(c,d);
    return e;
}

void main(){
    int x=1, y=1;
    add(x,y);
    minus(x,y);
    multiple(x,y);
    cal1(x,y);
    cal2(x,y);
}

