def wrapper(f):
    def w():
        print(">:magic")
        f()
        print("<:magic")

    return w

@wrapper
def important():
    print("important message")

def supersmart(x):
    print("E:supersmart:" + x)
    def decor(f):
        def w():
            print(">:decor:" + x)
            f()
            print("<:decor")
        return w

    print("X:supersmart")
    return decor

@supersmart("admin")
def smarter():
    print("smarter decorator")

class KlassDecorator:
    def __init__(self, f):
        print(">:__init__")
        self._f = f
        print("<:__init__")

    def __call__(self):
        print(">:__call__")
        self._f()
        print("<:__call__")

@KlassDecorator
def morestuff():
    print("more stuff")

print("\nMethods Defined\n")

#important()
smarter()
print()
morestuff()

