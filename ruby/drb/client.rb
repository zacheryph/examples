# drb client
require 'drb'
require 'mymod'

DRb.start_service

sys = DRbObject.new nil, ARGV.shift

puts "calling uname method:"
puts "   #{sys.uname}"
puts "calling uptime method:"
puts "   #{sys.uptime}"

puts ; puts

mod = MyModule.new "Message Number 1"
sys.register DRb::DRbObject.new(mod)
sys.call_reg
mod.message = "Setting a New Message"
sys.call_reg

mod = MyModule.new "Creating a New Module"
sys.register DRb::DRbObject.new(mod)
sys.call_reg
