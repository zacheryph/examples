#!/usr/bin/env tclsh

namespace eval modloader {
  variable cb 0
  proc set_callback {meth} {
    variable cb
    set cb $meth
  }

  proc call_methods {} {
    variable cb
    puts "modload: calling callbacks"
    $cb "message from modload"
  }
}

lappend auto_path "[pwd]/pkg"

puts "modload: attempting to load testmod"
package require testmod
modloader::call_methods
