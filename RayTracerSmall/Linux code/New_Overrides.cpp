#include "New_Overrides.h"
#include<iostream>

void* operator new (size_t size) {

	//check and create defualt heap
	Heap* h = Memory_Management::GetDefaultHeap();

	void* pStartMemBlock = ::operator new (size, h);
	return pStartMemBlock;
	
}

void* operator new (size_t size, Heap* heap) {

	//set up number of bytes
	size_t nRequestedBytes = size + sizeof(Header) + sizeof(Footer);
	char* pMem = (char*)malloc(nRequestedBytes);

	//create headder
	Header* pHeader = (Header*)pMem;
	pHeader->size = size;
	pHeader->checkvalue = 0xDEADC0DE;
	pHeader->pHeap = heap;

	if (heap != nullptr) {
		pHeader->pHeap->AddBytesAllocated(pHeader->size);
	}

	//link headers in heap
	if (heap != nullptr)
	{
		if (heap->start == nullptr) {
			heap->start = pHeader;
			pHeader->pPrev = nullptr;
			heap->Current = pHeader;

		}
		else {
			pHeader->pPrev = heap->Current;
			heap->Current->pNext = pHeader;
			heap->Current = pHeader;
			
		}
		pHeader->pNext = nullptr;
	}
	
	//creat footer
	void* pFooterAddr = pMem + sizeof(Header) + size;
	Footer* pFooter = (Footer*)pFooterAddr;
	pFooter->reserved = size;
	pFooter->checkvalue = 0xDEADC0DE;

	void* pStartMemBlock = pMem + sizeof(Header);


	//output values going in
	//printf("\n New header: \n Value: %p \n checkvalue: %x \n Size: %i \n Heap: %p \n Next: %p\n prev: %p \n", pHeader, pHeader->checkvalue, pHeader->size , pHeader->pHeap, pHeader->pNext, pHeader->pPrev);

	return pStartMemBlock;
}
//overide delete
void operator delete (void* pMem) {

	
		Header* pHeader = (Header*)((char*)pMem - sizeof(Header));
		Footer* pFooter = (Footer*)((char*)pMem + pHeader->size);

		//for objects that have no heap (privetly created objects by external code and  defualt heap)
		if (pHeader->pHeap != nullptr)
		{
			//swap headers for list
			if (pHeader == pHeader->pHeap->start) {
				pHeader->pHeap->start = pHeader->pNext;
			}
			else
			{
				if (pHeader->pNext != nullptr) {
					pHeader->pPrev->pNext = pHeader->pNext;
					pHeader->pNext->pPrev = pHeader->pPrev;
				}
				else
				{
					
					pHeader->pPrev->pNext = nullptr;
					pHeader->pHeap->Current = pHeader->pPrev;
				}
			}
			pHeader->pHeap->RemoveBytesAllocated(pHeader->size);

		}

		//error checks
		if (pHeader->checkvalue != 0xDEADC0DE) {
			printf("%s", "memory error");
		}
		else if (pFooter->checkvalue != 0xDEADC0DE) {
			printf("%s", "memory error");
		}
	
		//say what is being deleted
		//printf("\n Delete header: \n Value: %p \n checkvalue: %x \n Size: %i \n Heap: %p \n Next: %p\n prev: %p \n", pHeader, pHeader->checkvalue, pHeader->size, pHeader->pHeap, pHeader->pNext, pHeader->pPrev);

		free(pHeader);
	
}