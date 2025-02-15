﻿#pragma once
#include"d3dUtil.h"
#include<cmath>
#include<process.h>
#include <atlcoll.h>
#include<windowsx.h>
#define GRS_UPPER(A,B) ((UINT)(((A)+((B)-1))&~(B - 1)))
HANDLE g_hOutput = 0;
std::unordered_map<ID3D12DescriptorHeap*, UINT>HeapOffsetTable;
#define PI 3.1415926f
struct pageTableItem {
    int nextPageNote = -1;
    void* PageStart = nullptr;
};

class pageTable {
public:
    pageTable() {
        init();
    }
    void init() {
        data = new pageTableItem[pageTableItemSize];
    }
    int addPageTableItems(void** pagesStart, int pageNum) {
        int it = 0;
        int lastPageNote = -1;
        int firstNote;
        for (int i = 0;i < pageTableItemSize;i++) {
            if (data[i].PageStart == nullptr) {
                if (it == 0)
                    firstNote = i;
                data[i].PageStart = pagesStart[it];
                if (lastPageNote != -1)
                    data[lastPageNote].nextPageNote = i;
                lastPageNote = i;
                it++;
            }
            if (it == pageNum)
                break;
            if (i == pageTableItemSize - 1)
                assert(i < pageTableItemSize);
        }
        return firstNote;
    }
    void free(int pageNote) {
        int nextNote = pageNote;
        while (nextNote > 0) {
            data[nextNote].PageStart = nullptr;
            int t = data[nextNote].nextPageNote;
            data[nextNote].nextPageNote = -1;
            nextNote = t;
        }
    }
    void getPages(int startPageNote, void*** pageStart,int pageNum) {
        int nextNote = startPageNote;
        *pageStart = new void* [pageNum];
        int it = 0;
        while (nextNote >= 0) {
            (*pageStart)[it] = data[nextNote].PageStart;
            nextNote = data[nextNote].nextPageNote;
            it++;
        }
    }
    void* getPageStart(int pageNote,int pageIndex) {
        int retNote = pageNote;
        for (int i = 0;i < pageIndex;i++) 
        {
            retNote = data[retNote].nextPageNote;
        }
        return data[retNote].PageStart;
    }


private:
    int pageSize;
    int pageTableItemSize = 2000;
    pageTableItem* data;
};
pageTable pagetable;
namespace EST {
    struct Block {
        Block* next = nullptr;
    };

    class allocator {
    public:
        allocator() = default;
        allocator(UINT pagesize, UINT dividecount) {
            pageSize = pagesize;
            divideCount = dividecount;
            ptr = operator new (pageSize);//申请一大块内存
            BlockSize = pageSize / pow(2, divideCount);//一个block占多大
            freeList = reinterpret_cast<Block*>(ptr);
            freeList->next = nullptr;//让freeList存储大块内存的首地址，并将第一个block的开头一小部分用于存储next指针
            std::uint8_t* p = reinterpret_cast<std::uint8_t*>(ptr);
            for (int j = 1;j < pow(2, divideCount);j++) {//将所有block加入freelist
                p += BlockSize;
                Block* b;
                b = reinterpret_cast<Block*>(p);
                b->next = freeList;
                freeList = b;
                freeNum++;
                
            }
        }
        void* allocate() {
            if (freeNum > 0) {
                Block* b = freeList;
                freeList = b->next;
                freeNum--;
                return (void*)b;
            }
            return nullptr;
        }
        void free(void* freeBlock) {//递归释放页表里连续的页面或者，如果divideNum为0，则此页面不由链表管理，也就不必增加链表节点，只需进行pageTable的Free即可
            if (divideCount == 0)
                pagetable.free(pageNote);
            else {
                Block* b = reinterpret_cast<Block*>(freeBlock);
                b->next = freeList;
                freeList = b;
                freeNum++;
            }
        }
        void freeAll() {

        }
        void* getPTR() {//获得大块内存的首地址
            return ptr;

        }
        UINT divideCount;
        UINT BlockSize;
        UINT pageSize;
        UINT pageNote;
        UINT pageNum=1;
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
        MemoryManager() = default;
        MemoryManager(UINT pageSizeInKB, UINT maxdividenum) {
            PageSize = pageSizeInKB * 1024;
            MaxDivideNum = maxdividenum;
            PageList = nullptr;
            //for (int i = 0;i < maxdividenum;i++)
            //    allocPage(i);
        }
        Page* allocPage(int PageDivideNum) {
            allocator* alloc = new allocator(PageSize, PageDivideNum);
            Page* p = reinterpret_cast<Page*>(reinterpret_cast<std::uint8_t*>(alloc->getPTR()) + sizeof(Block));
            p->alloc = alloc;
            p->next = PageList;
            PageList = p;
            PageNum++;
            return p;
        }
        //void freePage(void* freepage) {
        //    //判断freepage应该在页表还是链表。
        //    Page* it = PageList;
        //    Page* preit = nullptr;
        //    for (int i = 0;i < PageNum;i++) {
        //        if ((void*)it == freepage) {
        //            preit->next = it->next;
        //            it->alloc->freeAll();
        //            break;
        //        }
        //        preit = it;
        //        it = it->next;
        //    }
        //}
        void* alloc(UINT MSize,allocator**alloc) {
            int level = 0;
            int currentSize = PageSize / pow(2, MaxDivideNum);
            while (MSize > currentSize) {
                level++;
                currentSize *= 2;
            }
            Page* p = PageList;
            void* m;
            //如果divideNum==0，则不必遍历链表，因为这是页一旦被创建就会被分配，不存在剩余block，这时算出需要多少页面，for创建页面并且存入页表。
            //else 则数据一定是连续的，这是遍历链表并交给链表管理。
            if (level >= MaxDivideNum)
            {
                int allocPageNum = MSize / PageSize + 1;
                void** pagesStart = new void* [allocPageNum];
                for (int i = 0;i < allocPageNum;i++) {
                    allocator* alc = new allocator(PageSize, 0);
                    pagesStart[i] = alc->getPTR();
                    if (i == 0) {
                        alc->pageNum = allocPageNum;
                        *alloc = alc;
                    }
                }
                (*alloc)->pageNote=pagetable.addPageTableItems(pagesStart, allocPageNum);
                m = pagesStart[0];
            }
            else {
                while (1) {
                    if (p == nullptr) {
                        p = allocPage(MaxDivideNum - level);
                        m = p->alloc->allocate();
                        break;
                    }
                    if (p->alloc == nullptr) {
                        p = p->next;
                        continue;
                    }
                    if (p->alloc->divideCount == MaxDivideNum - level) {
                        m = p->alloc->allocate();
                        if (m == nullptr) {
                            p = p->next;
                            continue;
                        }
                        break;
                    }
                    p = p->next;
                }
                *alloc = p->alloc;
            }
            return m;
        }
        UINT PageSize;
    private:
        UINT MaxDivideNum;
        Page* PageList = nullptr;

        UINT PageNum = 0;
    };
    EST::MemoryManager* memorymanager = new EST::MemoryManager(16, 14);
 

    template<class T>
    class vector {
    public:
        vector() {
            init();
        }
        ~vector() {
            free();
        }
        vector(T* startInit, int size) {//类在初始化时要么调用构造要么调用拷贝构造，声明对象后直接等于调用拷贝构造，声明后再用等于调用=重载，retrun会创建一个临时对象并调用其拷贝构造，之后将函数调用处换成临时对象。
            init();
            resize(size);
            for (int i = 0;i < Size;i++) {
                (*this)[i] = *startInit;
                startInit++;
            }
        }
        void assign(T* startInit, int size) {
            resize(size);
            for (int i = 0;i < Size;i++) {
                (*this)[i] = *startInit;
                startInit++;
            }
        }
        void free() {
            if (this->alloc == nullptr||capacity==0)
                return;
            alloc->free((void*)data);
            Size = 0;
            capacity = 0;
        }
        void init() {
            StyleSizeInBytes = sizeof(T);
            MM = memorymanager;
            TNumPerPage = int(MM->PageSize / StyleSizeInBytes);
        }
        void reserve(int cap) {
            int MemorySize = cap * StyleSizeInBytes;
            T* preBegin = begin;
            void** prePagesPrt = nullptr;
            if (this->alloc != nullptr && alloc->divideCount == 0)//如果之前容器有数据，并且divideNum==0，则数据由页表管理。
                pagetable.getPages(alloc->pageNote, &prePagesPrt, alloc->pageNum);//ToDo，得到之前所有页面首地址以及页面数
            else if (this->alloc != nullptr && alloc->divideCount > 0) {    //之前有数据并且dnum》0，则数据由链表管理
                prePagesPrt = new void* [1];
                *prePagesPrt =(void*)begin ;
            }
            allocator* newAllocator = nullptr;
           begin= reinterpret_cast<T*>( MM->alloc(MemorySize,&newAllocator));//分配内存，并得到第一个页面的alloctor以及第一个页面的首地址
           void** newPagesPtr = nullptr;
           if (newAllocator->divideCount == 0) {//如果新分配的页面不止一个//TODO:一旦进入这个判断里就会出现BUG！！！！！！快去改
               pagetable.getPages(newAllocator->pageNote, &newPagesPtr,newAllocator->pageNum);
               for (int i = 0;i < newAllocator->pageNum;i++) {
                   T* NewPageStart =(T*) newPagesPtr[i];
                   NewPageStart = new(NewPageStart)T[TNumPerPage];
                   if (capacity > 0 && i < alloc->pageNum) {
                       T* preit = (T*)prePagesPrt[i];
                       for (int j = 0;j < TNumPerPage;j++) {
                           T* pushPos = ((NewPageStart) + j);
                           *pushPos = *preit;
                           preit++;
                       }
                   }
               }
               data =(T*) newPagesPtr[0];
               int endPageIndex = int(Size / TNumPerPage);
               end = (T*)newPagesPtr[endPageIndex] +(Size % TNumPerPage);
           }
           else {//新分配的页面只有一个
               data = new(begin)T[cap];
               for (int i = 0;i < capacity;i++)
                   begin[i] = *(preBegin+i);
               end = begin + Size;
           }
           if(this->alloc!=nullptr&&capacity!=0)
           alloc->free(preBegin);//释放之前的所有页面
           alloc = newAllocator;
           capacity = cap;
        }
        void resize(int size) {
            if (size > capacity)
                reserve(2 * size);
            end = begin + size;
            Size = size;
        }
        void memset(T* start, int size,T obj) {
            if (size > Size)
                resize(size);
            for (int i = 0;i < size;i++)
                data[start-begin+i] = obj;
        }
        void memcpyPages(UINT8*dest,UINT BufferSize) {
            if (alloc->divideCount == 0) {
                void** pagesStart = nullptr;
                pagetable.getPages(alloc->pageNote, &pagesStart, alloc->pageNum);
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
        void push_back(T obj) {
            if (Size+1 > capacity)
                reserve((Size+1)*2);
            if (Size % TNumPerPage == 0&&Size!=0) {
                int pageIndex = Size / TNumPerPage;
                end = (T*)pagetable.getPageStart(alloc->pageNote, pageIndex);
            }
            T* push_pos =end;
            *push_pos = obj;
            end++;
            Size += 1;
        }
        T& operator[](int index) {
            if (index > Size - 1) {
                WriteConsole(g_hOutput, std::to_string(index).c_str(),1, NULL, NULL);
                abort();
            }
            if (alloc->divideCount == 0) {
                int pageIndex = (index + 1) / TNumPerPage;
                T* pageStart = (T*)pagetable.getPageStart(alloc->pageNote, pageIndex);//ToDo得到某一页的首地址
                T da = data[index % TNumPerPage];
                return pageStart[index % TNumPerPage];
            }
            else
                return data[index];
        }
        vector( vector& src)
        {
            init();
                addData(src.Getdata(), src.size());
           Size = src.size();
           capacity = src.capacity;
        }
        vector& operator=(vector& src) {
                addData(src.Getdata(), src.size());
            Size = src.size();
           capacity = src.capacity;
          return *this;
        }
        int size() {
            return Size;
        }
        bool empty() {
            if (Size == 0)
                return true;
            return false;
        }
        T& back() {
            if (Size == 0)
                abort();
            T re = *(end - 1);
            return *(end-1);
        }
        void pop_back() {
            Size--;
            end--;
            //if (Size < (capacity / 2))
            //    reserve(capacity / 2);
        }
        void erase(T* erasebegin, T* eraseend, T obj) {
            for (int i = 0;i < eraseend - erasebegin;i++) {
                if (obj == *(erasebegin + i)) {
                    for (int j = 0;j < Size - 1-(erasebegin+i-begin);j++)
                        *(erasebegin + i+j) = *(erasebegin + i + 1+j);
                    Size--;
                    end--;
                    return;
                }
            }
        }
        void clear() {
            Size = 0;
            end = begin;
        }
        T* getbegin() {
            return begin;
        }
        T* getend() {
            return end;
        }
        T* Getdata() {
            return data;
        }
        void addData(T*dataBegin,int Size) {
            T* it = dataBegin;
            for (int i = 0;i < Size;i++) {
                push_back(*it);
                it++;
            }
        }
    private:
        T* data;
        allocator* alloc=nullptr;
        MemoryManager* MM;
        int StyleSizeInBytes;
        int Size=0;
        int capacity=0;
        int TNumPerPage;
        T* begin;
        T* end;
    };
}
struct meshdata {
    meshdata() = default;
    meshdata(meshdata& d) {
        vertices.init();
        indices.init();
        vertices = d.vertices;
        indices = d.indices;
    }
    EST::vector<XMFLOAT3> vertices;
    EST::vector<std::uint16_t> indices;

};

struct Vertex
{
    XMFLOAT4 position;
    XMFLOAT2 uv;
    XMFLOAT3 normal;
    XMFLOAT3 TangentU;
    float AOk = 0;
    XMFLOAT3 color =XMFLOAT3{-1,-1,-1};
};
struct AABBbox {
    XMFLOAT3 center;
    UINT isLeaf=0;
    XMFLOAT3 extent;
    int missIndex;
    UINT triangleStart=-1;
    UINT triangleNum=0;
};
struct triangle {
    XMFLOAT3 pos1;
    UINT matIndex;
    XMFLOAT3 pos2;
    UINT texIndex;
    XMFLOAT3 pos3;
    float pad1;
    XMFLOAT3 color;
    float pad2;
    XMFLOAT3 n;
    float pad3;
    XMFLOAT4 uv12;
    XMFLOAT2 uv3;
};
struct material {
    XMFLOAT3 albedo;
    float roughness;
    XMFLOAT3 F0;
};
struct PolygonalLight {
    float area;
    float Xstart;
    float Xend;
    float Zstart;
    float Zend;
    XMFLOAT3 color;
    XMFLOAT3 normal;
};
struct ParallelLight {
    XMFLOAT3 direction;
    XMFLOAT3 color;
};
struct SAHnode {
    SAHnode() {
        triangles.init();
    }
    SAHnode* left=nullptr;
    SAHnode* right=nullptr;
    BoundingBox box;
    EST::vector<triangle>triangles;
};
struct lastVPmat {
    XMMATRIX lastVP;
    XMMATRIX last6VP;
    UINT nframe;
    UINT filterNote;
    float FilterVariance;
};
int comparex(const void* a, const void* b)
{
    triangle first = *(triangle*)a;
    triangle second = *(triangle*)b;
    float firstg = (first.pos1.x + first.pos2.x + first.pos3.x) / 3.0f;
    float secondg = (second.pos1.x + second.pos2.x + second.pos3.x) / 3.0f;
    if (firstg < secondg)
        return -1;
    else
        return 1;
}
int comparey(const void* a, const void* b)
{
    triangle first = *(triangle*)a;
    triangle second = *(triangle*)b;
    float firstg = (first.pos1.y + first.pos2.y + first.pos3.y) / 3.0f;
    float secondg = (second.pos1.y + second.pos2.y + second.pos3.y) / 3.0f;
    if (firstg < secondg)
        return -1;
    else
        return 1;
}
int comparez(const void* a, const void* b)
{
    triangle first = *(triangle*)a;
    triangle second = *(triangle*)b;
    float firstg = (first.pos1.z + first.pos2.z + first.pos3.z) / 3.0f;
    float secondg = (second.pos1.z + second.pos2.z + second.pos3.z) / 3.0f;
    if (firstg < secondg)
        return -1;
    else
        return 1;
}
class SAHtree {
public:
    SAHtree() {
        
    }
    void init(EST::vector<triangle>tris) {
        head = new SAHnode();
        head->triangles = tris;
        BuildBoundingBox(head->box, tris.Getdata(), tris.size());
        missIndexTable.resize(500);
        missIndexTable.memset(missIndexTable.Getdata(), 500, -1);
        divide(head,0,1);
    }
    void divide(SAHnode* node,int divtype,int d) {
        if (missIndexTable[d - 1] == -1)
            missIndexTable[d - 1] = boxNum;
        else {
            SortedBoxes[missIndexTable[d - 1]].missIndex = boxNum;
            missIndexTable[d - 1] = boxNum;
            int deeper = d;
            while (missIndexTable[deeper] != -1) {
                SortedBoxes[missIndexTable[deeper]].missIndex = boxNum;
                missIndexTable[deeper] = -1;
                deeper++;
            }
        }
        boxNum++;
        depth = max(depth, d);
        AABBbox box;
        box.center = node->box.Center;
        box.extent = node->box.Extents;
        box.triangleNum = node->triangles.size();
        box.triangleStart = CurrentTriangleIndex;
        if (node->triangles.size() < 5) {
            box.isLeaf = 1;
            leafBox.push_back(node->box);
            CurrentTriangleIndex += box.triangleNum;
            SortedBoxes.push_back(box);
            for (int i = 0;i < node->triangles.size();i++)
                Triangles.push_back((node->triangles)[i]);
            return;
        }
        SortedBoxes.push_back(box);
 /*       if (divtype == 0)
            qsort(node->triangles.Getdata(), node->triangles.size(), sizeof(triangle), comparex);
        else if (divtype == 1)
            qsort(node->triangles.Getdata(), node->triangles.size(), sizeof(triangle), comparey);
        else
            qsort(node->triangles.Getdata(), node->triangles.size(), sizeof(triangle), comparez);*/
        SAHnode* left=new SAHnode;
        SAHnode* right=new SAHnode;
        float mincost = MathHelper::Infinity;
        for (int i = 1;i < node->triangles.size();i++) {
            SAHnode l;
            SAHnode r;
            EST::vector<triangle>lefttri(node->triangles.Getdata(), i);
            EST::vector<triangle>righttri(&node->triangles.Getdata()[i], node->triangles.size() - i);
            l.triangles.addData(lefttri.Getdata(),lefttri.size());
            r.triangles.addData(righttri.Getdata(),righttri.size());
            float cost = calcCost(&l)+calcCost(&r);
            if (cost < mincost) {
                left->triangles.free();
                right->triangles.free();
                left->triangles.resize(0);
                right->triangles.resize(0);
                mincost = cost;
                left->triangles.addData(lefttri.Getdata(),lefttri.size());
                left->box = l.box;
                right->box = r.box;
                right->triangles.addData(righttri.Getdata(), righttri.size());
            }
        }
        node->left = left;
        node->right = right;
        divide(node->left,(divtype+1)%3,d+1);
        divide(node->right, (divtype + 1) % 3,d+1);
    }
    float calcCost(SAHnode* node) {
        BuildBoundingBox(node->box, node->triangles.Getdata(), node->triangles.size());
        float S = 0;
        S += 2 * node->box.Extents.x * 2 * node->box.Extents.y * 2;
        S += 2 * node->box.Extents.y * 2 * node->box.Extents.z * 2;
        S += 2 * node->box.Extents.z * 2 * node->box.Extents.x * 2;
        return S * node->triangles.size();
    }
    void BuildBoundingBox(BoundingBox& box, triangle* triangles, int faceCount) {
        XMFLOAT3 f3min = { +MathHelper::Infinity,+MathHelper::Infinity ,+MathHelper::Infinity };
        XMFLOAT3 f3max = { -MathHelper::Infinity,-MathHelper::Infinity ,-MathHelper::Infinity };
        XMVECTOR v3min = XMLoadFloat3(&f3min);
        XMVECTOR v3max = XMLoadFloat3(&f3max);
        for (int i = 0;i < faceCount;i++) {
            XMVECTOR p1 = XMLoadFloat3(&triangles[i].pos1);
            XMVECTOR p2 = XMLoadFloat3(&triangles[i].pos2);
            XMVECTOR p3 = XMLoadFloat3(&triangles[i].pos3);
            XMVECTOR mi = XMVectorMin(p1, p2);
            mi = XMVectorMin(p1, p3);
            v3min = XMVectorMin(v3min, mi);
            XMVECTOR ma = XMVectorMax(p1, p2);
            ma = XMVectorMax(p1, p3);
            v3max = XMVectorMax(v3max, ma);
        }
        XMStoreFloat3(&box.Center, 0.5f * (v3min + v3max));
        XMStoreFloat3(&box.Extents, 0.5f * (v3max - v3min)+XMVectorSet(1,1,1,1));
    }
    int CurrentTriangleIndex = 0;
    EST::vector<int>missIndexTable;
    SAHnode* head;
    int depth;
    UINT boxNum=0;
    EST::vector<BoundingBox>leafBox;
    EST::vector<AABBbox>SortedBoxes;
    EST::vector<triangle>Triangles;
};



struct ResourceID {
    int HeapIndex;
    int HeapOffset;
};
struct buddyID {
    int level;
    int index;
};
class buddySystem {
public:
    buddySystem() = default;
    buddySystem(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device) {
        init(totalSizeInKB, hierarchy, device);
    }
   virtual void init(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device) {
        totalSize = totalSizeInKB * 1024;
        Hierarchy = hierarchy;
        minSize = totalSize / pow(2, hierarchy - 1);
        stateList = new EST::vector<int>[hierarchy];
        for (int i = 0;i < hierarchy;i++) {
            int num = pow(2, hierarchy - i-1 );
            stateList[i].resize(num);
            stateList[i].memset(stateList[i].Getdata(), num, 0);
        }
    }
   buddyID createPlacedBufferResourceInBS(ID3D12Resource** BufferResource, ID3D12Device4* device, UINT BufferSize) {
       buddyID ID = allocate(BufferSize);
       if (ID.index != -1) {
           ThrowIfFailed(device->CreatePlacedResource(
               Heap.Get()
               , ID.index * minSize * pow(2, ID.level)
               , &CD3DX12_RESOURCE_DESC::Buffer(BufferSize)
               , D3D12_RESOURCE_STATE_GENERIC_READ
               , nullptr
               , IID_PPV_ARGS(BufferResource)));
       }
       return ID;
   }
    buddyID allocate(UINT size) {
        buddyID ID = { -1,-1 };
        UINT currentSize = minSize;
        int level = 0;
        while (size > currentSize) {
            level++;
            currentSize *= 2;
            if (level > Hierarchy - 1)
                return ID;
        }
        for (int i = 0;i < stateList[level].size();i++) {
            if (stateList[level][i] == 0) {
                stateList[level][i] = 1;
                for (int j = level + 1;j < Hierarchy - 1;j++)
                    stateList[j][i / pow(2, j - level)] = 1;
                for (int j = level - 1;j >= 0;j--) {
                    for (int k = 0;k < pow(2, level - j);k++)
                        stateList[j][i * pow(2, level - j) + k] = 1;
                }
                ID.level = level;
                ID.index = i;
                return ID;
            }
        }
        return ID;
    }
    void free(buddyID ID) {
        stateList[ID.level][ID.index] = 0;
        for (int j = ID.level;j < Hierarchy - 1;j++) {
            int cindex = ID.index / pow(2, j - ID.level);
            stateList[j][cindex] = 0;
            if (ID.index % 2 == 0) {
                if (stateList[j][cindex + 1] == 0)
                    continue;
                break;
            }
            else {
                if (stateList[j][cindex - 1] == 0)
                    continue;
                break;
            }
        }
        for (int j = ID.level - 1;j >= 0;j--) {
            for (int k = 0;k < pow(2, ID.level - j);k++)
                stateList[j][ID.index * pow(2, ID.level - j) + k] = 0;
        }
    }
    EST::vector<int>* stateList;
    UINT totalSize;
    UINT minSize;
    UINT Hierarchy;
    ComPtr<ID3D12Heap> Heap;
};
class uploadBuddySystem :buddySystem {
public:
    uploadBuddySystem() = default;
    uploadBuddySystem(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device) {
        init(totalSizeInKB, hierarchy, device);
    }
     void init(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device)override {
        buddySystem::init(totalSizeInKB, hierarchy, device);
        D3D12_HEAP_DESC stDefaultHeapDesc = {  };
        stDefaultHeapDesc.Alignment = 0;
        stDefaultHeapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;		//上传堆类型
        stDefaultHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        stDefaultHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        stDefaultHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
        stDefaultHeapDesc.SizeInBytes = totalSize;
        ThrowIfFailed(device->CreateHeap(&stDefaultHeapDesc, IID_PPV_ARGS(&Heap)));
    }
     buddyID createPlacedBufferResourceInBS(ID3D12Resource** BufferResource, ID3D12Device4* device,UINT BufferSize) {
         return  buddySystem::createPlacedBufferResourceInBS(BufferResource, device, BufferSize);
     }
     buddyID allocate(UINT size) {
         return  buddySystem::allocate(size);
     }
     void free(buddyID ID) {
         buddySystem::free(ID);
     }
};
class defaultBuddySystem :buddySystem {
public:
    defaultBuddySystem() = default;
    defaultBuddySystem(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device) {
        init(totalSizeInKB, hierarchy, device);
    }
     void init(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device)override {
        buddySystem::init(totalSizeInKB, hierarchy, device);
        D3D12_HEAP_DESC stDefaultHeapDesc = {  };
        stDefaultHeapDesc.Alignment = 0;
        stDefaultHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;		
        stDefaultHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        stDefaultHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        stDefaultHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
        stDefaultHeapDesc.SizeInBytes = totalSize;
        ThrowIfFailed(device->CreateHeap(&stDefaultHeapDesc, IID_PPV_ARGS(&Heap)));
    }
     buddyID createPlacedBufferResourceInBS(ID3D12Resource** BufferResource, ID3D12Device4* device,UINT BufferSize) {
        return buddySystem::createPlacedBufferResourceInBS(BufferResource, device, BufferSize);
     }
     buddyID allocate(UINT size) {
        return buddySystem::allocate(size);
     }
     void free(buddyID ID) {
         buddySystem::free(ID);
     }
};
class SegregatedFreeLists {

};
class RT_DS_TextureSegregatedFreeLists:SegregatedFreeLists {
public:
    RT_DS_TextureSegregatedFreeLists() = default;
    RT_DS_TextureSegregatedFreeLists(UINT minsizeinKB, UINT listnum, ID3D12Device4* device) {
        init(minsizeinKB, listnum, device);
    }
    ~RT_DS_TextureSegregatedFreeLists() {
        delete[]DefaultHeapState;
    }
    void freeDefaultResource(ResourceID ID) {
        DefaultHeapDeadLists[ID.HeapIndex].push_back(ID.HeapOffset);
        DefaultHeapState[ID.HeapIndex][1]--;
    }
    void init(UINT minsizeinKB, UINT listnum, ID3D12Device4* device) {
        minSizeInKB = minsizeinKB;
        listNum = listnum;
        Device = device;
        DefaultHeaps.resize(listNum);
        DefaultHeapDeadLists = new EST::vector<int>[listnum];
        DefaultHeapState = new EST::vector<int>[listnum];
        for (int i = 0;i < listnum;i++) {
            DefaultHeapState[i].resize(2);
            DefaultHeapState[i].memset(DefaultHeapState[i].Getdata(), 2, 0);
        }
    }
    ResourceID createPlacedResourceInDefaultSFL(ID3D12Resource** Tex, D3D12_RESOURCE_DESC* TextureDesc, D3D12_CLEAR_VALUE* dsclear) {
        UINT currentSize = minSizeInKB*1024;
        int level = 0;
        ResourceID ID = { -1,-1 };
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT  TexLayouts = {};
        UINT                                nNumRows = {};
        UINT64                              n64RowSizeInBytes = {};
        UINT64                              n64TotalBytes = 0;
        Device->GetCopyableFootprints(TextureDesc, 0, 1, 0, &TexLayouts, &nNumRows, &n64RowSizeInBytes, &n64TotalBytes);
        while (currentSize < n64TotalBytes) {
            currentSize *= 2;
            level++;
        }
        if (level > listNum - 1)
            return ID;
        int NumResource = pow(2, 3 - min(3, level));
        int SizeResource = pow(2, level) * minSizeInKB*1024;
        if (DefaultHeapState[level][0] == 0)
        {
            DefaultHeapState[level][0] = 1;
            D3D12_HEAP_DESC stDefaultHeapDesc = {  };
            stDefaultHeapDesc.Alignment = 0;
            stDefaultHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;		//上传堆类型
            stDefaultHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            stDefaultHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            stDefaultHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
            stDefaultHeapDesc.SizeInBytes = SizeResource * NumResource;
            auto h = (&DefaultHeaps[level]);
            ThrowIfFailed(Device->CreateHeap(&stDefaultHeapDesc, IID_PPV_ARGS(&DefaultHeaps[level])));
        }
        if (DefaultHeapState[level][1] < NumResource)
        {
            int offset;
            if (!DefaultHeapDeadLists[level].empty())
            {
                offset = DefaultHeapDeadLists[level].back();
                DefaultHeapDeadLists[level].pop_back();
            }
            else
                offset = DefaultHeapState[level][1];
            ThrowIfFailed(Device->CreatePlacedResource(
                DefaultHeaps[level].Get()
                , offset * SizeResource
                , TextureDesc
                , D3D12_RESOURCE_STATE_GENERIC_READ
                , dsclear
                , IID_PPV_ARGS(Tex)));
            DefaultHeapState[level][1]++;
            ID.HeapIndex = level;
            ID.HeapOffset = offset;
            return ID;
        }
        return ID;
    }
private:
    ID3D12Device4* Device;
    EST::vector<ComPtr<ID3D12Heap>>DefaultHeaps;
    UINT minSizeInKB;
    UINT listNum;
    EST::vector<int>(* DefaultHeapDeadLists);
    EST::vector<int>(* DefaultHeapState);
};
class NON_RT_DS_TextureSegregatedFreeLists :SegregatedFreeLists {
public:
    NON_RT_DS_TextureSegregatedFreeLists() = default;
    NON_RT_DS_TextureSegregatedFreeLists(UINT minsizeinKB, UINT listnum, ID3D12Device4* device) {
        init(minsizeinKB,listnum,device);
    }
    ~NON_RT_DS_TextureSegregatedFreeLists() {
        delete[]UploadHeapState;
        delete[]DefaultHeapState;
        delete[]UploadHeapDeadLists;
        delete[]DefaultHeapDeadLists;
    }
    void freeUploadResource(ResourceID ID) {
        UploadHeapDeadLists[ID.HeapIndex].push_back(ID.HeapOffset);
        UploadHeapState[ID.HeapIndex][1]--;
    }
    void freeDefaultResource(ResourceID ID) {
        DefaultHeapDeadLists[ID.HeapIndex].push_back(ID.HeapOffset);
        DefaultHeapState[ID.HeapIndex][1]--;
    }
    void init(UINT minsizeinKB, UINT listnum, ID3D12Device4* device) {
        minSizeInKB = minsizeinKB;
        listNum = listnum;
        Device = device;
        UploadHeaps.resize(listNum);
        DefaultHeaps.resize(listNum);
        UploadHeapState = new EST::vector<int>[listnum];
        DefaultHeapState = new EST::vector<int>[listnum];
        UploadHeapDeadLists = new EST::vector<int>[listnum];
        DefaultHeapDeadLists = new EST::vector<int>[listnum];
        for (int i = 0;i < listnum;i++) {
            UploadHeapState[i].resize(2);
            DefaultHeapState[i].resize(2);
            UploadHeapState[i].memset(UploadHeapState[i].Getdata(), 2, 0);
            DefaultHeapState[i].memset(DefaultHeapState[i].Getdata(), 2, 0);
        }
    }
    ResourceID createPlacedResourceInUploadTexSFLHeap(ID3D12Resource**Tex,UINT BufferSize) {
        UINT currentSize = minSizeInKB*1024;
        int level = 0;
        ResourceID ID = {-1,-1};
        while (currentSize < BufferSize) {
            currentSize *= 2;
            level++;
        }
        if (level > listNum-1)
            return ID;
        int NumResource = pow(2, 3 - min(3, level));
        int SizeResource = pow(2, level) * minSizeInKB*1024;
        if (UploadHeapState[level][0] == 0)
        {
            UploadHeapState[level][0] = 1;
            D3D12_HEAP_DESC stUploadHeapDesc = {  };
                stUploadHeapDesc.Alignment = 0;
                stUploadHeapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;		//上传堆类型
                stUploadHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
                stUploadHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
                stUploadHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
                stUploadHeapDesc.SizeInBytes = SizeResource *NumResource;
                ThrowIfFailed(Device->CreateHeap(&stUploadHeapDesc, IID_PPV_ARGS(&UploadHeaps[level])));
        }
        if (UploadHeapState[level][1] < NumResource)
        {
            int offset;
            if (!UploadHeapDeadLists[level].empty())
            {
                offset = UploadHeapDeadLists[level].back();
                UploadHeapDeadLists[level].pop_back();
            }
            else
                offset = UploadHeapState[level][1];
            ThrowIfFailed(Device->CreatePlacedResource(
                UploadHeaps[level].Get()
                , offset* SizeResource
                , &CD3DX12_RESOURCE_DESC::Buffer(BufferSize)
                , D3D12_RESOURCE_STATE_GENERIC_READ
                , nullptr
                , IID_PPV_ARGS(Tex)));
            UploadHeapState[level][1]++;//这个Heap存Resource的数量
            ID.HeapIndex = level;
            ID.HeapOffset = offset;
            return ID;
        }
        return ID;
    }
    ResourceID createPlacedResourceInDefaultSFL(ID3D12Resource** Tex, D3D12_RESOURCE_DESC* TextureDesc) {
        UINT currentSize = minSizeInKB*1024;
        int level = 0;
        ResourceID ID = { -1,-1 };
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT  TexLayouts = {};
        UINT                                nNumRows = {};
        UINT64                              n64RowSizeInBytes = {};
        UINT64                              n64TotalBytes = 0;
        Device->GetCopyableFootprints(TextureDesc, 0, 1, 0, &TexLayouts, &nNumRows, &n64RowSizeInBytes, &n64TotalBytes);
        while (currentSize <= n64TotalBytes) {
            currentSize *= 2;
            level++;
        }
        if (level > listNum - 1)
            return ID;
        int NumResource = pow(2, 3 - min(3, level));
        int SizeResource = pow(2, level) * minSizeInKB * 1024;
        if (DefaultHeapState[level][0] == 0)
        {
            DefaultHeapState[level][0] = 1;
            D3D12_HEAP_DESC stDefaultHeapDesc = {  };
            stDefaultHeapDesc.Alignment = 0;
            stDefaultHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;		//上传堆类型
            stDefaultHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            stDefaultHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            stDefaultHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
            stDefaultHeapDesc.SizeInBytes = SizeResource * NumResource;
            ThrowIfFailed(Device->CreateHeap(&stDefaultHeapDesc, IID_PPV_ARGS(&DefaultHeaps[level])));
        }
        if (DefaultHeapState[level][1] < NumResource)
        {
            int offset;
            if (!DefaultHeapDeadLists[level].empty())
            {
                offset = DefaultHeapDeadLists[level].back();
                DefaultHeapDeadLists[level].pop_back();
            }
            else
                offset = DefaultHeapState[level][1];
            ThrowIfFailed(Device->CreatePlacedResource(
                DefaultHeaps[level].Get()
                , offset * SizeResource
                , TextureDesc
                , D3D12_RESOURCE_STATE_GENERIC_READ
                , nullptr
                , IID_PPV_ARGS(Tex)));
            DefaultHeapState[level][1]++;
            ID.HeapIndex = level;
            ID.HeapOffset = offset;
            return ID;
        }
        return ID;
    }
private:
    ID3D12Device4* Device;
    EST::vector<ComPtr<ID3D12Heap>>UploadHeaps;
    EST::vector<ComPtr<ID3D12Heap>>DefaultHeaps;
    EST::vector<int>* UploadHeapDeadLists;
    EST::vector<int>* DefaultHeapDeadLists;
    UINT minSizeInKB;
    UINT listNum;
    EST::vector<int>*UploadHeapState;
    EST::vector<int>*DefaultHeapState;
};
class ResourceItem {

};
class SamplerResourceItem :ResourceItem {
public:
    SamplerResourceItem() = default;
    SamplerResourceItem(ID3D12Device4* device, ID3D12DescriptorHeap* samplerheap, D3D12_SAMPLER_DESC samplerDesc, int offsetInHeap) {
        init(device, samplerheap, samplerDesc, offsetInHeap);
    }
    void init(ID3D12Device4* device, ID3D12DescriptorHeap* samplerheap, D3D12_SAMPLER_DESC samplerDesc, int offsetInHeap) {
        samplerHeap = samplerheap;
        samplerOffset = offsetInHeap;
        CD3DX12_CPU_DESCRIPTOR_HANDLE samplerHandel(samplerheap->GetCPUDescriptorHandleForHeapStart());
        device->CreateSampler(&samplerDesc, samplerHandel);
        HeapOffsetTable[samplerHeap]++;
    }
    CD3DX12_GPU_DESCRIPTOR_HANDLE getSampler() {
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(samplerHeap->GetGPUDescriptorHandleForHeapStart(), samplerOffset, SamplerSize);
    }
    ID3D12DescriptorHeap* samplerHeap = nullptr;
    int samplerOffset;
private:
    UINT SamplerSize;
};
class TextureResourceItem :ResourceItem {//先初始化，然后调用创建资源函数，然后调用创建SRV函数
public:
    TextureResourceItem() = default;
    TextureResourceItem(ID3D12Device4* device, ID3D12DescriptorHeap* srvuavheap, ID3D12DescriptorHeap* rtvheap, ID3D12DescriptorHeap* dsvheap, bool issticker,int texindex) {
        init(device, srvuavheap, rtvheap, dsvheap, issticker,texindex);
    }
    void init(ID3D12Device4* device, ID3D12DescriptorHeap* srvuavheap, ID3D12DescriptorHeap* rtvheap, ID3D12DescriptorHeap* dsvheap, bool issticker,int texindex) {
        SRVUAVincrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        DSVincrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        RTVincrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        RTVHeap = rtvheap;
        SRVUAVHeap = srvuavheap;
        DSVHeap = dsvheap;
        isSticker = issticker;
        TextureIndex = texindex;
    }
    void setTextureToRI(ComPtr<ID3D12Resource> tex) {
        Texture = tex;
    }
    ID3D12Resource* getResource() {
        return Texture.Get();
    }
    void createStickerTex(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, wchar_t* fileName) {
        if (!isSticker)
        {
            return;
        }
       CreateDDSTextureFromFile12(device, cmdlist, fileName, Texture, TextureUpload);//这个库函数是用提交方式创建的，所以我们就让系统自己管理贴图资源,函数过程：loadDDS（）获得上传资源大小和已经创建好的默认堆资源，以及subresource，之后创建上传堆资源，之后updatesubresource
    }
    void createNON_RT_DS_WritableTex(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, D3D12_RESOURCE_DESC* TextureDesc, std::unordered_map<std::string, std::unique_ptr< NON_RT_DS_TextureSegregatedFreeLists>>*NONRTDSSFLTable) {
        if (isSticker)
            return;
        for (std::unordered_map<std::string, std::unique_ptr< NON_RT_DS_TextureSegregatedFreeLists>>::iterator it = NONRTDSSFLTable->begin();it != NONRTDSSFLTable->end();it++) {
           ID=it->second->createPlacedResourceInDefaultSFL(&Texture, TextureDesc);
            if (ID.HeapIndex != -1)
                break;
        }
    }
    void createRT_DS_WritableTex(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, D3D12_RESOURCE_DESC* TextureDesc, D3D12_CLEAR_VALUE* dsclear, RT_DS_TextureSegregatedFreeLists* sfl) {
        if (isSticker)
            return;
        RT_DS_SFL = sfl;
        ID = RT_DS_SFL->createPlacedResourceInDefaultSFL(&Texture, TextureDesc,dsclear);
    }
    void createSRVforResourceItem(D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, ID3D12Device4* device, UINT SRVOffsetInHeap) {
        srvDesc->Format = Texture->GetDesc().Format;
        if (srvDesc->ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2D) {
            srvDesc->Texture2D.MipLevels = Texture->GetDesc().MipLevels;
        }
        else if (srvDesc->ViewDimension == D3D12_SRV_DIMENSION_TEXTURE3D) {
            srvDesc->Texture3D.MipLevels = Texture->GetDesc().MipLevels;
        }
        else if (srvDesc->ViewDimension == D3D12_SRV_DIMENSION_TEXTURECUBE) {
            srvDesc->TextureCube.MipLevels = Texture->GetDesc().MipLevels;
        }
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SRVUAVHeap->GetCPUDescriptorHandleForHeapStart(), SRVOffsetInHeap, SRVUAVincrementSize);
        device->CreateShaderResourceView(Texture.Get(), srvDesc, cpuHandle);
        SRVOffsetList.push_back(SRVOffsetInHeap);
        HeapOffsetTable[SRVUAVHeap]++;
    }
    void createUAVforResourceItem(D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc, ID3D12Device4* device, UINT UAVOffsetInHeap, ID3D12GraphicsCommandList* cmdlist) {
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SRVUAVHeap->GetCPUDescriptorHandleForHeapStart(), UAVOffsetInHeap, SRVUAVincrementSize);
        device->CreateUnorderedAccessView(Texture.Get(), nullptr, uavDesc, cpuHandle);
        UAVOffsetList.push_back(UAVOffsetInHeap);
        HeapOffsetTable[SRVUAVHeap]++;
        cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition
        (Texture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
    }
    void createDSVforResourceItem(D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc, ID3D12Device4* device, UINT DSVOffsetInHeap, ID3D12GraphicsCommandList* cmdlist) {
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(DSVHeap->GetCPUDescriptorHandleForHeapStart(), DSVOffsetInHeap, DSVincrementSize);
        device->CreateDepthStencilView(Texture.Get(), dsvDesc, dsvHandle);
        DSVoffset = DSVOffsetInHeap;
        HeapOffsetTable[DSVHeap]++;
        cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition
        (Texture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
    }
    void createRTVforResourceItem(D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc, ID3D12Device4* device, UINT RTVOffsetInHeap, ID3D12GraphicsCommandList* cmdlist) {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(RTVHeap->GetCPUDescriptorHandleForHeapStart(), RTVOffsetInHeap, RTVincrementSize);
        device->CreateRenderTargetView(Texture.Get(), rtvDesc, rtvHandle);
        RTVoffset = RTVOffsetInHeap;
        HeapOffsetTable[RTVHeap]++;
        cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition
        (Texture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
    }
    CD3DX12_GPU_DESCRIPTOR_HANDLE getSRVGPU(int SRVnote) {
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(SRVUAVHeap->GetGPUDescriptorHandleForHeapStart(), SRVOffsetList[SRVnote], SRVUAVincrementSize);
    }
    CD3DX12_GPU_DESCRIPTOR_HANDLE getUAVGPU(int UAVnote) {
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(SRVUAVHeap->GetGPUDescriptorHandleForHeapStart(), UAVOffsetList[UAVnote], SRVUAVincrementSize);
    }
    CD3DX12_CPU_DESCRIPTOR_HANDLE getRTVCPU() {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(RTVHeap->GetCPUDescriptorHandleForHeapStart(), RTVoffset, RTVincrementSize);
    }
    CD3DX12_CPU_DESCRIPTOR_HANDLE getDSVCPU() {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(DSVHeap->GetCPUDescriptorHandleForHeapStart(), DSVoffset, DSVincrementSize);
    }
    ID3D12DescriptorHeap* RTVHeap = nullptr;
    ID3D12DescriptorHeap* DSVHeap = nullptr;
    ID3D12DescriptorHeap* SRVUAVHeap = nullptr;
    NON_RT_DS_TextureSegregatedFreeLists* NON_RT_DS_SFL = nullptr;
    RT_DS_TextureSegregatedFreeLists* RT_DS_SFL = nullptr;
    ResourceID ID;
    int RTVoffset;
    int DSVoffset;
    EST::vector<int>SRVOffsetList;
    EST::vector<int>UAVOffsetList;
    int TextureIndex;
private:
    bool isSticker = true;
    UINT SRVUAVincrementSize;
    UINT DSVincrementSize;
    UINT RTVincrementSize;
    ComPtr< ID3D12Resource> Texture;
    ComPtr< ID3D12Resource> TextureUpload;
};
template<class T>
class ConstantBufferResourceItem :ResourceItem {//管理cbv以及对应结构体以及资源本身,构造函数创建资源，并map，更新函数可以memcpy更新，但创建完资源项得自行创建CBV并赋值偏移量
public:
    ConstantBufferResourceItem() = default;
    ConstantBufferResourceItem(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, T* strP, bool isstatic, ID3D12DescriptorHeap* cbvheap,uploadBuddySystem*upBS,defaultBuddySystem*defBS) {
        init(device, cmdlist, strP, isstatic, cbvheap,upBS,defBS);
    }
    void init(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, T* strP, bool isstatic, ID3D12DescriptorHeap* cbvheap, uploadBuddySystem* upbs, defaultBuddySystem* defbs) {
        this->isStatic = isstatic;
        this->CBVHeap = cbvheap;
        this->upBS = upbs;
        this->defBS = defbs;
        CBVoffset = HeapOffsetTable[CBVHeap];
        SRVCBVUAVincrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        this->str = *strP;
        D3D12_RANGE range = { 0,0 };
        strSize = sizeof(T) + 255 & ~255;
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.SizeInBytes = strSize;
        if (isStatic) {
           defID= defBS->createPlacedBufferResourceInBS(&constantBufferDefault, device,strSize);
           upID= upBS->createPlacedBufferResourceInBS(&constantBufferUpload, device,strSize);
           constantBufferUpload->Map(0, &range, reinterpret_cast<void**>(&cbmapped));
           memcpy(cbmapped, &str, strSize);
           cmdlist->CopyBufferRegion(constantBufferDefault.Get(), 0, constantBufferUpload.Get(), 0, strSize);
            //D3D12_SUBRESOURCE_DATA subResourceData;
            //subResourceData.pData = &str;
            //subResourceData.RowPitch = strSize;
            //subResourceData.SlicePitch = subResourceData.RowPitch;
            //UpdateSubresources<1>(cmdlist, constantBufferDefault.Get(), constantBufferUpload.Get(), 0, 0, 1, &subResourceData);//这个函数只需先创建两个资源，然后他会帮你完成Map memcpy、unmap copyregion等操作
            cbvDesc.BufferLocation = constantBufferDefault->GetGPUVirtualAddress();
            //upBS->free(upID);//不要释放上传堆，他还有用
        }
        else {
            upID=upBS->createPlacedBufferResourceInBS(&constantBufferUpload, device, strSize);
            constantBufferUpload->Map(0, &range, reinterpret_cast<void**>(&cbmapped));
            memcpy(cbmapped, &str, strSize);
            cbvDesc.BufferLocation = constantBufferUpload->GetGPUVirtualAddress();
        }
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CBVHeap->GetCPUDescriptorHandleForHeapStart(), CBVoffset, SRVCBVUAVincrementSize);
        device->CreateConstantBufferView(&cbvDesc, cpuHandle);
        HeapOffsetTable[CBVHeap]++;
    }
    void updateCB(T* strP) {
        if (isStatic)
            return;
        str = *strP;
        memcpy(cbmapped, &str, strSize);
    }
    CD3DX12_GPU_DESCRIPTOR_HANDLE getCBVGPU() {
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(CBVHeap->GetGPUDescriptorHandleForHeapStart(), CBVoffset, SRVCBVUAVincrementSize);
    }
    ID3D12DescriptorHeap* CBVHeap = nullptr;

    UINT strSize;
    int CBVoffset;
    T str;
    bool isStatic = true;
    uploadBuddySystem* upBS = nullptr;
    defaultBuddySystem* defBS = nullptr;
    buddyID upID;
    buddyID defID;
private:
    byte* cbmapped = nullptr;
    UINT SRVCBVUAVincrementSize;
    ComPtr< ID3D12Resource> constantBufferDefault;
    ComPtr< ID3D12Resource> constantBufferUpload;
};
template<class T>
class StructureBufferResourceItem :ResourceItem {//管理cbv以及对应结构体以及资源本身,构造函数创建资源，并map，更新函数可以memcpy更新，但创建完资源项得自行创建CBV并赋值偏移量
public:
    StructureBufferResourceItem() = default;
    StructureBufferResourceItem(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, EST::vector<T>*vec, bool isstatic, ID3D12DescriptorHeap* cbvheap, uploadBuddySystem* upBS, defaultBuddySystem* defBS,UINT elementNum) {
        init(device, cmdlist, vec, isstatic, cbvheap, upBS, defBS,elementNum);
    }
    void init(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, EST::vector<T>*vec, bool isstatic, ID3D12DescriptorHeap* srvHeap, uploadBuddySystem* upbs, defaultBuddySystem* defbs, UINT elementNum) {
        this->isStatic = isstatic;
        this->SRVHeap = srvHeap;
        this->upBS = upbs;
        this->defBS = defbs;
        SRVCBVUAVincrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        this->str = vec->Getdata();
        D3D12_RANGE range = { 0,0 };
        strSize = (sizeof(T)*elementNum) + 255 & ~255;
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = elementNum;
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        srvDesc.Buffer.StructureByteStride = sizeof(T);
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        if (isStatic) {
            defID = defBS->createPlacedBufferResourceInBS(&StructureBufferDefault, device, strSize);
            upID = upBS->createPlacedBufferResourceInBS(&StructureBufferUpload, device, strSize);
            StructureBufferUpload->Map(0, &range, reinterpret_cast<void**>(&cbmapped));
            vec->memcpyPages(cbmapped, strSize);
            cmdlist->CopyBufferRegion(StructureBufferDefault.Get(), 0, StructureBufferUpload.Get(), 0, strSize);
       /*     D3D12_SUBRESOURCE_DATA subResourceData;
            subResourceData.pData = str;
            subResourceData.RowPitch = strSize;
            subResourceData.SlicePitch = subResourceData.RowPitch;
            UpdateSubresources<1>(cmdlist, StructureBufferDefault.Get(), StructureBufferUpload.Get(), 0, 0, 1, &subResourceData);*///这个函数只需先创建两个资源，然后他会帮你完成Map memcpy、unmap copyregion等操作
            CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SRVHeap->GetCPUDescriptorHandleForHeapStart(), HeapOffsetTable[SRVHeap], SRVCBVUAVincrementSize);
            device->CreateShaderResourceView(StructureBufferDefault.Get(), &srvDesc, cpuHandle);
            SRVoffset = HeapOffsetTable[SRVHeap];
            HeapOffsetTable[SRVHeap]++;
            //upBS->free(upID);//不要释放上传堆，他还有用
        }
        else {
            upID = upBS->createPlacedBufferResourceInBS(&StructureBufferUpload, device, strSize);
            StructureBufferUpload->Map(0, &range, reinterpret_cast<void**>(&cbmapped));
            vec->memcpyPages(cbmapped, strSize);
           // memcpy(cbmapped, str, strSize);
            CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SRVHeap->GetCPUDescriptorHandleForHeapStart(), HeapOffsetTable[SRVHeap], SRVCBVUAVincrementSize);
            device->CreateShaderResourceView(StructureBufferUpload.Get(), &srvDesc, cpuHandle);
            SRVoffset = HeapOffsetTable[SRVHeap];
            HeapOffsetTable[SRVHeap]++;
        }
    }
    void updateCB(EST::vector<T>* vec) {
        if (isStatic)
            return;
        str = vec->Getdata();
        vec->memcpyPages(cbmapped, strSize);
      //  memcpy(cbmapped, str, strSize);
    }
    CD3DX12_GPU_DESCRIPTOR_HANDLE getSRVGPU() {
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(SRVHeap->GetGPUDescriptorHandleForHeapStart(), SRVoffset, SRVCBVUAVincrementSize);
    }
    ID3D12DescriptorHeap* SRVHeap = nullptr;

    UINT strSize;
    int SRVoffset;
    T* str;
    bool isStatic = true;
    uploadBuddySystem* upBS = nullptr;
    defaultBuddySystem* defBS = nullptr;
    buddyID upID;
    buddyID defID;
private:
    byte* cbmapped = nullptr;
    UINT SRVCBVUAVincrementSize;
    ComPtr< ID3D12Resource> StructureBufferDefault;
    ComPtr< ID3D12Resource> StructureBufferUpload;
};
struct GeometryItem {//构造函数传入顶点及其索引，然后调用创建静态or动态顶点，最后获取其vbvibv即可；
public:
    GeometryItem() = default;
    ~GeometryItem() {
        freeVertexAndIndex();
    }
    template <typename T>
    void createDynamicGeo(ID3D12Device4* device, EST::vector<T>* vertices, EST::vector<std::uint16_t>* indices, std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>*upBSTable) {
        //this->vertices = vertices;
        //this->indices = indices;
        indexNum = indices->size();
        vbsize = (UINT)sizeof(T) * (UINT)vertices->size();
        ibsize = (UINT)sizeof(std::uint16_t) * (UINT)indices->size();
     //顶点缓冲及索引缓冲初始状态为common效率最佳
        for (std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>::iterator it = upBSTable->begin();it != upBSTable->end();it++) {
            it->second->createPlacedBufferResourceInBS(&VertexBufferUpload, device, vbsize);
            if (VertexBufferUpload.Get() != nullptr)
                break;
        }
        for (std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>::iterator it = upBSTable->begin();it != upBSTable->end();it++) {
            it->second->createPlacedBufferResourceInBS(&IndexBufferUpload, device, vbsize);
            if (IndexBufferUpload.Get() != nullptr)
                break;
        }
        D3D12_RANGE range = { 0,0 };
        VertexBufferUpload->Map(0, &range, reinterpret_cast<void**>(&vmapped));
        IndexBufferUpload->Map(0, &range, reinterpret_cast<void**>(&imapped));

        vertices->memcpyPages(vmapped, vbsize);
        indices->memcpyPages(imapped, ibsize);
       // memcpy(vmapped, vertices->Getdata(), vbsize);
       // memcpy(imapped, indices->Getdata(), ibsize);
        VertexBufferUpload->Unmap(0, nullptr);
        IndexBufferUpload->Unmap(0, nullptr);
            vbv.BufferLocation = VertexBufferUpload->GetGPUVirtualAddress();
            vbv.SizeInBytes = vbsize;
            vbv.StrideInBytes = sizeof(T);
            ibv.Format = DXGI_FORMAT_R16_UINT;
            ibv.SizeInBytes = ibsize;
            ibv.BufferLocation = IndexBufferUpload->GetGPUVirtualAddress();
    }
    template <typename T>
    void updateVB(EST::vector<T>*vec) {
        vec->memcpyPages(vmapped, vbsize);
        //memcpy(vmapped, vp, vbsize);
    }
    template <typename T>
    void createStaticGeo(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, EST::vector<T>* vertices, EST::vector<std::uint16_t>* indices, std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>*upBSTable, std::unordered_map<std::string, std::unique_ptr< defaultBuddySystem>>*defBSTable) {//这之后必须excute
        createDynamicGeo<T>(device,vertices, indices, upBSTable);
        for (std::unordered_map<std::string, std::unique_ptr< defaultBuddySystem>>::iterator it = defBSTable->begin();it != defBSTable->end();it++) {
            it->second->createPlacedBufferResourceInBS(&VertexBufferDefault, device, vbsize);
            if (VertexBufferDefault.Get() != nullptr)
                break;
        }
        for (std::unordered_map<std::string, std::unique_ptr< defaultBuddySystem>>::iterator it = defBSTable->begin();it != defBSTable->end();it++) {
            it->second->createPlacedBufferResourceInBS(&IndexBufferDefault, device, vbsize);
            if (IndexBufferDefault.Get() != nullptr)
                break;
        }

        cmdlist->CopyBufferRegion(VertexBufferDefault.Get(),0,VertexBufferUpload.Get(),0,vbsize);
        cmdlist->CopyBufferRegion(IndexBufferDefault.Get(),0,IndexBufferUpload.Get(),0,ibsize);
   /*     D3D12_SUBRESOURCE_DATA subResourceData;
        subResourceData.pData = vertices->Getdata();
        subResourceData.RowPitch = vbsize;
        subResourceData.SlicePitch = subResourceData.RowPitch;
        UpdateSubresources<1>(cmdlist, VertexBufferDefault.Get(), VertexBufferUpload.Get(), 0, 0, 1, &subResourceData);
        subResourceData.pData = indices->Getdata();
        subResourceData.RowPitch = ibsize;
        subResourceData.SlicePitch = subResourceData.RowPitch;
        UpdateSubresources<1>(cmdlist, IndexBufferDefault.Get(), IndexBufferUpload.Get(), 0, 0, 1, &subResourceData);*/



        vbv.BufferLocation = VertexBufferDefault->GetGPUVirtualAddress();
        vbv.SizeInBytes = vbsize;
        vbv.StrideInBytes = sizeof(T);
        ibv.Format = DXGI_FORMAT_R16_UINT;
        ibv.SizeInBytes = ibsize;
        ibv.BufferLocation = IndexBufferDefault->GetGPUVirtualAddress();
    }
    D3D12_VERTEX_BUFFER_VIEW* getVBV() {
        return &vbv;
    }
    D3D12_INDEX_BUFFER_VIEW* getIBV() {
        return &ibv;
    }
    void freeVertexAndIndex() {
        //vertices->~vector();
        //indices->~vector();
    }
    UINT indexNum;
private:
    UINT vbsize;
    UINT ibsize;
    UINT8* vmapped = nullptr;
    UINT8* imapped = nullptr;
    //EST::vector<Vertex>* vertices;
    //EST::vector<std::uint16_t>* indices;
    D3D12_VERTEX_BUFFER_VIEW vbv;
    D3D12_INDEX_BUFFER_VIEW ibv;
    ComPtr< ID3D12Resource> VertexBufferDefault;
    ComPtr< ID3D12Resource> VertexBufferUpload;
    ComPtr< ID3D12Resource> IndexBufferDefault;
    ComPtr< ID3D12Resource> IndexBufferUpload;

};
struct RenderItem {
    RenderItem() = default;
    RenderItem(GeometryItem* geo, int instancenum, int basevertex, int startindex, int indexnum, int startinstance, D3D12_PRIMITIVE_TOPOLOGY primitive, objectconstant* objc, ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, ID3D12DescriptorHeap* CBVHeap, uploadBuddySystem* upBS, defaultBuddySystem* defBS) {
        init(geo, instancenum, basevertex, startindex, indexnum, startinstance, primitive, objc, device, cmdlist, CBVHeap,upBS,defBS);
    }
    void init(GeometryItem* geo, int instancenum, int basevertex, int startindex, int indexnum, int startinstance, D3D12_PRIMITIVE_TOPOLOGY primitive, objectconstant* objc, ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, ID3D12DescriptorHeap* CBVHeap, uploadBuddySystem* upBS, defaultBuddySystem* defBS) {
        Geo = geo;
        InstanceNum = instancenum;
        baseVertex = basevertex;
        startIndex = startindex;
        indexNum = indexnum;
        startInstance = startinstance;
        Primitive = primitive;
        auto ptr = std::make_unique< ConstantBufferResourceItem<objectconstant>>(device, cmdlist, objc, true, CBVHeap,upBS,defBS);//让渲染项管理一个资源项
        objconstantRI = std::move(ptr);
    }
    std::unique_ptr< ConstantBufferResourceItem<objectconstant>>objconstantRI;
    D3D12_PRIMITIVE_TOPOLOGY Primitive;
    GeometryItem* Geo;
    int InstanceNum;
    int baseVertex;
    int startIndex;
    int indexNum;
    int startInstance;
    int threadNote;
};
struct RootSignatureItem {
public:
    RootSignatureItem() = default;
    RootSignatureItem(ID3D12RootSignature* rs, int descriptorNum, int srvnum, int cbvnum, int uavnum, int samplernum, CD3DX12_DESCRIPTOR_RANGE* dt) {
        init(rs, descriptorNum, dt);
    }
    void init(ID3D12RootSignature* rs, int descriptorNum, CD3DX12_DESCRIPTOR_RANGE* dt) {
        this->rs = rs;
        for (int i = 0;i < descriptorNum;i++) {
            D3D12_DESCRIPTOR_RANGE_TYPE type = dt[i].RangeType;
            if (type == D3D12_DESCRIPTOR_RANGE_TYPE_SRV) {
                SRVDescriptorTableIndex.push_back(i);
                SRVNum++;
            }
            if (type == D3D12_DESCRIPTOR_RANGE_TYPE_CBV) {
                CBVDescriptorTableIndex.push_back(i);
                CBVNum++;
            }
            if (type == D3D12_DESCRIPTOR_RANGE_TYPE_UAV) {
                UAVDescriptorTableIndex.push_back(i);
                UAVNum++;
            }
            if (type == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER) {
                SamplerDescriptorTableIndex.push_back(i);
                SamplerNum++;
            }
        }
    }
    int getSRVTableIndex(int srvIndex) {
        return SRVDescriptorTableIndex[srvIndex];
    }
    int getCBVTableIndex(int cbvIndex) {
        return CBVDescriptorTableIndex[cbvIndex];
    }
    int getUAVTableIndex(int uavIndex) {
        return UAVDescriptorTableIndex[uavIndex];
    }
    int getSamplerTableIndex(int samplerIndex) {
        return SamplerDescriptorTableIndex[samplerIndex];
    }
    ID3D12RootSignature* rs = nullptr;
private:
    int SRVNum=0;
    int CBVNum=0;
    int UAVNum=0;
    int SamplerNum=0;
    EST::vector<int>SRVDescriptorTableIndex;
    EST::vector<int>CBVDescriptorTableIndex;
    EST::vector<int>UAVDescriptorTableIndex;
    EST::vector<int>SamplerDescriptorTableIndex;
};
class PSOItem {
public:
    PSOItem() = default;
    PSOItem(D3D12_GRAPHICS_PIPELINE_STATE_DESC* PSOdesc, std::wstring ShaderFileName, ID3D12Device4* device,D3D_SHADER_MACRO*defines, std::string PSName) {
        vsshader = d3dUtil::CompileShader(ShaderFileName, defines, "VS", "vs_5_1");
        psshader = d3dUtil::CompileShader(ShaderFileName, defines, PSName, "ps_5_1");
        PSOdesc->VS = { reinterpret_cast<BYTE*>(vsshader->GetBufferPointer()),vsshader->GetBufferSize() };
        PSOdesc->PS = { reinterpret_cast<BYTE*>(psshader->GetBufferPointer()),psshader->GetBufferSize() };
        ThrowIfFailed(device->CreateGraphicsPipelineState(PSOdesc, IID_PPV_ARGS(&PSO)));
    }
    ID3D12PipelineState* PSO;
private:
    ComPtr<ID3DBlob>vsshader = nullptr;
    ComPtr<ID3DBlob>psshader = nullptr;
};
class computePSOItem {
public:
    computePSOItem() = default;
    computePSOItem(D3D12_COMPUTE_PIPELINE_STATE_DESC* computePSOdesc, std::wstring ShaderFileName, ID3D12Device4* device,const D3D_SHADER_MACRO* defines,std::string CSName) {
        csshader = d3dUtil::CompileShader(ShaderFileName, defines, CSName, "cs_5_1");
        computePSOdesc->CS = { reinterpret_cast<BYTE*>(csshader->GetBufferPointer()),csshader->GetBufferSize() };
        ThrowIfFailed(device->CreateComputePipelineState(computePSOdesc, IID_PPV_ARGS(&PSO)));
    }
    ID3D12PipelineState* PSO;
private:
    ComPtr<ID3DBlob>csshader = nullptr;
};
void drawRenderItem(RenderItem* ri, ID3D12GraphicsCommandList* cmdlist, int objcPara) {
    CD3DX12_GPU_DESCRIPTOR_HANDLE objcHandle = ri->objconstantRI->getCBVGPU();
    cmdlist->SetGraphicsRootDescriptorTable(objcPara, objcHandle);
    cmdlist->IASetPrimitiveTopology(ri->Primitive);
        cmdlist->IASetVertexBuffers(0, 1, ri->Geo->getVBV());
        cmdlist->IASetIndexBuffer(ri->Geo->getIBV());
        cmdlist->DrawIndexedInstanced(ri->indexNum, ri->InstanceNum, ri->startIndex, ri->baseVertex, ri->startInstance);
}

void drawRenderItems(std::unordered_map<std::string,std::unique_ptr< RenderItem>>*RIs,ID3D12GraphicsCommandList* cmdlist, int objcPara,RenderItem*out) {
    for (std::unordered_map<std::string, std::unique_ptr< RenderItem>>::iterator it = RIs->begin();it != RIs->end();it++) {
        if (it->second.get() == out)
            continue;
        drawRenderItem(it->second.get(), cmdlist, objcPara);
    }
}
void BuildBoxAndTriangleSBRI(SAHtree* tree, std::unique_ptr<StructureBufferResourceItem<AABBbox>>& boxSBRI,std::unique_ptr<StructureBufferResourceItem<triangle>>& triangleSBRI, ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, ID3D12DescriptorHeap* srvheap, uploadBuddySystem* upBS, defaultBuddySystem* defBS) {
     boxSBRI = std::make_unique<StructureBufferResourceItem<AABBbox>>(device, cmdlist, &tree->SortedBoxes, true, srvheap, upBS, defBS, tree->SortedBoxes.size());
     triangleSBRI = std::make_unique<StructureBufferResourceItem<triangle>>(device, cmdlist, &tree->Triangles, true, srvheap, upBS, defBS, tree->Triangles.size());
}
void GenerateRandomNum(EST::vector<float>& randnums, int num) {
    for (int i = 0;i < num;i++)
        randnums.push_back(rand() % 1001 / (float)1001);
}
void GenerateOffsets(EST::vector<XMINT2>& offsets) {
    for (int i = 0;i < 5;i++) {
        for (int j = -2;j < 3;j++) {
            for (int k = -2;k < 3;k++) {
                XMINT2 int2 = XMINT2{ j * (int)pow(2,i),k * (int)pow(2,i) };
                offsets.push_back(int2);
            }
        }
    }
}
void ResourceBarrierTrans(ID3D12Resource* r, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, ID3D12GraphicsCommandList*cmdlist) {
    cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition
    (r, before, after));
}
void AddTrianglesToScene(EST::vector<triangle>& SceneTris, EST::vector<Vertex>& vertices, EST::vector<std::uint16_t>& indices,int texIndex,XMMATRIX world) {
    for (int i = 0;i < indices.size() / 3;i++) {
        triangle tri;
        tri.texIndex = texIndex;
        Vertex v1 = vertices[indices[i * 3]];
        Vertex v2 = vertices[indices[i * 3 + 1]];
        Vertex v3 = vertices[indices[i * 3 + 2]];
        XMStoreFloat3( &tri.pos1,(XMVector3TransformCoord(XMLoadFloat4(&XMFLOAT4{ v1.position.x,v1.position.y,v1.position.z,1.0f }), world)));
        XMStoreFloat3( &tri.pos2, (XMVector3TransformCoord(XMLoadFloat4(&XMFLOAT4{ v2.position.x,v2.position.y,v2.position.z,1.0f}), world)));
        XMStoreFloat3( &tri.pos3, (XMVector3TransformCoord(XMLoadFloat4(&XMFLOAT4{ v3.position.x,v3.position.y,v3.position.z,1.0f }), world)));
        XMVECTOR trin =XMVector3Normalize( XMVector3Cross(XMLoadFloat4(&v1.position) - XMLoadFloat4(&v2.position), XMLoadFloat4(&v3.position) - XMLoadFloat4(&v2.position)));
        trin = (XMVectorGetX(XMVector3Dot(XMLoadFloat3(&v1.normal), trin)) > 0) ? trin : -trin;
        XMStoreFloat3( &tri.n, XMVector4Normalize( XMVector3TransformNormal( trin, world)));
        tri.uv12 = XMFLOAT4{ v1.uv.x,v1.uv.y, v2.uv.x,v2.uv.y };
        tri.uv3 = XMFLOAT2{ v3.uv.x,v3.uv.y };
        tri.color = v2.color;
        SceneTris.push_back(tri);
    }
}
void updateLight(GeometryItem* LightGeo, PolygonalLight &l) {
    Vertex v[] = {
{XMFLOAT4{l.Xstart ,199.5,l.Zstart,1.0f},XMFLOAT2{0,0},XMFLOAT3{0,-1.0f,0},XMFLOAT3{1.0f,0,0},0,XMFLOAT3{1.0,1.0,1.0}},
{XMFLOAT4{l.Xend,199.5,l.Zstart,1.0f},XMFLOAT2{1.0f,.0f},XMFLOAT3{0,-1.0f,0},XMFLOAT3{1.0f,0,0},0,XMFLOAT3{1.0,1.0,1.0}},
{XMFLOAT4{l.Xstart,199.5,l.Zend,1.0f},XMFLOAT2{0,1.0f},XMFLOAT3{0,-1.0f,0},XMFLOAT3{1.0f,0,0},0,XMFLOAT3{1.0,1.0,1.0}},
{XMFLOAT4{l.Xend,199.5,l.Zend,1.0f},XMFLOAT2{1.0f,1.0f},XMFLOAT3{0,-1.0f,0},XMFLOAT3{1.0f,0,0},0,XMFLOAT3{1.0,1.0,1.0}}
    };
    EST::vector< Vertex> Lightv(v,4);

    LightGeo->updateVB(&Lightv);
}
void CreateBox(float width, float height, float depth,EST::vector<Vertex>&vertices,EST::vector<std::uint16_t>&indices,XMFLOAT3 color)
{
    float w2 = 0.5f * width;
    float h2 = 0.5f * height;
    float d2 = 0.5f * depth;
    Vertex v[24] = {
        // Fill in the front face vertex data.
        {XMFLOAT4{ -w2, -h2, -d2,1.0f},XMFLOAT2{ 0.0f, 1.0f},XMFLOAT3{ 0.0f, 0.0f, -1.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ -w2, +h2, -d2,1.0f},XMFLOAT2{ 0.0f, 0.0f},XMFLOAT3{  0.0f, 0.0f, -1.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, +h2, -d2,1.0f},XMFLOAT2{ 1.0f, 0.0f},XMFLOAT3{  0.0f, 0.0f, -1.0f},XMFLOAT3{1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, -h2, -d2,1.0f},XMFLOAT2{ 1.0f, 1.0f},XMFLOAT3{  0.0f, 0.0f, -1.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        // Fill in the back face vertex data.
        {XMFLOAT4{ -w2, -h2, +d2,1.0f},XMFLOAT2{ 1.0f, 1.0f},XMFLOAT3{ 0.0f, 0.0f, 1.0f},XMFLOAT3{ -1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, -h2, +d2,1.0f},XMFLOAT2{ 0.0f, 1.0f},XMFLOAT3{  0.0f, 0.0f, 1.0f},XMFLOAT3{ -1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, +h2, +d2,1.0f},XMFLOAT2{ 0.0f, 0.0f},XMFLOAT3{  0.0f, 0.0f, 1.0f},XMFLOAT3{-1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ -w2, +h2, +d2,1.0f},XMFLOAT2{ 1.0f, 0.0f},XMFLOAT3{  0.0f, 0.0f, 1.0f},XMFLOAT3{ -1.0f, 0.0f, 0.0f},0,color},
        // Fill in the top face vertex data.
        {XMFLOAT4{ -w2, +h2, -d2,1.0f},XMFLOAT2{ 0.0f, 1.0f},XMFLOAT3{ 0.0f, 1.0f, 0.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ -w2, +h2, +d2,1.0f},XMFLOAT2{ 0.0f, 0.0f},XMFLOAT3{  0.0f, 1.0f, 0.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, +h2, +d2,1.0f},XMFLOAT2{ 1.0f, 0.0f},XMFLOAT3{  0.0f, 1.0f, 0.0f},XMFLOAT3{1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, +h2, -d2,1.0f},XMFLOAT2{ 1.0f, 1.0f},XMFLOAT3{  0.0f, 1.0f, 0.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        // Fill in the bottom face vertex data.
        {XMFLOAT4{ -w2, -h2, -d2,1.0f},XMFLOAT2{ 1.0f, 1.0f},XMFLOAT3{ 0.0f, -1.0f, 0.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, -h2, -d2,1.0f},XMFLOAT2{ 0.0f, 1.0f},XMFLOAT3{  0.0f, -1.0f, 0.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, -h2, +d2,1.0f},XMFLOAT2{ 0.0f, 0.0f},XMFLOAT3{  0.0f, -1.0f, 0.0f},XMFLOAT3{1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ -w2, -h2, +d2,1.0f},XMFLOAT2{ 1.0f, 0.0f},XMFLOAT3{  0.0f, -1.0f, 0.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        // Fill in the left face vertex data.
        {XMFLOAT4{ -w2, -h2, +d2,1.0f},XMFLOAT2{ 0.0f, 1.0f},XMFLOAT3{ -1.0f, 0.0f, 0.0f},XMFLOAT3{ 0.0f, 0.0f,-1.0f},0,color},
        {XMFLOAT4{ -w2, +h2, +d2,1.0f},XMFLOAT2{ 0.0f, 0.0f},XMFLOAT3{  -1.0f, 0.0f, 0.0f},XMFLOAT3{ 0.0f, 0.0f, -1.0f},0,color},
        {XMFLOAT4{ -w2, +h2, -d2,1.0f},XMFLOAT2{ 1.0f, 0.0f},XMFLOAT3{  -1.0f, 0.0f, 0.0f},XMFLOAT3{0.0f, 0.0f, -1.0f},0,color},
        {XMFLOAT4{ -w2, -h2, -d2,1.0f},XMFLOAT2{ 1.0f, 1.0f},XMFLOAT3{  -1.0f, 0.0f, 0.0f},XMFLOAT3{ 0.0f, 0.0f, -1.0f},0,color},

        {XMFLOAT4{ +w2, -h2, -d2,1.0f},XMFLOAT2{ 0.0f, 1.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},XMFLOAT3{ 0.0f, 0.0f,1.0f},0,color},
        {XMFLOAT4{ +w2, +h2, -d2,1.0f},XMFLOAT2{ 0.0f, 0.0f},XMFLOAT3{  1.0f, 0.0f, 0.0f},XMFLOAT3{ 0.0f, 0.0f, 1.0f},0,color},
        {XMFLOAT4{ +w2, +h2, +d2,1.0f},XMFLOAT2{ 1.0f, 0.0f},XMFLOAT3{  1.0f, 0.0f, 0.0f},XMFLOAT3{0.0f, 0.0f, 1.0f},0,color},
        {XMFLOAT4{ +w2, -h2, +d2,1.0f},XMFLOAT2{ 1.0f, 1.0f},XMFLOAT3{  1.0f, 0.0f, 0.0f},XMFLOAT3{ 0.0f, 0.0f, 1.0f},0,color}
        // Fill in the right face vertex data.
    };
    vertices.assign(v, 24);
    std::uint16_t i[36];
    // Fill in the front face index data
    i[0] = 0; i[1] = 1; i[2] = 2;
    i[3] = 0; i[4] = 2; i[5] = 3;
    // Fill in the back face index data
    i[6] = 4; i[7] = 5; i[8] = 6;
    i[9] = 4; i[10] = 6; i[11] = 7;
    // Fill in the top face index data
    i[12] = 8; i[13] = 9; i[14] = 10;
    i[15] = 8; i[16] = 10; i[17] = 11;
    // Fill in the bottom face index data
    i[18] = 12; i[19] = 13; i[20] = 14;
    i[21] = 12; i[22] = 14; i[23] = 15;
    // Fill in the left face index data
    i[24] = 16; i[25] = 17; i[26] = 18;
    i[27] = 16; i[28] = 18; i[29] = 19;
    // Fill in the right face index data
    i[30] = 20; i[31] = 21; i[32] = 22;
    i[33] = 20; i[34] = 22; i[35] = 23;
    indices.assign(i, 36);
}
meshdata subvide(meshdata& inmesh, int vsize, int isize) {
    meshdata mesh;
    for (int meshindex = 1;meshindex <= (isize / 3);meshindex++) {
        int a = inmesh.indices[(meshindex - 1) * 3];//每个面源的点索引
        int b = inmesh.indices[(meshindex - 1) * 3 + 1];
        int c = inmesh.indices[(meshindex - 1) * 3 + 2];
        XMFLOAT3 v1 = XMFLOAT3{ (inmesh.vertices[a].x + inmesh.vertices[b].x) / 2,(inmesh.vertices[a].y + inmesh.vertices[b].y) / 2,(inmesh.vertices[a].z + inmesh.vertices[b].z) / 2 };
        XMFLOAT3 v2 = XMFLOAT3{ (inmesh.vertices[b].x + inmesh.vertices[c].x) / 2,(inmesh.vertices[b].y + inmesh.vertices[c].y) / 2,(inmesh.vertices[b].z + inmesh.vertices[c].z) / 2 };
        XMFLOAT3 v3 = XMFLOAT3{ (inmesh.vertices[a].x + inmesh.vertices[c].x) / 2,(inmesh.vertices[a].y + inmesh.vertices[c].y) / 2,(inmesh.vertices[a].z + inmesh.vertices[c].z) / 2 };
        mesh.vertices.push_back(inmesh.vertices[a]);//0
        mesh.vertices.push_back(inmesh.vertices[b]);//1
        mesh.vertices.push_back(inmesh.vertices[c]);//2
        mesh.vertices.push_back(v1);//3
        mesh.vertices.push_back(v2);//4
        mesh.vertices.push_back(v3);//5
        int k = (meshindex - 1) * 6;
        mesh.indices.push_back(3 + k);
        mesh.indices.push_back(1 + k);
        mesh.indices.push_back(4 + k);
        mesh.indices.push_back(0 + k);
        mesh.indices.push_back(3 + k);
        mesh.indices.push_back(5 + k);
        mesh.indices.push_back(3 + k);
        mesh.indices.push_back(4 + k);
        mesh.indices.push_back(5 + k);
        mesh.indices.push_back(5 + k);
        mesh.indices.push_back(4 + k);
        mesh.indices.push_back(2 + k);
    }
    return mesh;
}
void  createSphere(EST::vector<Vertex>& v, EST::vector<std::uint16_t>& ind,float r,XMFLOAT3 color) {
    const float X = 0.5257f;
    const float Z = 0.8506f;
    XMFLOAT3 sphereVertices[] = {
      XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
    XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
    XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
    XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
    XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
    XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
    };
    std::uint16_t sphereIndices[] = {
    1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
    1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
    3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
    10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
    };
    meshdata mesh12;
    mesh12.indices.addData(sphereIndices, 60);
    mesh12.vertices.addData(sphereVertices, 12);

    meshdata m1=subvide(mesh12, mesh12.vertices.size(), mesh12.indices.size());
    meshdata m2=subvide(m1, m1.vertices.size(), m1.indices.size());
    meshdata m3=subvide(m2, m2.vertices.size(), m2.indices.size());

    for (int i = 0;i < m3.vertices.size();i++) {
        XMVECTOR vec = { m3.vertices[i].x, m3.vertices[i].y, m3.vertices[i].z };
        vec = XMVector3Normalize(vec);
        float theta = std::atanf(XMVectorGetZ(vec) / XMVectorGetX(vec));
        if (theta < 0)
            theta += XM_2PI;
        float thetak = theta / XM_2PI;
        float phi = std::acosf(XMVectorGetY(vec) / 1.0f);
        float phik = phi / XM_PI;
        Vertex ver;
        ver.position = XMFLOAT4{ XMVectorGetX(vec) * r ,XMVectorGetY(vec) * r,XMVectorGetZ(vec) * r,1.0f };
        ver.uv = { thetak ,phik };
        ver.TangentU = XMFLOAT3{ r * sin(phi) * sin(theta),0,r * sin(phi) * cos(theta) };
        ver.color = color;
        XMStoreFloat3(&ver.normal, vec);
        v.push_back(ver);
    }
    for (int i = 0;i < m3.indices.size();i++) {
        ind.push_back(m3.indices[i]);
    }
}
EST::vector<float>CalcGaussWeights(float variance,float scale)
{
    int MaxBlurRadius = 100;
    float sigma = std::sqrt(variance);
    float twoSigma2 = 2.0f * sigma * sigma;
    int blurRadius = (int)ceil(2.0f * sigma*scale);
    blurRadius = min(90, blurRadius);
   // assert(blurRadius <= MaxBlurRadius);

    EST::vector<float> weights;
    weights.resize(2 * blurRadius + 2);
    weights[0] = float(2 * blurRadius + 1);
    float weightSum = 0.0f;

    for (int i = -blurRadius; i <= blurRadius; ++i)
    {
        float x = (float(i)/scale);

        weights[i + blurRadius+1] = expf(-x * x / twoSigma2);

        weightSum += weights[i + blurRadius+1];
    }

    // Divide by the sum so all the weights add up to 1.0.
    for (int i = 0; i < weights.size()-1; ++i)
    {
        weights[i+1] /= weightSum;
    }

    return weights;
}
class APP {
public:
	APP() = default;
    bool initDX12(HINSTANCE hInstance, WNDPROC CALLBACK    WndProc, int       nCmdShow) {
        createWindow(hInstance, WndProc, nCmdShow);
        openDebug();
        createFactoryAndDevice();
        return true;
    }
    int iWidth = 1024;
    int iHeight = 768;
    UINT nDXGIFactoryFlags = 0U;
    HWND hWnd = nullptr;
    UINT ThreadNum = 0;
    lastVPmat lastvpmat;
    ComPtr<IDXGIFactory5>                pIDXGIFactory5;
    ComPtr<IDXGIAdapter1>                pIAdapter;
    ComPtr<ID3D12Device4>                pID3DDevice;
private:

    bool createWindow(HINSTANCE hInstance, WNDPROC CALLBACK    WndProc, int       nCmdShow) {
        AllocConsole();
        g_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        {
            WNDCLASSEX wcex = {};
            wcex.cbSize = sizeof(WNDCLASSEX);
            wcex.style = CS_GLOBALCLASS;
            wcex.lpfnWndProc = WndProc;
            wcex.cbClsExtra = 0;
            wcex.cbWndExtra = 0;
            wcex.hInstance = hInstance;
            wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);		//防止无聊的背景重绘
            wcex.lpszClassName = GRS_WND_CLASS_NAME;
            RegisterClassEx(&wcex);

            DWORD dwWndStyle = WS_OVERLAPPED | WS_SYSMENU;
            RECT rtWnd = { 0, 0, iWidth, iHeight };
            AdjustWindowRect(&rtWnd, dwWndStyle, FALSE);

            // 计算窗口居中的屏幕坐标
            INT posX = (GetSystemMetrics(SM_CXSCREEN) - rtWnd.right - rtWnd.left) / 2;
            INT posY = (GetSystemMetrics(SM_CYSCREEN) - rtWnd.bottom - rtWnd.top) / 2;

            hWnd = CreateWindowW(GRS_WND_CLASS_NAME
                , GRS_WND_TITLE
                , dwWndStyle
                , posX
                , posY
                , rtWnd.right - rtWnd.left
                , rtWnd.bottom - rtWnd.top
                , nullptr
                , nullptr
                , hInstance
                , nullptr);

            if (!hWnd)
            {
                return FALSE;
            }

            ShowWindow(hWnd, nCmdShow);
            UpdateWindow(hWnd);
        }


    }
    void openDebug() {
        // 打开显示子系统的调试支持
#if defined(_DEBUG)
        ComPtr<ID3D12Debug> debugController;
        ComPtr<ID3D12Debug1>debugController1;
        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        debugController->EnableDebugLayer();
        debugController.As(&debugController1);
        debugController1->SetEnableGPUBasedValidation(true);
        // 打开附加的调试支持
        nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

#endif 
    }
    void createFactoryAndDevice() {
        //创建factory
        {
            CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&pIDXGIFactory5));
            ThrowIfFailed(pIDXGIFactory5->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
        }
        //创建device
        {
            DXGI_ADAPTER_DESC1 desc = {};
            for (UINT index = 0;DXGI_ERROR_NOT_FOUND != pIDXGIFactory5->EnumAdapters1(index, &pIAdapter);index++) {

                pIAdapter->GetDesc1(&desc);
                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                    continue;
                }
                if (SUCCEEDED(D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
                {

                    break;
                }
            }
            ThrowIfFailed(D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&pID3DDevice)));
            TCHAR pszWndTitle[MAX_PATH] = {};
            ThrowIfFailed(pIAdapter->GetDesc1(&desc));
            ::GetWindowText(hWnd, pszWndTitle, MAX_PATH);
            ::SetWindowText(hWnd, pszWndTitle);
        }
    }

};
