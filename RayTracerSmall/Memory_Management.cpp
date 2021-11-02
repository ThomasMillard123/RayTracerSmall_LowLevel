#include "Memory_Management.h"

vecHeap Memory_Management::_Heap;
Heap* Memory_Management::DefaultHeap = nullptr;
void Memory_Management::AddHeap(string Name)
{
	Memory_Management::_Heap.push_back(new Heap(Name));
}

Heap* Memory_Management::GetHeap(string Name)
{

	for (Heap* HeapData : _Heap) {
		if (HeapData->GetName() == Name) {
			return HeapData;
		}
	}

	return nullptr;
}

void Memory_Management::RemoveAllHeap()
{
    _Heap.clear();
}

static bool Load = true;
Heap* Memory_Management::GetDefaultHeap()
{
	if (DefaultHeap == nullptr && Load) {
		Load = false;
		DefaultHeap = new Heap();

	}

	return DefaultHeap;
}

void Memory_Management::AddDefaultHeap(Heap* pHeap)
{
	DefaultHeap = pHeap;
}


Heap::Heap()
{
	SetUp();
}
Heap::Heap(string name) : Name(name)
{
	totalNumberOfbytes = 0;
}

Heap::~Heap()
{
}

void Heap::SetUp()
{
	Name = "Default";
	totalNumberOfbytes = 0;
}

void Heap::AddBytesAllocated(int numberOfBytes)
{

	totalNumberOfbytes += numberOfBytes;

	cout << "\n\n Name: " << Name << "\n";
	cout << "Number of bytes added: " << numberOfBytes << "\n";
	cout << "Total " << totalNumberOfbytes << "\n";

}

void Heap::RemoveBytesAllocated(int numberOfBytes)
{
	totalNumberOfbytes -= numberOfBytes;

	cout << "\n Name: " << Name << "\n";
	cout << "Number of bytes removed: " << numberOfBytes << "\n";
	cout << "Total " << totalNumberOfbytes << "\n";
}

void Heap::GetAllData()
{
	int count = 0;
	cout << "\n Heap name: " << Name << endl;

	printf("|%10s|%5s| \n", "checkvalue", "size");
	Header* curr = start;
	while (curr != nullptr)
	{


		printf("|%10X|%5i| \n", curr->checkvalue, curr->size);
		count++;


		curr = curr->pNext;



	}

	printf("|%*s %i| \n", 15, "Total objects", count);
}


