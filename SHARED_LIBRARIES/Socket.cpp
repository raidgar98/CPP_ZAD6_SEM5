#include "Socket.h"

#include <assert.h>
#include <vector>
#include <thread>

const socket_destructor_type Socket::sck_destructor = []( SOCKET* sck ){
	if( sck == nullptr ) return;
	shutdown( *sck , SD_BOTH );
	closesocket( *sck );
};
//-		sck	0x014cb808 {300}	unsigned int *
Socket::Socket()
	:__m_socket( new SOCKET( socket( AF_INET , SOCK_STREAM , IPPROTO_TCP ) ) , sck_destructor )
{}

Socket::Socket( const SOCKET& src )
	: __m_socket( new SOCKET( src ) , sck_destructor )
{}

void Socket::Send( container data , const size_type dsize ) const
{
	Send( data.get() , dsize );
}

snum Socket::Receive( container& save_to, const size_type dsize ) const
{
	assert( dsize >= 1B );

	container buffor{ new char[ ( ( dsize / buffer_size ) + 1 ) * buffer_size ] };
	memset( buffor.get(), NULL, ( ( dsize / buffer_size ) + 1 ) * buffer_size );
	char* iterator = buffor.get();
	snum ret = 0;
	snum total_data_received_count = 0;

	for( ever )
	{
		ret = recv( *__m_socket, iterator, buffer_size, NULL );
		if( ret <= 0 ) break;
		total_data_received_count += ret;
		if( total_data_received_count >= dsize )
			break;
		else
			iterator += ret;
	}
	save_to.reset( new char[dsize] );
	memcpy( save_to.get(), buffor.get(), dsize );
	return total_data_received_count;
}

void Socket::Listen( const port_num port , const std::chrono::milliseconds& timeout ) const
{
	std::unique_ptr<sockaddr_in> address_1{ new sockaddr_in{} };

	address_1->sin_family = AF_INET;
	address_1->sin_addr.s_addr = INADDR_ANY;
	address_1->sin_port = htons( port );
	int ret = bind( *__m_socket , reinterpret_cast<sockaddr*>( address_1.get() ) , sizeof( sockaddr_in ) );
	if( ret ) THROW( bind_exception );
	if( timeout == 0ms )
		listen( *__m_socket , SOMAXCONN );
	else
		std::thread( [&](){
		listen( *__m_socket , SOMAXCONN );
		std::this_thread::sleep_for( timeout );
		try { shutdown( *__m_socket , SD_BOTH ); } catch( ... ) {}
	} ).detach();
}

SOCKET Socket::Accept( sockaddr_in& address ) const
{
	int _ = sizeof( sockaddr_in );
	return accept( 
		*__m_socket.get(), 
		reinterpret_cast<sockaddr*>( &address ), 
		&_ 
	);
}

void Socket::Connect( const str_v & address, const number port ) const
{
	sockaddr_in adr;
	adr.sin_family = AF_INET;
	adr.sin_addr.S_un.S_addr = inet_addr( address.data() );
	adr.sin_port = htons( port );
	if( adr.sin_addr.S_un.S_addr == INADDR_NONE )
	{
		hostent* maszyna = NULL; 
		maszyna = gethostbyname( address.data() );
		if( maszyna == NULL )
			THROW( resolving_host_exception );
		memcpy( &adr.sin_addr.S_un.S_addr, maszyna->h_addr, maszyna->h_length ); 
	}

	int ret = connect( *__m_socket, reinterpret_cast<sockaddr*>( &adr ), sizeof( adr ) );
	if( ret )
		THROW( connection_exception );
}

void Socket::Send( char * data , const size_type dsize ) const
{
	assert( dsize >= 1B );
	assert( data != nullptr );

	size_type already_sent{ 0 };

	while( already_sent < dsize )
	{
		snum ret = send( *__m_socket , data + already_sent , dsize - already_sent , NULL );
		if( ret == 0 || ret <= SOCKET_ERROR )
			THROW( connection_exception, "error occured on socket while sending" );
		already_sent += ret;
	}
}
