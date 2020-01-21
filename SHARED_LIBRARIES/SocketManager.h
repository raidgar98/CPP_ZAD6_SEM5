#pragma once

#include "BufferedSocket.h"

using socket_type = BufferedSocket;
using socket_holder = std::shared_ptr<socket_type>;

using work = std::function<void( socket_holder src )>;

struct pair
{
	using Thread = std::shared_ptr<std::thread>;
	inline const static number threads_in_pair = 2;

	Thread listener, worker;
	socket_holder for_listener, for_worker;

	std::condition_variable_any incoming_work;
	std::atomic_bool work_is_waiting;

	std::atomic_bool finish;
	std::shared_mutex mtx;

	pair( port_num port, work to_do_on_accept);
	~pair();
};

class SocketManager
{
private:

	std::list<pair> __m_running;

public:

	SocketManager( work _to_do, const number = 2, const port_num p_num = 9001 ) noexcept;

	port_num port = 9001;
	work to_do;

	void set_pair_count( const number count );
	number get_pair_count() const noexcept;

};

