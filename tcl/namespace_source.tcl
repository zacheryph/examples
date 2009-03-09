#!/usr/bin/env tclsh

set conf config.tcl

namespace eval config {
  source $conf
}

puts "IRC Server: $config::server:$config::port"
puts "  IRC Nick: $config::nick"
