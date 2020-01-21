#include "SocketManager.h"

pair::pair(
	port_num port,
	work to_do_on_accept
)
{
	for_listener.reset( new socket_type() );
	work_is_waiting.store( false );
	finish.store( false );
	worker.reset( new std::thread( []( pair* parent, work to_do_on_accept ){
		std::shared_lock<std::shared_mutex> lck( parent->mtx );

		while( !( parent->finish.load() ) )
		{
			while( !(parent->finish.load()) && !(parent->work_is_waiting.load()) )
				parent->incoming_work.wait( lck );

			if( parent->finish.load() )
				return;

			auto fw = parent->for_worker;
			lck.unlock();

			to_do_on_accept( fw );

			parent->for_worker.reset();
			parent->work_is_waiting.store( false );
			lck.lock();
		}
	}, this, to_do_on_accept ) );
	listener.reset( new std::thread( []( pair* parent, const port_num port ){
		std::shared_lock<std::shared_mutex> lck( parent->mtx );
		parent->for_listener->Listen( port );

		while( !parent->finish.load() )
		{
			sockaddr_in address;
			SOCKET tmp = parent->for_listener->Accept( address );
			if( tmp == INVALID_SOCKET ) continue;
			if( parent->work_is_waiting.load() )
			{
				shutdown( tmp, SD_BOTH );
				closesocket( tmp );
				continue;
			}
			parent->work_is_waiting.store( true );
			parent->for_worker.reset( new socket_type( tmp ) );
			lck.unlock();
			parent->incoming_work.notify_all();
			//std::this_thread::sleep_for( 1s );
			lck.lock();
		}

	}, this, port ) );
	
}

pair::~pair()
{
	finish.store( true );

	for_listener.~shared_ptr();
	for_worker.~shared_ptr();

	incoming_work.notify_all();

	listener->~thread();
	worker->~thread();
}

SocketManager::SocketManager( work _to_do, const number count, const port_num p_num ) noexcept
	:to_do{ _to_do }, port{ p_num }
{
	set_pair_count( count );
}

void SocketManager::set_pair_count( const number count )
{
	//assert( count <= get_cores_count() / pair::threads_in_pair );
	if( count == get_cores_count() ) return;
	if( count == 0 )__m_running.clear();
	if( count < get_pair_count() )
		while( get_pair_count() != count )
			__m_running.pop_back();
	else
		while( get_pair_count() != count )
			__m_running.emplace_back( port + (__m_running.size()), to_do );
}

number SocketManager::get_pair_count() const noexcept
{
	return __m_running.size();
}
