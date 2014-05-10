#ifndef ____ARBITRARY_DIMENSION_ARRAY__
#define ____ARBITRARY_DIMENSION_ARRAY__
#include <iostream>
#include <cstdarg>
#include <string>
#include <stdexcept>
#include <sstream>
#include <type_traits>
#include <typeinfo>
#include <stdarg.h>
#define OPEN_CS540 namespace cs540 {
#define CLOSE_CS540 }

OPEN_CS540

bool debug = false;
struct OutOfRange: public std::runtime_error
{
    OutOfRange(std::string const& message)
    : std::runtime_error(message + " Was thrown")
    {}
};
struct WrongTemplate: public std::runtime_error
{
    WrongTemplate(std::string const& message)
    : std::runtime_error(message + " Was thrown")
    {}
};
void throwOutOfRangeException(){
    throw OutOfRange("Out of Range Exception");
}
void throwWrongTemplateException(){
    throw WrongTemplate("Wrong Template Exception");
}

template<typename T, size_t D, size_t... Dims>
class Array{
public:
    const std::type_info& ValueType;
    Array<T, Dims...>* arr;
    size_t dimSize;
    size_t size;
    
public:
    friend class Array<T , Dims...>;
    Array(): ValueType(typeid(T)), arr(nullptr), dimSize(sizeof...(Dims)), size(D){
        if(debug){
            std::cout<<"allocate array\n";
        }
        if(D<=0){
            throwOutOfRangeException();
        }
        size = D;
        arr = new Array<T, Dims...>[D];
    }
    Array(const Array & array): ValueType(array.ValueType), dimSize(sizeof...(Dims)), size(D){
        size_t i;
        if(ValueType != array.ValueType || dimSize != array.dimSize || size != array.size){
            throwWrongTemplateException();
        }
        arr = new Array<T, Dims...>[size];
        for(i=0; i< size; i++){
            arr[i]= array.arr[i];
        }
    }
    template <typename U>
    Array(const Array<U, D, Dims...> & array): ValueType(array.ValueType), dimSize(sizeof...(Dims)), size(D){
        size_t i;
        arr = new Array<U, Dims...>[size];
        for(i=0; i< size; i++){
            arr[i] = new Array<U, Dims...>(array.arr[i]);
        }
    }
    Array &operator=(const Array &array){
        if(debug){
            std::cout<<"copy assignment 1\n";
        }
        size_t i;
        if(array.arr == arr){
            if(debug){
                std::cout<<"    self assignment 1: no op\n";
            }
            return *this;
        }
        if(dimSize != array.dimSize || size != array.size){
            throwWrongTemplateException();
        }
        for(i=0; i< size; i++){
            arr[i] = array.arr[i];
        }
        return *this;
    }
    template <typename U>
    Array &operator=(const Array<U, D, Dims...> & array){
        if(debug){
            std::cout<<"copy assignment 2: different type\n";
        }
        size_t i;
        for(i=0; i< size; i++){
            arr[i] = array.arr[i];
        }
        return *this;
    }
    Array<T, Dims... >& operator[](const size_t d1) {
        if(debug){
            std::cout<<"call array["<<d1<<"] at Array. ";
            std::cout<<"Dimension of Array is "<<dimSize<<"\n";
        }
        if(d1>=size){
            throwOutOfRangeException();
        }
        return arr[d1];
    }
    const Array<T, Dims... >& operator[](const size_t d1) const{
        if(d1>=size){
            throwOutOfRangeException();
        }
        return arr[d1];
    }
    ~Array(){
        delete [] arr;
    }
    class FirstDimensionMajorIterator{
    public:
        friend class Array<T, Dims...>;
        friend class Array<T, Dims...>::FirstDimensionMajorIterator;
        Array<T, D, Dims...> *arr;
        typename Array<T, Dims...>::FirstDimensionMajorIterator nextIterator;
        size_t size;
        size_t curdims;
        bool isEnd;
        FirstDimensionMajorIterator():size(D),curdims(0), isEnd(false) {
        }
        FirstDimensionMajorIterator(const FirstDimensionMajorIterator & fmi): arr(fmi.arr)
            , nextIterator(fmi.nextIterator)
            , size(fmi.size)
            , curdims(fmi.curdims)
            , isEnd(fmi.isEnd){ }
        FirstDimensionMajorIterator &
        operator=(const FirstDimensionMajorIterator & fmi){
            arr = fmi.arr;
            size = fmi.size;
            curdims = fmi.curdims;
            nextIterator = fmi.nextIterator;
            isEnd = fmi.isEnd;
            return *this;
        }
        friend bool operator==(const FirstDimensionMajorIterator & fmi1,
                        const FirstDimensionMajorIterator &fmi2){
            if(fmi1.arr!=fmi2.arr){
                return false;
            }
            if(fmi1.size!=fmi2.size){
                return false;
            }
            if(fmi1.curdims!=fmi2.curdims){
                return false;
            }
            if(fmi1.isEnd != fmi2.isEnd){
                return false;
            }
            if(fmi1.isEnd){
                return true;
            }
            return fmi1.nextIterator == fmi2.nextIterator;
        }
        friend bool operator!=(const FirstDimensionMajorIterator & fmi1,
                        const FirstDimensionMajorIterator & fmi2){
            if(fmi1.arr!=fmi2.arr){
                return true;
            }
            if(fmi1.size!=fmi2.size){
                return true;
            }
            if(fmi1.curdims!=fmi2.curdims){
                return true;
            }
            if(fmi1.isEnd != fmi2.isEnd){
                return true;
            }
            if(fmi1.isEnd){
                return false;
            }
            return fmi1.nextIterator != fmi2.nextIterator;
        }
        void relocate(){
            nextIterator.arr = &(arr->arr[curdims]);
            nextIterator.isEnd = false;
            nextIterator.relocate();
        }
        FirstDimensionMajorIterator &operator++(){
            if(debug){
                std::cout << "fmi pre increment\n";
            }
            ++nextIterator;
            if(nextIterator.isEnd && nextIterator.curdims == 0){
                if(debug){
                    std::cout << "next dimension\n";
                }
                curdims = curdims+1;
                if(curdims == size){
                    curdims = 0;
                    isEnd = true;
                }
                else{
                    isEnd = false;
                }
                relocate();
            }
            
            return *this;
        }
        FirstDimensionMajorIterator operator++(int){
            FirstDimensionMajorIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        T &operator*() const{
            return *nextIterator;
        }
        void printDim(){
            std::cout << curdims;
            if(isEnd){
                std::cout<<"T ";
            }
            else
                std::cout<<"F ";
            nextIterator.printDim();
        }
    };
    class LastDimensionMajorIterator{
    public:
        friend class Array<T, Dims...>;
        friend class Array<T, Dims...>::LastDimensionMajorIterator;
        Array<T, D, Dims...> *arr;
        typename Array<T, Dims...>::LastDimensionMajorIterator nextIterator;
        size_t size;
        size_t curdims;
        bool isEnd;
        LastDimensionMajorIterator():size(D),curdims(0), isEnd(false) {}
        LastDimensionMajorIterator(const LastDimensionMajorIterator & fmi): arr(fmi.arr)
        , nextIterator(fmi.nextIterator)
        , size(fmi.size)
        , curdims(fmi.curdims)
        , isEnd(fmi.curdims){ }
        LastDimensionMajorIterator &
        operator=(const LastDimensionMajorIterator & fmi){
            arr = fmi.arr;
            size = fmi.size;
            curdims = fmi.curdims;
            nextIterator = fmi.nextIterator;
            isEnd = fmi.isEnd;
            return *this;
        }
        friend bool operator==(const LastDimensionMajorIterator & fmi1,
                               const LastDimensionMajorIterator &fmi2){
            if(fmi1.arr!=fmi2.arr){
                return false;
            }
            if(fmi1.size!=fmi2.size){
                return false;
            }
            if(fmi1.curdims!=fmi2.curdims){
                return false;
            }
            if(fmi1.isEnd != fmi2.isEnd){
                return false;
            }
            return fmi1.nextIterator == fmi2.nextIterator;
        }
        friend bool operator!=(const LastDimensionMajorIterator & fmi1,
                               const LastDimensionMajorIterator & fmi2){
            if(fmi1.arr!=fmi2.arr){
                return true;
            }
            if(fmi1.size!=fmi2.size){
                return true;
            }
            if(fmi1.curdims!=fmi2.curdims){
                return true;
            }
            if(fmi1.isEnd != fmi2.isEnd){
                return true;
            }
            return fmi1.nextIterator != fmi2.nextIterator;
        }
        void relocate(){
            nextIterator.arr = &(arr->arr[curdims]);
            nextIterator.relocate();
            isEnd = nextIterator.isEnd;
        }
        LastDimensionMajorIterator &operator++(){
            curdims = (curdims+1)%size;
            if(curdims == 0){
                isEnd = true;
                ++nextIterator;
            }
            relocate();
            return *this;
        }
        LastDimensionMajorIterator operator++(int){
            LastDimensionMajorIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        T &operator*() const{
            return *nextIterator;
        }
        void printDim(){
            std::cout << curdims;
            if(isEnd){
                std::cout<<"T ";
            }
            else
                std::cout<<"F ";
            nextIterator.printDim();
        }
    
    };
    
    FirstDimensionMajorIterator fmbegin(){
        FirstDimensionMajorIterator fmi;
        fmi.arr = this;
        fmi.nextIterator = arr[0].fmbegin();
        return fmi;
    }
    FirstDimensionMajorIterator fmend(){
        FirstDimensionMajorIterator fmi;
        fmi.arr = this;
        fmi.isEnd = true;
        fmi.nextIterator = arr[0].fmend();
        return fmi;
    }
    LastDimensionMajorIterator lmbegin(){
        LastDimensionMajorIterator fmi;
        fmi.arr = this;
        fmi.nextIterator = arr[0].lmbegin();
        return fmi;
    }
    LastDimensionMajorIterator lmend(){
        LastDimensionMajorIterator fmi;
        fmi.arr = this;
        fmi.isEnd = true;
        fmi.nextIterator = arr[0].lmend();
        return fmi;
    }

};
template<typename T, size_t D>
class Array<T, D>{
public:
    const std::type_info& ValueType;
    T* arr;
    size_t dimSize;
    size_t size;
    
public:
    Array(): ValueType(typeid(T)),arr(nullptr), dimSize(0), size(D){
        if(debug){
            std::cout<<"allocate array ["<<dimSize<<"]\n";
        }
        if(D<=0){
            throwOutOfRangeException();
        }
        arr = new T[D];
    }
    Array(const Array & array): ValueType(typeid(T)), dimSize(0), size(D){
        size_t i;
        if(ValueType != array.ValueType || dimSize != array.dimSize || size != array.size){
            throwWrongTemplateException();
        }
        
        arr = new T[size];
        for(i=0; i< size; i++){
            arr[i] = array.arr[i];
        }
    }
    template <typename U>
    Array(const Array<U, D> & array): ValueType(array.ValueType), dimSize(0), size(D){
        size_t i;
        arr = new U[D];
        for(i=0; i< size; i++){
            arr[i] = array.arr[i];
        }
    }
    Array &operator=(const Array & array){
        size_t i;
        if(0 != array.dimSize || size != array.size){
            throwWrongTemplateException();
        }
        for(i=0; i< size; i++){
            arr[i] = array.arr[i];
        }
        return *this;
    }
    template <typename U>
    Array &operator=(const Array<U, D> & array){
        size_t i;
        for(i=0; i< size; i++){
            arr[i] = array.arr[i];
        }
        return *this;
    }
    T& operator[](const size_t d1){
        if(debug){
            std::cout<<"call array[] at Array. ";
            std::cout<<"Dimension of Array is "<<dimSize<<"\n";
        }
        if(d1>=size){
            throwOutOfRangeException();
        }
        return arr[d1];
    }
    const T &operator[](const size_t d1) const{
        if(d1>=size){
            throwOutOfRangeException();
        }
        return arr[d1];
    }
    ~Array(){
        delete []arr;
    }
    class FirstDimensionMajorIterator{
    public:
        Array<T, D> *arr;
        T* elem;
        size_t size;
        size_t curdims;
        bool isEnd;
        FirstDimensionMajorIterator():size(D),curdims(0), isEnd(false) { }
        FirstDimensionMajorIterator(const FirstDimensionMajorIterator & fmi): arr(fmi.arr)
            , elem(fmi.elem)
            , size(fmi.size)
            , curdims(fmi.curdims)
            , isEnd(fmi.isEnd){ }
        FirstDimensionMajorIterator &
        operator=(const FirstDimensionMajorIterator & fmi){
            arr = fmi.arr;
            size = fmi.size;
            curdims = fmi.curdims;
            elem = fmi.elem;
            isEnd = fmi.isEnd;
            return *this;
        }
        friend bool operator==(const FirstDimensionMajorIterator & fmi1,
                               const FirstDimensionMajorIterator &fmi2){
            if(fmi1.arr!=fmi2.arr){
                return false;
            }
            if(fmi1.size!=fmi2.size){
                return false;
            }
            if(fmi1.curdims!=fmi2.curdims){
                return false;
            }
            if(fmi1.isEnd != fmi2.isEnd){
                return false;
            }
            if(fmi1.isEnd){
                return true;
            }
            return fmi1.elem == fmi2.elem;
        }
        friend bool operator!=(const FirstDimensionMajorIterator & fmi1,
                               const FirstDimensionMajorIterator & fmi2){
            if(fmi1.arr!=fmi2.arr){
                return true;
            }
            if(fmi1.size!=fmi2.size){
                return true;
            }
            if(fmi1.curdims!=fmi2.curdims){
                return true;
            }
            if(fmi1.isEnd != fmi2.isEnd){
                return true;
            }
            if(fmi1.isEnd){
                return false;
            }
            return fmi1.elem != fmi2.elem;
        }
        void relocate(){
            elem = &(arr->arr[curdims]);
        }
        FirstDimensionMajorIterator &operator++(){
            curdims ++;
            if(curdims == size){
                curdims = 0;
                isEnd = true;
            }
            else{
                isEnd = false;
            }
            relocate();
            return *this;
        }
        FirstDimensionMajorIterator operator++(int){
            FirstDimensionMajorIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        T &operator*() const{
            return *elem;
        }
        void printDim(){
            std::cout << curdims;
            if(isEnd){
                std::cout<<"T ";
            }
            else
                std::cout<<"F ";
        }
    };
    class LastDimensionMajorIterator{
    public:
        friend class Array<T, D>;
        Array<T, D> *arr;
        T* elem;
        size_t size;
        size_t curdims;
        bool isEnd;
        LastDimensionMajorIterator():size(D),curdims(0), isEnd(false) { }
        LastDimensionMajorIterator(const LastDimensionMajorIterator & lmi): arr(lmi.arr)
        , elem(lmi.elem)
        , size(0)
        , curdims(lmi.curdims)
        , isEnd(lmi.isEnd){ }
        LastDimensionMajorIterator &
        operator=(const LastDimensionMajorIterator & lmi){
            arr = lmi.arr;
            size = lmi.size;
            curdims = lmi.curdims;
            elem = lmi.elem;
            isEnd = lmi.isEnd;
            return *this;
        }
        
        friend bool operator==(const LastDimensionMajorIterator & fmi1,
                               const LastDimensionMajorIterator &fmi2){
            if(fmi1.arr!=fmi2.arr){
                return false;
            }
            if(fmi1.size!=fmi2.size){
                return false;
            }
            if(fmi1.curdims!=fmi2.curdims){
                return false;
            }
            if(fmi1.isEnd != fmi2.isEnd){
                return false;
            }
            if(fmi1.isEnd){
                return true;
            }
            return fmi1.elem == fmi2.elem;
        }
        friend bool operator!=(const LastDimensionMajorIterator & fmi1,
                               const LastDimensionMajorIterator & fmi2){
            if(fmi1.arr!=fmi2.arr){
                return true;
            }
            if(fmi1.size!=fmi2.size){
                return true;
            }
            if(fmi1.curdims!=fmi2.curdims){
                return true;
            }
            if(fmi1.isEnd != fmi2.isEnd){
                return true;
            }
            if(fmi1.isEnd){
                return false;
            }
            return fmi1.elem != fmi2.elem;
        }
        void relocate(){
            elem = &(arr->arr[curdims]);
        }
        LastDimensionMajorIterator &operator++(){
            curdims = curdims+1;
            if(curdims == size){
                curdims = curdims%size;
                isEnd = true;
            }
            elem = &(arr->arr[curdims]);
            return *this;
        }
        LastDimensionMajorIterator operator++(int){
            LastDimensionMajorIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        T &operator*() const{
            return *elem;
        }
        void printDim(){
            std::cout << curdims;
            if(isEnd){
                std::cout<<"T ";
            }
            else
                std::cout<<"F ";
        }
    };
    FirstDimensionMajorIterator fmbegin(){
        FirstDimensionMajorIterator fmi;
        fmi.arr = this;
        fmi.elem = &arr[0];
        fmi.isEnd = false;
        return fmi;
    }
    FirstDimensionMajorIterator fmend(){
        FirstDimensionMajorIterator fmi;
        fmi.arr = this;
        fmi.elem = &arr[0];
        fmi.isEnd = true;
        return fmi;
    }
    LastDimensionMajorIterator lmbegin(){
        LastDimensionMajorIterator fmi;
        fmi.arr = this;
        fmi.elem = &arr[0];
        return fmi;
    }
    LastDimensionMajorIterator lmend(){
        LastDimensionMajorIterator fmi;
        fmi.arr = this;
        fmi.elem = &arr[0];
        fmi.isEnd = true;
        return fmi;
    }
};



CLOSE_CS540
#endif /* defined(____ARBITRARY_DIMENSION_ARRAY__) */