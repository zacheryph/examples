# example drb service
#require 'mymod'

class SystemInfo
  def uname
    sys = `uname -a`
    sys.sub! "\n", ""
    sys
  end

  def uptime
    `uptime`.sub! "\n", ""
  end

  def register(mod)
    @rem = mod
    puts mod.inspect
  end

  def call_reg
    puts "Message called by Server TO Client: "+@rem.get_message
  end

  def initialize
  end
end


require 'drb' # WOOHOO

DRb.start_service nil, SystemInfo.new
puts DRb.uri
DRb.thread.join
