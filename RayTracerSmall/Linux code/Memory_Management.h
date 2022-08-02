#pragma once

#include <vector>
#include<string>
#include<iostream>

using namespace std;



class Heap;
/// <summary>
/// this is a factory for creating and tracking heaps
/// </summary>
typedef vector<Heap*> vecHeap;
class Memory_Management
{
public:

	static vecHeap _Heap;
	static void AddHeap(string Name);
	static Heap* GetHeap(string Name);
	static void RemoveAllHeap();

	static Heap* GetDefaultHeap();
	static void AddDefaultHeap(Heap* pHeap);
private:
	static Heap* DefaultHeap;
};


struct Header;
/// <summary>
/// heap class is for tracking memory it can call all memeory alocated to it.
/// </summary>
class Heap
{
public:
	Heap();
	Heap(string name);
	~Heap();

	void SetUp();
	void AddBytesAllocated(int numberOfBytes);
	void RemoveBytesAllocated(int numberOfBytes);
	string GetName() { return Name; }
	void GetAllData();

	Header* start = nullptr;

	Header* Current = nullptr;
	Header* prev = nullptr;



private:

	int totalNumberOfbytes = 0;
	string Name = "Default";
};

struct Header
{
	int size;
	int checkvalue = 0xDEADC0DE;
	Heap* pHeap;
	Header* pNext = nullptr;
	Header* pPrev = nullptr;
};

struct Footer
{
	int reserved;
	int checkvalue = 0xDEADC00DE;
};


struct Chunk
{
	size_t size;
	void* mem;
	bool isAllocated;

	Chunk* nextChunck;
	Chunk* PrevChunck;
};



