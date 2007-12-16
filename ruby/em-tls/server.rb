#!/usr/bin/env ruby
require 'eventmachine'

module TlsServer
  def post_init
    #puts "tls_server: starting tls"
    #start_tls
  end

  def receive_data(data)
    puts "tls_server: recieved: #{data}\n"
    send_data "hi, TlsServer got your message"
  end
end

EventMachine::run do
  EventMachine::start_server '127.0.0.1', 21455, TlsServer
end
