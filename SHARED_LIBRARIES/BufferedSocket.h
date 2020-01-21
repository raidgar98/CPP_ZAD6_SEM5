#pragma once
#include "Socket.h"


class BufferedSocket :
	public Socket
{
public:

	BufferedSocket() = default;
	explicit BufferedSocket( const SOCKET& );
	BufferedSocket( const BufferedSocket& ) = delete;
	BufferedSocket( BufferedSocket&& ) = default;

	void BSend( container, const size_type, const size_type = 512B ) const;
	void FBSend( const str&, const size_type = ~0ull, const size_type = 512B ) const;

};

