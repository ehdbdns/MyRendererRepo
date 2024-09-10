#pragma once

#include<cmath>
#include<process.h>
#include <atlcoll.h>
#include<windowsx.h>
struct pageTableItem {
    int nextPageNote = -1;
    void* PageStart = nullptr;
};

class pageTable {
public:
    pageTable();
    void init();
    int addPageTableItems(void** pagesStart, int pageNum);
    void free(int pageNote);
    void getPages(int startPageNote, void*** pageStart, int pageNum);
    void* getPageStart(int pageNote, int pageIndex);


private:
    int pageSize;
    int pageTableItemSize = 2000;
    pageTableItem* data;
};

struct Block {
    Block* next = nullptr;
};

class allocator {
public:
    allocator();
    allocator(UINT pagesize, UINT dividecount);
    void* allocate();
    void free(void* freeBlock);
    void freeAll();
    void* getPTR();
    UINT divideCount;
    UINT BlockSize;
    UINT pageSize;
    UINT pageNote;
    UINT pageNum = 1;
private:
    Block* freeList;
    int freeNum;
    void* ptr = nullptr;
};
struct Page {
public:
    Page* next = nullptr;
    allocator* alloc = nullptr;
};
class MemoryManager {
public:
    MemoryManager();
    MemoryManager(UINT pageSizeInKB, UINT maxdividenum);
    Page* allocPage(int PageDivideNum);
    void* alloc(UINT MSize, allocator** alloc);
    UINT PageSize;
private:
    UINT MaxDivideNum;
    Page* PageList = nullptr;
    UINT PageNum = 0;
};
extern MemoryManager* memorymanager;//extern不要放在命名空间里
extern pageTable* pagetable;

namespace EST {
  

    template<class T>
    class vector {
    public:
        vector();
        ~vector();
        vector(T* startInit, int size);
        void assign(T* startInit, int size);
        void free();
        void init();
        void reserve(int cap);
        void resize(int size);
        void memset(T* start, int size, T obj);
        void memcpyPages(UINT8* dest, UINT BufferSize);
        void push_back(T obj);
        T& operator[](int index);
        vector(vector& src);
        vector& operator=(vector& src);
        int size();
        bool empty();
        T& back();
        void pop_back();
        void erase(T* erasebegin, T* eraseend, T obj);
        void clear();
        T* getbegin();
        T* getend();
        T* Getdata();
        void addData(T* dataBegin, int Size);
    private:
        T* data;
        allocator* alloc = nullptr;
        MemoryManager* MM;
        int StyleSizeInBytes;
        int Size = 0;
        int capacity = 0;
        int TNumPerPage;
        T* begin;
        T* end;
    };

    template<class T>
    vector<T>::vector() {
        init();
    }
    template<class T>
    vector<T>::~vector() {
        free();
    }
    template<class T>
    vector<T>::vector(T* startInit, int size) {//类在初始化时要么调用构造要么调用拷贝构造，声明对象后直接等于调用拷贝构造，声明后再用等于调用=重载，retrun会创建一个临时对象并调用其拷贝构造，之后将函数调用处换成临时对象。
        vector<T>::init();
        resize(size);
        for (int i = 0;i < Size;i++) {
            (*this)[i] = *startInit;
            startInit++;
        }
    }
    template<class T>
    void vector<T>::assign(T* startInit, int size) {
        resize(size);
        for (int i = 0;i < Size;i++) {
            (*this)[i] = *startInit;
            startInit++;
        }
    }
    template<class T>
    void vector<T>::free() {
        if (this->alloc == nullptr || capacity == 0)
            return;
        alloc->free((void*)data);
        Size = 0;
        capacity = 0;
    }
    template<class T>
    void vector<T>::init() {
        StyleSizeInBytes = sizeof(T);
        MM = memorymanager;
        TNumPerPage = int(MM->PageSize / StyleSizeInBytes);
    }
    template<class T>
    void vector<T>::reserve(int cap) {
        int MemorySize = cap * StyleSizeInBytes;
        T* preBegin = begin;
        void** prePagesPrt = nullptr;
        if (this->alloc != nullptr && alloc->divideCount == 0)//如果之前容器有数据，并且divideNum==0，则数据由页表管理。
            pagetable->getPages(alloc->pageNote, &prePagesPrt, alloc->pageNum);//ToDo，得到之前所有页面首地址以及页面数
        else if (this->alloc != nullptr && alloc->divideCount > 0) {    //之前有数据并且dnum》0，则数据由链表管理
            prePagesPrt = new void* [1];
            *prePagesPrt = (void*)begin;
        }
        allocator* newAllocator = nullptr;
        begin = reinterpret_cast<T*>(MM->alloc(MemorySize, &newAllocator));//分配内存，并得到第一个页面的alloctor以及第一个页面的首地址
        void** newPagesPtr = nullptr;
        if (newAllocator->divideCount == 0) {//如果新分配的页面不止一个//TODO:一旦进入这个判断里就会出现BUG！！！！！！快去改
            pagetable->getPages(newAllocator->pageNote, &newPagesPtr, newAllocator->pageNum);
            for (int i = 0;i < newAllocator->pageNum;i++) {
                T* NewPageStart = (T*)newPagesPtr[i];
                NewPageStart = new(NewPageStart)T[TNumPerPage];
                if (capacity > 0 && i < alloc->pageNum) {
                    T* preit = (T*)prePagesPrt[i];
                    for (int j = 0;j < TNumPerPage;j++) {
                        T* pushPos = ((NewPageStart)+j);
                        *pushPos = *preit;
                        preit++;
                    }
                }
            }
            data = (T*)newPagesPtr[0];
            int endPageIndex = int(Size / TNumPerPage);
            end = (T*)newPagesPtr[endPageIndex] + (Size % TNumPerPage);
        }
        else {//新分配的页面只有一个
            data = new(begin)T[cap];
            for (int i = 0;i < capacity;i++)
                begin[i] = *(preBegin + i);
            end = begin + Size;
        }
        if (this->alloc != nullptr && capacity != 0)
            alloc->free(preBegin);//释放之前的所有页面
        alloc = newAllocator;
        capacity = cap;
    }
    template<class T>
    void vector<T>::resize(int size) {
        if (size > capacity)
            reserve(2 * size);
        end = begin + size;
        Size = size;
    }
    template<class T>
    void vector<T>::memset(T* start, int size, T obj) {
        if (size > Size)
            resize(size);
        for (int i = 0;i < size;i++)
            data[start - begin + i] = obj;
    }
    template<class T>
    void vector<T>::memcpyPages(UINT8* dest, UINT BufferSize) {
        if (alloc->divideCount == 0) {
            void** pagesStart = nullptr;
            pagetable->getPages(alloc->pageNote, &pagesStart, alloc->pageNum);
            int SizeOfOnePage = StyleSizeInBytes * TNumPerPage;
            for (int i = 0;i < alloc->pageNum;i++) {
                int redundancy = (BufferSize % SizeOfOnePage == 0) ? SizeOfOnePage : BufferSize % SizeOfOnePage;
                int size = (i == alloc->pageNum - 1) ? (redundancy) : SizeOfOnePage;
                memcpy(dest + i * SizeOfOnePage, pagesStart[i], size);
            }
        }
        else {
            memcpy(dest, data, BufferSize);
        }
    }
    template<class T>
    void vector<T>::push_back(T obj) {
        if (Size + 1 > capacity)
            reserve((Size + 1) * 2);
        if (Size % TNumPerPage == 0 && Size != 0) {
            int pageIndex = Size / TNumPerPage;
            end = (T*)pagetable->getPageStart(alloc->pageNote, pageIndex);
        }
        T* push_pos = end;
        *push_pos = obj;
        end++;
        Size += 1;
    }
    template<class T>
    T& vector<T>:: operator[](int index) {
        if (index > Size - 1) {
            WriteConsole(g_hOutput, std::to_string(index).c_str(), 1, NULL, NULL);
            abort();
        }
        if (alloc->divideCount == 0) {
            int pageIndex = (index + 1) / TNumPerPage;
            T* pageStart = (T*)pagetable->getPageStart(alloc->pageNote, pageIndex);//ToDo得到某一页的首地址
            T da = data[index % TNumPerPage];
            return pageStart[index % TNumPerPage];
        }
        else
            return data[index];
    }
    template<class T>
    vector<T>::vector(vector& src)
    {
        init();
        addData(src.Getdata(), src.size());
        Size = src.size();
        capacity = src.capacity;
    }
    template<class T>
    vector<T>& vector<T>:: operator=(vector& src) {
        addData(src.Getdata(), src.size());
        Size = src.size();
        capacity = src.capacity;
        return *this;
    }
    template<class T>
    int vector<T>::size() {
        return Size;
    }
    template<class T>
    bool vector<T>::empty() {
        if (Size == 0)
            return true;
        return false;
    }
    template<class T>
    T& vector<T>::back() {
        if (Size == 0)
            abort();
        T re = *(end - 1);
        return *(end - 1);
    }
    template<class T>
    void vector<T>::pop_back() {
        Size--;
        end--;
        //if (Size < (capacity / 2))
        //    reserve(capacity / 2);
    }
    template<class T>
    void vector<T>::erase(T* erasebegin, T* eraseend, T obj) {
        for (int i = 0;i < eraseend - erasebegin;i++) {
            if (obj == *(erasebegin + i)) {
                for (int j = 0;j < Size - 1 - (erasebegin + i - begin);j++)
                    *(erasebegin + i + j) = *(erasebegin + i + 1 + j);
                Size--;
                end--;
                return;
            }
        }
    }
    template<class T>
    void vector<T>::clear() {
        Size = 0;
        end = begin;
    }
    template<class T>
    T* vector<T>::getbegin() {
        return begin;
    }
    template<class T>
    T* vector<T>::getend() {
        return end;
    }
    template<class T>
    T* vector<T>::Getdata() {
        return data;
    }
    template<class T>
    void vector<T>::addData(T* dataBegin, int Size) {
        T* it = dataBegin;
        for (int i = 0;i < Size;i++) {
            push_back(*it);
            it++;
        }
    }

}

