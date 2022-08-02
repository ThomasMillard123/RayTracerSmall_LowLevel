#pragma once


#include <iostream>
#include <iomanip>
#include <utility>
#include <vector>
#include <cstddef>
#include <cstdlib>
#include<mutex>

using namespace std;

template<size_t chunckSize, size_t NumberOfChuckPerBlock>
class PoolAllocator {
public:

	PoolAllocator() {}

	//free memory
	~PoolAllocator() {
		for (auto block : blockList) {
			free(block);
		}
	}


	//allocate data
	void* Allocate() {
		//if there is no block create one
		if (NextChunk == nullptr)
			NextChunk = allocate_block(nullptr);

		//allocate free block
		Chunk* chunk = NextChunk;
		//set nextChunck to a free chunck
		NextChunk = NextChunk->next;
		chunk->inUse = true;
		printf("Allocate chunk #%i  Location: %p \n", chunk->m_seq, chunk);
		return chunk->data;
	}
	//remove from alocation 
	void Deallocate(void* ptr) {

		Chunk* cuurChunck = startChunck;
		//find chunk to free
		for (int i = 0; i < NumberOfChuckPerBlock; i++) {
			if (cuurChunck->data == ptr) {
				cuurChunck->inUse = false;
				//let it be known chunk if free
				cuurChunck->next = NextChunk;
				NextChunk = cuurChunck;
				printf("Deallocate chunk #%i  Location: %p \n", cuurChunck->m_seq, cuurChunck);
				break;
			}

			cuurChunck = cuurChunck + 1;
		}

	}

	void reserve_blocks(std::size_t blocks)
	{
		while (blocks > blockList.size())
			NextChunk = allocate_block(NextChunk);
	}
	//output all data from pools
	void GetAllData() {

		for (auto block : blockList) {
			unsigned int MemInuse = 0;
			Chunk* cuurChunck = startChunck;
			for (int i = 0; i < NumberOfChuckPerBlock; i++) {

				printf("\n Chunk #%i \n Location: %p  \n Data: %p \n Data Size: %i \n Next: %p \n Size: %i\n In use: %s \n", cuurChunck->m_seq, cuurChunck, cuurChunck->data, sizeof(cuurChunck->data), cuurChunck->next, cuurChunck->Size, cuurChunck->inUse ? "True": "False");

				if (cuurChunck->inUse) {
					MemInuse += cuurChunck->Size;
				}
				//move to next chunk
				cuurChunck = cuurChunck + 1;

			}
			printf("Block Size: %i \n", sizeof(Chunk) * NumberOfChuckPerBlock);
			printf("Size in use %i \n \n", MemInuse);

		}


	}

	
private:
	size_t k_seq = 1;



	struct Chunk {

		Chunk(Chunk* next = nullptr, size_t size = 0, size_t No = 0, bool inuse = false) : next(next), Size(size), inUse(inuse), m_seq(No) {
			printf("Chunk no: %u \n" , (unsigned int)m_seq);
		}

		Chunk* next;
		unsigned char data[chunckSize];
		size_t Size;
		bool inUse;



		size_t m_seq;
		
	};

	Chunk* NextChunk = nullptr;
	Chunk* startChunck = nullptr;

	//create Block
	Chunk* allocate_block(Chunk* tail) {
		//error
		//unique_lock guard(m_lock);
		if (NextChunk != nullptr)
			return NextChunk;

		//create memory
		Chunk* block = reinterpret_cast<Chunk*>(std::malloc(sizeof(Chunk) * NumberOfChuckPerBlock));
		if (NextChunk != nullptr) {
			printf("Error %s \n", "no block");
			return NextChunk;
		}
		blockList.push_back(block);

		printf("Creat Block Size %i \n", sizeof(sizeof(Chunk) * NumberOfChuckPerBlock));


		Chunk* chunk = block;
		startChunck = chunk;

		//create chunks
		for (std::size_t i = 0; i < NumberOfChuckPerBlock - 1; ++i)
		{
			
			new (chunk) Chunk{ chunk + 1, sizeof(Chunk),k_seq };

			chunk = chunk->next;
			k_seq++;
		}
		//end Chunck
		new (chunk) Chunk{ tail,sizeof(Chunk),k_seq };
		k_seq++;
		return block;
	}

	vector<void*> blockList;
	std::mutex m_lock;
};
