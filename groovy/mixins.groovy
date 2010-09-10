class MyMixin {
  void print_me() {
    print "See me Print\n"
  }
}

@Mixin(MyMixin)
class Donkey {}

donkey = new Donkey()
print donkey.print_me()

