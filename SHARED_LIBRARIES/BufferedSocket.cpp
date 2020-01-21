#include "BufferedSocket.h"

BufferedSocket::BufferedSocket( const SOCKET & sck)
	:Socket{ sck }
{}

void BufferedSocket::BSend( container data, const size_type dsize, const size_type bsize ) const
{
	assert( data != nullptr );
	assert( dsize >= 1B );
	assert( bsize >= 1B );

	num std_iterations = dsize / bsize;
	num change = dsize - ( std_iterations * bsize );
	char* iterator = data.get();

	while( std_iterations )
	{
		iterator += bsize;
		std_iterations--;
	}

	if( bsize >= 1B )
		Send( iterator, change );
}

void BufferedSocket::FBSend( const str & file_name, const size_type dsize, const size_type bsize ) const
{
	assert( dsize >= 1B );
	assert( bsize >= 1B );

	std::ifstream file{ file_name, std::ios::binary };

	assert( file.good() );
	container buff{ new char[bsize] };
	num already_read{ 0 };

	while( already_read + bsize <= dsize )
	{
		file.read( buff.get(), bsize );
		Send( buff.get(), bsize );
		already_read += bsize;
	}

	if( already_read != dsize )
	{
		file.read( buff.get(), dsize - already_read );
		Send( buff.get(), dsize - already_read );
	}

	file.close(); 
}