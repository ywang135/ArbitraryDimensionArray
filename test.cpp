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
    cs540::Array<int, 1> b;
    i=0;
    b[0] = 10;
    for (auto it = b.lmbegin(); it != b.lmend(); ++it) {
        assert(*it == 10);
        i++;
    }
    assert(i == 1);
}
void test1() {
    cout << "test 1: "<< endl;
    cs540::Array<int, 2, 3> a;
    int t=0;
    for(int i=0;i<2; i++){
        for(int j=0; j<3; j++){
            a[i][j] = t++;
        }
    }
    t = 0;
    for(int i=0; i<2; i++){
        for(int j=0; j<3; j++){
            assert(a[i][j] == t);
            t++;
        }
    }
    cs540::Array<char, 2> b;
    b[0] = 'i';
    b[1] = 'c';
    cs540::Array<char, 2> c(b);
    assert(c[0]=='i');
    assert(c[1]=='c');
    cs540::Array<int, 2, 3> d = a;
    d[0][0] = 90;
    assert(d[0][0]!=a[0][0]);
    auto iter1 = a.fmbegin();
    auto iter2 = a.fmend();
    assert (iter1 != iter2);
    t = 0;
    for(auto iter3=a.fmbegin(); iter3!=a.fmend(); iter3++){
        cout << *iter3 <<" ";
        assert(t == *iter3);
        t++;
    }
    cout << endl;
    for(auto iter3=a.lmbegin(); iter3!=a.lmend(); iter3++){
        cout << *iter3 <<" ";
    }
    cout << endl;
    for(auto iter3=d.fmbegin(); iter3!=d.fmend(); iter3++){
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
    for(int i=0; i<2; i++){
        for(int j=0; j<3; j++){
            for(int k=0; k<4; k++){
                a[i][j][k] = i*100+j*10+k;
            }
        }
    }
    // Range-checked indexing.
    a[1][1][1] = a[0][0][0];
    a[0][2][3] = 5678; // Set the element in the first plane, 3rd row, and 4th column.
    
    try{
        a[0][3][0] = 1; // Out of range, throws.
    }
    catch(exception& e){
        cout << "Exception: "<< e.what()<<endl;
    }
    
    a = a; // Self-assignment must be a no-op.
    b = a; // Okay, as long as dimensions and type match exactly.
    a = c; // Member template constructor.
    
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
    int t = 0;
    for(int i=0; i<2; i++){
        for(int j=0; j<3; j++){
            for(int k=0; k<2; k++){
                a[i][j][k] = t;
                b[k][j][i] = t;
                t++;
                
            }
        }
    }
    t=0;
    for(auto it = a.fmbegin(); it != a.fmend(); ++it){
        assert(*it == t);
        t++;
         
    }
    
    t=0;
    for(auto it = b.lmbegin(); it != b.lmend(); ++it){
        assert(*it == t);
        t++;
    }
}
int main(){
    test0();
    test1();
    test2();
    test3();
    return 1;
}