#pragma once

#include <memory>

#ifdef _M_X64
using int_ptr = long long;
#else
using int_ptr = int;
#endif


template<const int Num>
struct Space
{
	struct ToKibiByte
	{
		static_assert( Num != 0,"Num cannot be 0" );
		static constexpr unsigned value = Num * 1024u;
	};
	struct ToMibiByte
	{
		static constexpr unsigned value = ( ToKibiByte::value ) * 1024u;
	};
	struct ToGibiByte
	{
		static constexpr unsigned value = ( ToMibiByte::value ) * 1024u;
	};
};

class Mempool
{
public:
	Mempool();
	~Mempool();

	void ResetWorkSpace();
	
	unsigned *AllocateTexture( unsigned Width, unsigned Height );
	float *AllocateFromWorkspace( unsigned VertexCount );
	template <class T> T*AllocateBlock( unsigned BlockSize )
	{		
		T* pmem = nullptr;

		if( m_bufferRemaining > BlockSize )
		{
			pmem = reinterpret_cast<T*>( m_pNextAvail );
			m_bufferRemaining -= BlockSize;
			m_pNextAvail = &m_pMempool[ m_bufferSize - m_bufferRemaining ];
		}

		return pmem;
	}
	template <class T> T*AllocateBlockAligned( unsigned BlockSize )
	{
		T* pmem = nullptr;

		if( m_bufferRemaining > BlockSize + 3 )
		{
			int_ptr addr = reinterpret_cast<int_ptr>( m_pNextAvail );
			addr += 15;
			addr >>= 4;
			addr <<= 4;

			int_ptr end = reinterpret_cast<int_ptr>( &m_pMempool[ m_bufferSize ] );
			m_pNextAvail = reinterpret_cast<char*>( addr );
			pmem = reinterpret_cast<T*>( m_pNextAvail );
			m_bufferRemaining = end - addr - BlockSize;
			m_pNextAvail = &m_pMempool[ m_bufferSize - m_bufferRemaining ];
		}
		return pmem;
	}
private:
	std::unique_ptr<char[]>			m_pMempool;
	char							*m_pNextAvail, *m_pWSNextAvail, *m_pTSNextAvail;
	char							*m_pTextureSpace, *m_pWorkSpace;
	unsigned						m_bufferSize, m_bufferRemaining;
	unsigned						m_WSBufferSize, m_WSBufferRemaining;
	unsigned						m_TSBufferSize, m_TSBufferRemaining;
};

