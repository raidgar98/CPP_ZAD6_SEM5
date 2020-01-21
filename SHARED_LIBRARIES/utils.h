// pragmas
#pragma once
#pragma warning(disable : 4244)
#pragma warning(disable : 4996 4005)
//#pragma comment(lib, "ws2_32.lib")

// defines
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
//#define SI
#define ever ;;

// includes
#include <cmath>
#include <iostream>
#include <string>
#include <memory>
#include <string_view>
#include <chrono>
#include <limits>
#include <fstream>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <WinSock2.h>
#include <shared_mutex>
#include <functional>
#include <filesystem>
#include <cctype>
#include <assert.h>

// macros
#define THROW( exc, msg ) try{ throw exc( msg ); }catch( const exc& ){ throw; }
#define THROW( exc ) try{ throw exc(); }catch( const exc& ){ throw; }
#define CREATE_EXCEPTION( name, default_msg ) struct name : public std::exception { \
	name() :exception( default_msg ) {} \
	name(const char* msg) :exception( msg ) {} \
	name(const name&) = default; \
	name(name&&) = default; };

// used namespaces
using namespace std::literals::chrono_literals;

// basic types
using number = unsigned short;
using num = unsigned long long int;
using snum = long long int;
using str = std::string;
using str_v = std::string_view;
using real = double;
using size_type = num;
using container = std::shared_ptr<char>;

// constexpr
#ifndef SI
constexpr num base = 1024;
#else
constexpr num base = 1000;
#endif



// literals
constexpr num operator"" B( const num val ) noexcept
{
	return val;
}

constexpr num  operator"" KB( const num val ) noexcept
{
	return val * base;
}

constexpr num operator"" MB( const num val ) noexcept
{
	return val * base * base;
}

constexpr num operator"" GB( const num val ) noexcept
{
	return val * base * base * base;
}

constexpr num operator"" TB( const num val ) noexcept
{
	return val * base * base * base * base;
}

// exceptions
CREATE_EXCEPTION( connection_exception, "Exception occurred during connection" )
CREATE_EXCEPTION( bind_exception, "Exception occured while bound was created" )
CREATE_EXCEPTION( resolving_host_exception, "Exception occured while resolving hostname" )


// constexpresion cd...
constexpr num hash_size = 4KB;
constexpr number hash_length = 32;
constexpr number path_length = 256;

// support functions
inline str get_hash( container src, const size_type len )
{
	num hash = ~( 0ull );
	num* ptr = reinterpret_cast<num*>( src.get() );
	while( reinterpret_cast<num>( ptr + 1 ) < reinterpret_cast<num>( src.get() + len ) )
	{
		*ptr == 0 ? hash = ~hash : hash ^= *ptr;
		ptr++;
	}
	str pre_hash{ std::to_string( hash ) };
	while( pre_hash.length() < hash_length )
		pre_hash.insert( pre_hash.begin(), '0' );
	return pre_hash;
}

inline number get_cores_count()
{
	container ret{ new char[5] };
	for( int i = 0; i < 5; i++ )
		ret.get()[i] = '\0';

	GetEnvironmentVariable( "NUMBER_OF_PROCESSORS", ret.get(), 5 );

	return std::stoi( str( ret.get() ) );
}

inline std::ostream& signature( std::ostream& out )
{
	out << "Thread[" << std::this_thread::get_id() << "]: ";
	return out;
}

class sufix
{
	const double num;

public:

	sufix( const double _num )
		:num{ _num }
	{}

	std::ostream& operator()( std::ostream& out ) const noexcept
	{
		out << std::fixed << std::setprecision( 2 );
		if( num < 1KB )
			out << num << "B";
		else if( num >= 1KB && num < 1MB )
			out << num / std::pow( base, 1 ) << "KB";
		else if( num >= 1MB && num < 1GB )
			out << num / std::pow( base, 2 ) << "MB";
		else if( num >= 1GB && num < 1TB )
			out << num / std::pow( base, 3 ) << "GB";
		else
			out << num / std::pow( base, 4 ) << "TB";

		return out;
	}
};

inline std::ostream& operator<<( std::ostream& out, const sufix& src )
{
	return src( out );
}
