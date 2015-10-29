module WithRefine
  module StringHack
    refine String do
      def /(s)
        "#{self}/#{s}"
      end
    end
  end

  class Path
    using StringHack

    def method_missing(meth)
      meth.to_s
    end
  end
end

def RefinePath(&block)
  path = WithRefine::Path.new
  path.instance_eval &block
end

# refinements are lexically scoped.
# which requires using refinement here, not where the block is executed.
# sad face.
using WithRefine::StringHack
puts (RefinePath { some/more/goes/here })

#########################################################

def ClassPath(&block)
  ClassPath.run &block
end

class ClassPath
  attr_accessor :path

  def self.run(&block)
    self.new("").instance_eval(&block).path
  end

  def initialize(path)
    self.path = path
  end

  def method_missing(meth)
    self.class.new(meth.to_s)
  end

  def to_s
    path
  end

  def /(s)
    self.class.new("#{path}/#{s}")
  end
end

puts (ClassPath { some/more/goes/here })
