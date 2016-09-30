#include "Mempool.h"


Mempool::Mempool()
	:
	m_pMempool( new char[ Space<1>::ToGibiByte::value ] ),
	m_pNextAvail( m_pMempool.get() ),
	m_bufferSize( Space<1>::ToGibiByte::value ),
	m_bufferRemaining( Space<1>::ToGibiByte::value )
{
	m_WSBufferSize = Space<256>::ToMibiByte::value;
	m_WSBufferRemaining = m_WSBufferSize;
	m_pWorkSpace = AllocateBlockAligned<char>( m_WSBufferSize );
	m_pWSNextAvail = m_pWorkSpace;

	m_TSBufferSize = Space<256>::ToMibiByte::value;
	m_TSBufferRemaining = m_TSBufferSize;
	m_pTextureSpace = AllocateBlock<char>( m_TSBufferSize );
	m_pTSNextAvail = m_pTextureSpace;
}


Mempool::~Mempool()
{}

void Mempool::ResetWorkSpace()
{
	m_pWSNextAvail = m_pWorkSpace;
	m_WSBufferRemaining = m_WSBufferSize;
}

unsigned * Mempool::AllocateTexture( unsigned Width, unsigned Height )
{
	unsigned* pmem = nullptr;
	const unsigned blockSize = Width * Height * 4;

	if( m_TSBufferRemaining > blockSize )
	{
		pmem = reinterpret_cast<unsigned*>( m_pTSNextAvail );
		m_TSBufferRemaining -= blockSize;
		m_pTSNextAvail = &m_pTextureSpace[ m_TSBufferSize - m_TSBufferRemaining ];
	}

	return pmem;
}

float * Mempool::AllocateFromWorkspace( unsigned VertexCount )
{
	float *pmem = nullptr;
	unsigned blockSize = VertexCount * sizeof( float );

	if( m_WSBufferRemaining > blockSize )
	{
		int_ptr addr = reinterpret_cast<int_ptr>( m_pWSNextAvail );
		addr += 15;
		addr >>= 4;
		addr <<= 4;

		int_ptr end = reinterpret_cast<int_ptr>( &m_pWorkSpace[ m_WSBufferSize ] );
		m_pWSNextAvail = reinterpret_cast<char*>( addr );
		pmem = reinterpret_cast<float*>( m_pWSNextAvail );
		m_WSBufferRemaining = end - addr - blockSize;
		m_pWSNextAvail = &m_pWorkSpace[ m_WSBufferSize - m_WSBufferRemaining ];

	}

	return pmem;
}
