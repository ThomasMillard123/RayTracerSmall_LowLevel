#include "Memory_Management.h"

vecHeap Memory_Management::_Heap;
Heap* Memory_Management::DefaultHeap = nullptr;
void Memory_Management::AddHeap(string Name)
{
	Memory_Management::_Heap.push_back(new Heap(Name));
}

Heap* Memory_Management::GetHeap(string Name)
{

	for (const auto& HeapData : _Heap) {
		if (HeapData->GetName() == Name) {
			return HeapData;
		}
	}

	return nullptr;
}

void Memory_Management::RemoveAllHeap()
{
	for (auto& HeapData : _Heap) {
		delete HeapData;
	}

    _Heap.clear();
}

//this stops the loop when a defual heap is created
static bool Load = true;
Heap* Memory_Management::GetDefaultHeap()
{
	//checks if there is a default heap if not creat one
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
//adds data to the heap
void Heap::AddBytesAllocated(int numberOfBytes)
{

	totalNumberOfbytes += numberOfBytes;

	cout << "\n\n Name: " << Name << "\n";
	cout << "Number of bytes added: " << numberOfBytes << "\n";
	cout << "Total " << totalNumberOfbytes << "\n";

}
//removes data from the heap
void Heap::RemoveBytesAllocated(int numberOfBytes)
{
	totalNumberOfbytes -= numberOfBytes;

	cout << "\n Name: " << Name << "\n";
	cout << "Number of bytes removed: " << numberOfBytes << "\n";
	cout << "Total " << totalNumberOfbytes << "\n";

	if (totalNumberOfbytes == 0) {
		start = nullptr;
	}

}
//outputs all data within the heap in a readable format
void Heap::GetAllData()
{
	int count = 0;
	string errors;
	cout << "\n Heap name: " << Name << endl;

	printf("|%20s|%20s|%20s|%20s|%20s|%20s| \n","Current Hedder", "checkvalue", "size","pointer Heap","pointer Prev header","pointer Next headder");
	Header* curr = start;
	while (curr != nullptr)
	{
		

		printf("|%20p|%20X|%20i|%20p|%20p|%20p| \n",curr, curr->checkvalue, curr->size,Current->pHeap,curr->pPrev,curr->pNext);
		count++;



		if (curr->checkvalue != 0xDEADC0DE) {
			errors += "Over written memomory \n";
		}


		curr = curr->pNext;



	}

	printf("|%20s %83i| \n", "Total objects", count);
	printf("|%20s %83i| \n", "Total bytes", totalNumberOfbytes);
	printf("%20s \n %s \n", "Errors:", errors.c_str());

}




