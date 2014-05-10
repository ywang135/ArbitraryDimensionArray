#include <iostream>
#include <string>
#include <exception>
#include <typeinfo>
#include <assert.h>
#include "Array.hpp"
using namespace std;
void test0(){
    cs540::Array<int, 10> a;
    int i;
    for(i=0; i<10; i++){
        a[i] = i;
    }
    i=0;
    for (auto it = a.fmbegin(); it != a.fmend(); ++it) {
        assert(*it == i);
        i++;
    }
    i=0;
    for (auto it = a.lmbegin(); it != a.lmend(); ++it) {
        assert(*it == i);
        i++;
    }
}
void test1() {
    cout << "test 1: "<< endl;
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
    cout << endl;
    for(auto iter3=a.lmbegin(); iter3!=a.lmend(); iter3++){
        cout << *iter3 <<" ";
    }
    cout << endl;
}
void test2(){
    cout << "test 2: "<< endl;
    // Define a 2X3X4 array of integers.  Elements are uninitialized.
    cs540::Array<int, 2, 3, 4> a, b;
    cs540::Array<short, 2, 3, 4> c;
    //cs540::Array<int, 0> e1; // This line must cause a compile-time error.
    
    // Range-checked indexing.
    a[0][0][0] = 1234;
    a[0][0][3] = 3;
    a[1][1][1] = a[0][0][0];
    a[1][0][3] = 103;
    a[0][2][3] = 5678; // Set the element in the first plane, 3rd row, and 4th column.
    a[1][1][3] = 113;
    a[1][2][1] = 121;
    a[1][0][0] = 110;
    a[1][0][1] = 101;
    a[0][0][1] = 1;
    
    try{
        a[0][3][0] = 1; // Out of range, throws.
    }
    catch(exception& e){
        cout << "Exception: "<< e.what()<<endl;
    }
    
    a = a; // Self-assignment must be a no-op.
    b = a; // Okay, as long as dimensions and type match exactly.
    a = c; // Member template constructor.
    //cout << "~~~here2"<<endl;
    
    const cs540::Array<int, 2, 3, 4> &const_ref(a);
    
    int i = const_ref[0][0][0]; // Okay.
    i++;
    //const_ref[0][0][0] = 1; // Syntax error.
    
    // Iterate through in plane major order.  (Note: Normally, you would use
    // auto to simplify.)
    
    for (auto it = b.fmbegin(); it != b.fmend(); ++it) {
        cout << *it << "    <";
        it.printDim();
        cout <<">\n";
        i++;
    }
    cout << endl;
    // Iterate through in column major order.
    for (auto it = b.lmbegin(); it != b.lmend(); ++it) {
        cout << *it << "    <";
        it.printDim();
        cout <<">\n";
        i++;
    }
    cout << endl;
}
void test3(){
    cs540::Array<int, 2, 3, 2> a, b;
    a[0][0][0] = 0;
    a[0][0][1] = 1;
    a[0][1][0] = 2;
    a[0][1][1] = 3;
    a[0][2][0] = 4;
    a[0][2][1] = 5;
    a[1][0][0] = 6;
    a[1][0][1] = 7;
    a[1][1][0] = 8;
    a[1][1][1] = 9;
    a[1][2][0] = 10;
    a[1][2][1] = 11;
    
    b[0][0][0] = 0;
    b[1][0][0] = 1;
    b[0][1][0] = 2;
    b[1][1][0] = 3;
    b[0][2][0] = 4;
    b[1][2][0] = 5;
    b[0][0][1] = 6;
    b[1][0][1] = 7;
    b[0][1][1] = 8;
    b[1][1][1] = 9;
    b[0][2][1] = 10;
    b[1][2][1] = 11;
    int i=0;
    for(auto it = a.fmbegin(); it != a.fmend(); ++it){
        assert(*it == i);
        i++;
         
    }
    
    i=0;
    for(auto it = b.lmbegin(); it != b.lmend(); ++it){
        assert(*it == i);
        i++;
    }
}
int main(){
    test0();
    test1();
    test2();
    test3();
    return 1;
}