#include"EST.h"
#include<assert.h>
using namespace EST;
pageTable::pageTable() {
    init();
}
void pageTable::init() {
    data = new pageTableItem[pageTableItemSize];
}
int pageTable::addPageTableItems(void** pagesStart, int pageNum) {
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
void pageTable::free(int pageNote) {
    int nextNote = pageNote;
    while (nextNote > 0) {
        data[nextNote].PageStart = nullptr;
        int t = data[nextNote].nextPageNote;
        data[nextNote].nextPageNote = -1;
        nextNote = t;
    }
}
void pageTable::getPages(int startPageNote, void*** pageStart, int pageNum) {
    int nextNote = startPageNote;
    *pageStart = new void* [pageNum];
    int it = 0;
    while (nextNote >= 0) {
        (*pageStart)[it] = data[nextNote].PageStart;
        nextNote = data[nextNote].nextPageNote;
        it++;
    }
}
void* pageTable::getPageStart(int pageNote, int pageIndex) {
    int retNote = pageNote;
    for (int i = 0;i < pageIndex;i++)
    {
        retNote = data[retNote].nextPageNote;
    }
    return data[retNote].PageStart;
}
pageTable* pagetable = new pageTable();
allocator::allocator() = default;
allocator::allocator(UINT pagesize, UINT dividecount) {
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
void* allocator::allocate() {
    if (freeNum > 0) {
        Block* b = freeList;
        freeList = b->next;
        freeNum--;
        return (void*)b;
    }
    return nullptr;
}
void allocator::free(void* freeBlock) {//递归释放页表里连续的页面或者，如果divideNum为0，则此页面不由链表管理，也就不必增加链表节点，只需进行pageTable的Free即可
    if (divideCount == 0)
        pagetable->free(pageNote);
    else {
        Block* b = reinterpret_cast<Block*>(freeBlock);
        b->next = freeList;
        freeList = b;
        freeNum++;
    }
}
void allocator::freeAll() {

}
void* allocator::getPTR() {//获得大块内存的首地址
    return ptr;

}

MemoryManager::MemoryManager() = default;
MemoryManager::MemoryManager(UINT pageSizeInKB, UINT maxdividenum) {
    PageSize = pageSizeInKB * 1024;
    MaxDivideNum = maxdividenum;
    PageList = nullptr;
    //for (int i = 0;i < maxdividenum;i++)
    //    allocPage(i);
}
Page* MemoryManager::allocPage(int PageDivideNum) {
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
void* MemoryManager::alloc(UINT MSize, allocator** alloc) {
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
        (*alloc)->pageNote = pagetable->addPageTableItems(pagesStart, allocPageNum);
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
MemoryManager* memorymanager = new MemoryManager(16, 14);
