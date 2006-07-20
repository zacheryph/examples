#!/usr/bin/env ruby
# creates djbdns data lines for ip6 (forward and reverse) 
# without the need of any patches in djbdns
#!/usr/bin/env ruby
require 'ipaddr'
if 2 != ARGV.length
  puts "usage: add-ip6 <hostname> <ip6 address>"
  puts "output:"
  puts ":<host>:28:<encoded rdata for djbdns> # forward lookup line"
  puts "                                      # requires soa/ns for domain"
  puts "^<ip6 reverse>:<host>                 # reverse lookup line"
  puts "                                      # requires soa/ns for reverse domain"
  puts
  puts "# reverse domain lookup soa/ns:"
  puts "# this example shows the reverse soa/ns for: 2001:5c0:9164::/64"
  puts "Z4.6.1.9.0.c.5.0.1.0.0.2.ip6.arpa:<ns name>:<djbdns root email addr>"
  puts "&4.6.1.9.0.c.5.0.1.0.0.2.ip6.arpa:<ns ip>:<ns name>"
  exit
end

host = ARGV[0]
ip = IPAddr.new ARGV[1]
rdata = ip.to_i.to_s(16).scan(/../).map {|i| '\\%03o' % i.to_i(16) }.join

puts "# only put the : rule if we own/host this domain"
puts ":#{host}:28:#{rdata}"
puts "^#{ip.reverse}:#{host}"    
