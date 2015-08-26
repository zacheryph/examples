#!/usr/bin/env ruby
# Implements Go 'defer' keyword
#
require 'delegate'
require 'logger'

class DeferBlock < SimpleDelegator
  def self.run(base, &block)
    d = self.new(base)
    d.instance_eval &block
    d.__cleanup
  end

  def defer(&block)
    __stack.unshift block
  end

  def __stack
    @__stack ||= []
  end

  def __cleanup
    __stack.each do |blk|
      blk.call
    end
  end
end

module Defer
  def self.run(&block)
    DeferBlock.run(nil, &block)
  end

  def with_defer(&block)
    DeferBlock.run(self, &block)
  end
end

class Minion
  include Defer

  def call
    with_defer do
      puts "Call: First"
      defer { puts "Call: Defered 1" }
      depend
      defer { puts "Call: Defered 2" }
      puts "Call: Last Statement"
    end
  end

  def depend
    with_defer do
      puts "Depend: First"
      defer { puts "Depend: Defered" }
      puts "Depend: Last Statement"
    end
  end

  def interpolate
    important = "Im Here"

    with_defer do
      defer { puts "ensure we do crap: #{important}" }
      puts "And we are: #{important}"
    end
  end

  def direct
    we_need = "Im Here Still"

    with_defer do
      defer do
        print "Direct Defer: "
        puts we_need.inspect
      end

      printf "Direct Last: "
      puts we_need.inspect
    end
  end
end

Minion.new.call
# Call: First
# Depend: First
# Depend: Last Statement
# Depend: Defered
# Call: Last Statement
# Call: Defered 2
# Call: Defered 1

puts
Minion.new.interpolate
# And we are: Im Here
# ensure we do crap: Im Here

puts
Minion.new.direct
# "Im Here Still"
# "Im Here Still"

# A half useful example
class SomethingAwesome
  include Defer

  def do_alot_of_stuff
    with_defer do
      log = Log.open_some_file
      defer { log.close } # really ensure we close our log
      #...
      #  lots of important stuff here
      #...
    end
  end
end
