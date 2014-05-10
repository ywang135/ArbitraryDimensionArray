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
    throw OutOfRange("Out of Range");
}
void throwWrongTemplateException(){
    throw WrongTemplate("Wrong Template");
}
/*******************/
template<typename T>
T *ArrayOneDimension(size_t size){
    return new T[size];
}
size_t getLength(){
    return 1;
}
size_t getLength(int args, ...){
    int i;
    va_list vl;
    va_start(vl,args);
    size_t len = 1;
    typedef std::integral_constant<size_t, 0> zero_t;
    for(i=0; i< args; i++) {
        size_t val = va_arg(vl, size_t);
        // size_t ss = val;
        //typedef std::integral_constant<size_t, std::move(ss)> val_t;
        // static_assert(val_t::value > zero_t::value, "dim should be positive!");
        len = len*val;
    }
    va_end(vl);
    return len;
}
size_t* getDim(int args, ...){
    int i;
    va_list vl;
    va_start(vl,args);
    size_t* dim = new size_t[args];
    for(i=0; i< args; i++) {
        size_t val = va_arg(vl, size_t);
        // static_assert(val> 0, "dim should be positive!");
        dim[i] = val;
    }
    va_end(vl);
    return dim;
}
/*******************/
template<typename T, size_t D, size_t... Dims>
class Array{
public:
    const std::type_info& ValueType;
    Array<T, Dims...>* arr;
    size_t dimSize;
    size_t size;
    
public:
    friend class Array<T , Dims...>;
    Array(): ValueType(typeid(T)), dimSize(sizeof...(Dims)), size(D){
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
        size_t i;
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
        if(arr == array.arr){
            return *this;
        }
        size_t i;
        for(i=0; i< size; i++){
            arr[i] = array.arr[i];
        }
    }
    Array<T, Dims... >& operator[](const size_t d1) const{
        //std::cout << "dd: "<<typeid(arr).name() << std::endl;
        if(debug){
            std::cout<<"call array[] at Array. ";
            std::cout<<"Dimension of Array is "<<dimSize<<"\n";
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
            , isEnd(false){ }
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
            return fmi1.nextIterator == fmi1.nextIterator;
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
            return fmi1.nextIterator != fmi1.nextIterator;
        }
        FirstDimensionMajorIterator &operator++(){
            ++nextIterator;
            if(nextIterator.curdims == 0){
                curdims = curdims+1;
                if(curdims == size){
                    curdims = curdims%size;
                    isEnd = true;
                }
                nextIterator = arr->arr[curdims].fmbegin();
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
        fmi.nextIterator = arr[0].fmbegin();
        fmi.isEnd = true;
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
    Array(): ValueType(typeid(T)), dimSize(0), size(D){
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
        if(arr == array.arr){
            return *this;
        }
        size_t i;
        for(i=0; i< size; i++){
            arr[i] = array.arr[i];
        }
    }
    T& operator[](const size_t d1) const{
        //std::cout << "dd: "<<typeid(arr).name() << std::endl;
        if(debug){
            std::cout<<"call array[] at Array. ";
            std::cout<<"Dimension of Array is "<<dimSize<<"\n";
        }
        return arr[d1];
    }
    ~Array(){
        delete arr;
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
            return fmi1.elem == fmi1.elem;
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
            return fmi1.elem != fmi1.elem;
        }
        FirstDimensionMajorIterator &operator++(){
            curdims = curdims+1;
            if(curdims == size){
                curdims = curdims%size;
                isEnd = true;
            }
            elem = &(arr->arr[curdims]);
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

    };
    class LastDimensionMajorIterator{
    public:
        Array<T, D> *arr;
        T* elem;
        size_t size;
        size_t curdims;
        bool isEnd;
        LastDimensionMajorIterator():size(D),curdims(size-1), isEnd(false) { }
        LastDimensionMajorIterator(const LastDimensionMajorIterator & lmi): arr(lmi.arr)
        , elem(lmi.elem)
        , size(lmi.size)
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
            return fmi1.elem == fmi1.elem;
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
            return fmi1.elem != fmi1.elem;
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
            FirstDimensionMajorIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        T &operator*() const{
            return *elem;
        }
    };
    FirstDimensionMajorIterator fmbegin(){
        FirstDimensionMajorIterator fmi;
        fmi.arr = this;
        fmi.elem = &arr[0];
        return fmi;
    }
    FirstDimensionMajorIterator fmend(){
        FirstDimensionMajorIterator fmi;
        fmi.arr = this;
        fmi.elem = arr[0];
        fmi.isEnd = true;
        return fmi;
    }
};



CLOSE_CS540
#endif /* defined(____ARBITRARY_DIMENSION_ARRAY__) */