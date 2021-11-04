#include "New_Overrides.h"
#include<iostream>

void* operator new (size_t size) {

	Heap* h = Memory_Management::GetDefaultHeap();

	void* pStartMemBlock = ::operator new (size, h);
	return pStartMemBlock;
	
}

void* operator new (size_t size, Heap* heap) {

	size_t nRequestedBytes = size + sizeof(Header) + sizeof(Footer);
	char* pMem = (char*)malloc(nRequestedBytes);

	Header* pHeader = (Header*)pMem;
	pHeader->size = size;
	pHeader->checkvalue = 0xDEADC0DE;
	pHeader->pHeap = heap;
	if (heap != nullptr) {
		pHeader->pHeap->AddBytesAllocated(pHeader->size);
	}


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
			pHeader->pNext = nullptr;
		}
	}


	void* pFooterAddr = pMem + sizeof(Header) + size;
	Footer* pFooter = (Footer*)pFooterAddr;


	void* pStartMemBlock = pMem + sizeof(Header);



	std::cout << "\n checkvalue: " << pHeader->checkvalue << "\n Size: " << pHeader->size << "\n Heap: " << pHeader->pHeap;

	return pStartMemBlock;
}
void operator delete (void* pMem) {

	
		Header* pHeader = (Header*)((char*)pMem - sizeof(Header));

		if (pHeader->pHeap != nullptr)
		{
			pHeader->pHeap->RemoveBytesAllocated(pHeader->size);

			Footer* pFooter = (Footer*)((char*)pMem + pHeader->size);


		}

		std::cout << "\n checkvalue: " << pHeader->checkvalue << "\n Size: " << pHeader->size << "\n";

		free(pHeader);
	
}