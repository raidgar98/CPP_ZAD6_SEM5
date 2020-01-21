#pragma once

#include "../../SHARED_LIBRARIES/SocketManager.h"

int main()
{
	WSAData info;
	WSAStartup( MAKEWORD( 2, 2 ), &info );

	SocketManager mngr{ []( socket_holder sck ){

		container _{ new char[path_length] };
		memset( _.get(), 0, path_length );
		std::cout << signature << "waiting for path to file..." << std::endl;
		sck->Receive( _, path_length );
		const str path{ _.get() };
		std::cout << signature << "recived path to file: '" << path << "'" << std::endl;
		const num file_size = std::filesystem::file_size( path.data() );
		std::cout << signature << "size of file: " << sufix( file_size ) << std::endl;

		container f{ new char[hash_size] };
		memset( f.get(), 0, hash_size );
		std::ifstream file( path.data(), std::ios::binary );
		std::cout << signature << "creating hash..." << std::endl;
		file.read( f.get(), hash_size );
		file.close();
		const str hash{ get_hash(f, hash_size) };
		std::cout << signature << "created hash: '" << hash << "'" << std::endl;

		
		std::cout << signature << "sending hash..." << std::endl;
		f.reset( new char[hash.size()] );
		memcpy( f.get(), hash.data(), hash_length );
		sck->Send( f, hash_length );
		std::cout << signature << "hash has been sent. sending file..." << std::endl;
		std::cout << signature << "sending file..." << std::endl;
		sck->FBSend( path.data(), file_size );
		std::cout << signature << "file has been sent. closing connection." << std::endl;

	},3, 9001 };

	while( true ) std::this_thread::yield();

	WSACleanup();
	return 0;
}