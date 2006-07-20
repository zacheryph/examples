class MyModule
  attr_writer :message
  
  def initialize(msg)
    @message = msg
  end

  def get_message
    @message
  end
end
