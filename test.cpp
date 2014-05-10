#include <iostream>
#include <string>
#include <exception>
#include <typeinfo>
#include "Array.hpp"
using namespace std;

int main(){
    //size_t i;
    cs540::Array<int, 2, 3> a;
    cout << endl;
    a[1][1] = 2;
    cout << a[1].ValueType.name()<<endl;
    cout << a[1].ValueType.name()<<": "<<a[1][1]<<endl;
    cs540::Array<char, 2> b;
    b[1] = 'c';
    cs540::Array<char, 2> c(b);
    cout << c[1]<<endl;
    cs540::Array<int, 2, 3> d(a);
    cout << d[1].ValueType.name()<<": "<<d[1][1]<<endl;
    //cout << typeid(a[1][1]).name()<<endl;
    //cout<< a[1][2][3];
    //cs540::ArrayArray<int> *c = &((*b)[1]);
    //(*c)[1];
    a[0][0] = 100;
    a[0][1] = 123;
    /*
    auto iter = a.fmbegin();
    
    cout << *iter<< endl;
    *iter = 111;
    cout << a[0][0]<< endl;
    ++iter;
    cout << *iter<<" "<<a[0][1] << endl;
    
    auto iter1 = a.fmbegin();
    cout << *(iter1++) << endl;
    cout << *iter1 << endl;
     */
    auto iter1 = a.fmbegin();
    auto iter2 = a.fmend();
    cout << (iter1 == iter2) << endl<<endl;
    for(auto iter3=a.fmbegin(); iter3!=a.fmend(); iter3++){
        cout << *iter3 <<" ";
    }
    return 1;
}