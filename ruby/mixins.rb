module MyMixin
  def class_name
    self.class.to_s
  end
end

module Magical
  def magic
    puts "Magical: #{rand(100)}"
  end
end

class ClassOne
  include MyMixin
  include Magical
end

class ClassTwo
  include MyMixin
  include Magical
end

c1 = ClassOne.new
puts "c1.class_name:#{c1.class_name}"
c1.magic

c2 = ClassTwo.new
puts "c2.class_name: #{c2.class_name}"
c2.magic

