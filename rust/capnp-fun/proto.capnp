@0x932050e2a4b03124;

interface Session {
  getNumber @0 () -> (val :Int64);
  getName @1 () -> (name :Text);
}

interface Connect {
  register @0 (name :Text) -> (session :Session);
}
