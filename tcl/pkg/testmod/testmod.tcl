
namespace eval testmod {
  proc print_msg {msg} {
    puts "testmod: $msg"
  }
}

modloader::set_callback testmod::print_msg

package provide testmod 1.0
puts "testmod: testmod loaded successfully."
