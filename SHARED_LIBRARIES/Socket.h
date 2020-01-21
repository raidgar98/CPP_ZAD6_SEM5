#pragma once

#include "utils.h"

// types for socket usage
using port_num = unsigned short;
using address_string = str_v;
using socket_destructor_type = std::function<void( SOCKET* )>;
using socket_storage = std::unique_ptr<SOCKET, socket_destructor_type>;

// constexpr
constexpr size_type buffer_size = 256B;

class Socket
{
private:

	// members
	socket_storage __m_socket;

public:

	// static members
	const static socket_destructor_type sck_destructor;

	Socket();
	explicit Socket( const SOCKET& );
	Socket( const Socket& ) = delete;
	Socket( Socket&& ) = default;

	void Send( container, const size_type ) const;
	snum Receive( container&, const size_type ) const;
	void Listen( const port_num, const std::chrono::milliseconds& timeout = 0ms ) const;
	SOCKET Accept( sockaddr_in& address ) const;
	void Connect( const str_v& address, const number port ) const;

protected:

	void Send( char *, const size_type ) const;

};

