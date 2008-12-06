/* boost_test.cc: example boost::asio timer code.
 *
 * Mac OS X (boost installed in MacPorts)
 *    g++ -o bt -I/opt/local/include boost_test.cc -L/opt/local/lib -lboost_system-mt
 *
 * Most other Systems:
 *    g++ -o bt boost_test.cc -lboost_system-mt
 */
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


/* handler object */
class printer
{
public:
  printer(boost::asio::io_service &io):
    timer_(io, boost::posix_time::seconds(2)),
    count_(0)
  {
    timer_.async_wait(boost::bind(&printer::print, this));
  }

  ~printer()
  {
    std::cout << "Final Count: " << count_ << ".\n";
  }

  void print()
  {
    if (count_ < 5) {
      std::cout << "running: " << count_++ << ".\n";

      timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(2));
      timer_.async_wait(boost::bind(&printer::print, this));
    }
  }

private:
  boost::asio::deadline_timer timer_;
  int count_;
};


int main (int argc, char const **argv)
{
  boost::asio::io_service ios;
  printer p(ios);

  ios.run();

  return 0;
}
