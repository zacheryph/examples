#!/usr/bin/env ruby
require 'eventmachine'

module TlsClient
  def post_init
    #puts "tls_client: post_init"
    #start_tls
  end

  def connection_completed
    puts "tls_client: connection_completed (we should be tls?)"
    send_data "This is TlsClient"
  end

  def receive_data(data)
    puts "tls_client: received: #{data}"
    close_connection_after_writing
  end

  def unbind
    puts "tls_client: connection closed"
    EventMachine::stop_event_loop
  end
end

EventMachine::run do
  EventMachine::connect '127.0.0.1', 21455, TlsClient
end
