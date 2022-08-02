#pragma once

#include"Memory_Management.h"
//overide new and delete
void* operator new (size_t size);
void* operator new (size_t size, Heap* heap);
void operator delete (void* pMem,int a);