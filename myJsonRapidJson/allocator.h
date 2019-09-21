#ifndef MY_JSON_ALLOCATOR_H
#define MY_JSON_ALLOCATOR_H
#include <stdlib.h>		//for malloc
#include <stdint.h>		//for uint32_t uint16_t
#include <string.h>		//for memcpy
#ifndef MYJSON_ALLOCATOR_DRFAULT_CHUNK_CAPACITY
#define MYJSON_ALLOCATOR_DRFAULT_CHUNK_CAPACITY (64 * 1024)
#endif

//对齐字节，这里的处理是如果size小于8，那么按照8字节来处理，保证分配的内存大小是8的整数倍
#define MYJSON_ALIGN(x)(((x)+(size_t)7u) & ~(size_t)7u)		

class DefaultAllocator
{
public:
	static const bool kNeedFree = true;

	void *myMalloc(size_t size)							//::maloc
	{
		if (size <= 0)
			return nullptr;
		else
			return ::malloc(size);
	}

	void *myRealloc(void *oldPtr, size_t newSize)		//::realloc
	{
		if (!newSize)
		{
			::free(oldPtr);
			return nullptr;
		}
		else if (newSize > 0)
		{
			return ::realloc(oldPtr, newSize);
		}
		else
			return nullptr;
	}

	static void myFree(void *ptr) { ::free(ptr); }		//::free
};

/* 内存池分配器：注意myMalloc和myRealloc会导致块存在内存碎片 */
template<typename BaseAlloctor = DefaultAllocator>
class MemoryPoolAlloctor
{
public:
	static const bool kNeedFree = false;			//don't need to call free()

	MemoryPoolAlloctor(size_t chunkSize = MYJSON_ALLOCATOR_DRFAULT_CHUNK_CAPACITY, BaseAlloctor *baseAlloctor = nullptr) :
		_sTotalSize(0), _sUsedSize(0), _pChunkHead(nullptr), _sMinAllocSize(chunkSize), _pBaseAlloctor(baseAlloctor), _pOwnBaseAlloctor(nullptr)
	{

	}

	~MemoryPoolAlloctor()
	{
		clear();
		if (_pOwnBaseAlloctor)
			delete _pOwnBaseAlloctor;
	}

	void clear()
	{
		ChunkHead *ptr = _pChunkHead;
		while (ptr)
		{
			_pChunkHead = ptr->next;
			BaseAlloctor::myFree(ptr);
			ptr = _pChunkHead;
		}
		_sTotalSize = _sUsedSize = 0;
	}

	size_t totalSize()const { return _sTotalSize; }
	size_t usedSize()const { return _sUsedSize; }

	/* Malloc */
	void *myMalloc(size_t size)
	{
		/* 在内存池中去分配 */
		if (size <= 0)
			return nullptr;
		
		size = MYJSON_ALIGN(size);
		
		/* 第一种情况：size不能够在剩余中去分配 */
		if (!_pChunkHead || _pChunkHead->used_size + size > _pChunkHead->total_size)
		{
			/*添加*/
			if (!addChunk(_sMinAllocSize > size ? _sMinAllocSize : size))
				return nullptr;
		}

		//注意这里也需要对齐，我们保证分配的块字节是8整数，还要保证块头结构也要是8的整数，这样给用户也是一个8的整数倍
		void *buffer = reinterpret_cast<char*>(_pChunkHead) + MYJSON_ALIGN(sizeof(ChunkHead)) + _pChunkHead->used_size;		
		_pChunkHead->used_size += size;
		return buffer;
	}

	/* Realloc */
	void *myRealloc(void *oldPtr, size_t oldSize, size_t newSize)
	{
		if (!oldPtr)
			return myMalloc(newSize);

		if (!newSize)
			return nullptr;

		oldSize = MYJSON_ALIGN(oldSize);
		newSize = MYJSON_ALIGN(newSize);
		if (oldSize >= newSize)				//因为我们是一个内存池，采用预分配的策略，减少malloc次数 因此不允许缩小内存
			return oldPtr;

		/* 两种情况：第一种如果要重分配的指针刚好是最后一个chunk管理且增加的大小刚好不超过剩余的大小，那么直接返回old指针；否则需要重新申请一个chunk */
		uint8_t *ptr = reinterpret_cast<uint8_t*>(_pChunkHead) + MYJSON_ALIGN(sizeof(ChunkHead)) + _pChunkHead->used_size - oldSize;
		if (ptr == oldPtr)
		{
			if (_pChunkHead->total_size - _pChunkHead->used_size >= newSize)
				return oldPtr;
		}

		uint8_t *newPtr = myMalloc(newSize);
		if(newPtr)
			memcpy(newPtr, oldPtr, oldSize);
		return newPtr;
	}

private:
	
	//forbid copy and assign
	MemoryPoolAlloctor(const MemoryPoolAlloctor &other) = delete;
	MemoryPoolAlloctor& operator=(const MemoryPoolAlloctor &other) = delete;

	void *addChunk(size_t size)
	{
		if (!_pBaseAlloctor)
			_pBaseAlloctor = _pOwnBaseAlloctor = new BaseAlloctor;		//this _pOwnBaseAlloctor need delete by destructor

		ChunkHead *ptr = reinterpret_cast<ChunkHead*>(_pBaseAlloctor->myMalloc(MYJSON_ALIGN(sizeof(ChunkHead)) + size));
		if (ptr)
		{
			ptr->total_size = size;
			ptr->used_size = 0;
			ptr->next = _pChunkHead;
			_pChunkHead = ptr;
		}

		return ptr;
	}

private:
	typedef struct _ChunkHead				//this linked list save chunks info
	{
		size_t			used_size;
		size_t			total_size;
		_ChunkHead*		next;
	}ChunkHead;

	ChunkHead		*_pChunkHead;			//链表头，每次添加chunk都在头这里添加
	BaseAlloctor	*_pBaseAlloctor;
	BaseAlloctor	*_pOwnBaseAlloctor;
	size_t			_sMinAllocSize;
	size_t			_sTotalSize;
	size_t			_sUsedSize;
};


#endif // !MY_JSON_ALLOCATOR_H
