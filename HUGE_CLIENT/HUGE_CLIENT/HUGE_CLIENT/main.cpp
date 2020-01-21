#include "../../../SHARED_LIBRARIES/Socket.h"

constexpr char const * const PATH = "C:/Users/raidg/Downloads/4k.jpg";

int main()
{
	WSAData info;
	WSAStartup( MAKEWORD( 2, 2 ), &info );

	std::unique_ptr<Socket> sck{ new Socket };
	sck->Connect( "127.0.0.1", 9001 );

	container path{ new char[path_length] };
	memset( path.get(), NULL, path_length );
	memcpy( path.get(), PATH, strlen( PATH ) );
	std::cout << signature << "sending path to file..." << std::endl;
	sck->Send( path, path_length );

	container data{ new char[10MB] };
	memset( data.get(), NULL, 10MB );
	std::cout << signature << "waiting for hash..." << std::endl;
	snum received = sck->Receive( data, hash_length );
	str hash( hash_length, '\0' );
	memcpy( hash.data(), data.get(), hash_length );
	std::cout << signature << "hash: '"<<hash<<"' received." << std::endl;

	data.reset( new char[10MB] );
	memset( data.get(), NULL, 10MB );
	std::cout << signature << "waiting for data..." << std::endl;
	received = sck->Receive( data, 10MB );
	std::cout << signature << "received: " << sufix(received) << std::endl;
	
	std::ofstream file{ "file.jpg", std::ios::binary };
	assert( file.good() );
	file.write( data.get(), received );
	file.close();

	container f{ new char[hash_size] };
	memset( f.get(), NULL, hash_size );
	std::ifstream ifile( PATH, std::ios::binary );
	std::cout <<signature<< "creating hash..." << std::endl;
	ifile.read( f.get(), hash_size );
	ifile.close();
	const str chash{ get_hash(f, hash_size ) };
	std::cout << signature << "created hash: '" << chash << "'"<< std::endl;

	sck.reset();
	WSACleanup();

	std::cout << signature << "comparing hashes..." << std::endl;
	assert( std::stoull(chash) == std::stoull(hash) );
	std::cout << signature << "hashes comparation successfull" << std::endl;
	std::cin.get();
	return 0;
}