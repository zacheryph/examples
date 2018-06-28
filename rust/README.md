# rust

just learning rust

## very basics

* fizzbuzz - enough said
* ipcheck - simple reqwest/serde-json example
* loan - loan pay/amortization table exercise i like to do
* typeid - seeing how `std::any` works regarding ID'ing structs

## chat

chat server. a sort of 'tribute' to IRC protocol. A way for learning
Tokio and task handling, async ownership, more advanced stdlib/tokio
thank i should be biting so early. heavily based on Tokio docs/chat
example. Code commented for reasonings on doing things they way I
wanted to do them.

Read the commit history for details on where code derived and where
it is headed.

## tokio-fun

this is to get to know tokio a little bit more (.... or less? who knows.)

currently experiments with `tokio-inotify` and `futures::timer` for
running a job constantly and set intervals (tick)

## capnp-fun

quick experiment with capnp-rpc. would probably want to add some sort
of simple wrapper to make simple calls a little easier in tokio/futures.

## pswd-read

* serde
* csv
* sled
* bincode

quick test of serde & csv to see that it can read non-headered 'csv'
files into struct's. this also displays using bincode and sled for storing
data within a sled store, and pulling data back out.

## trait-usage

more serde & sled fun here. just getting more hands on with traits and
generics and lifetimes very little. need to do a lot more reading to
fully understand lifetime semantics though
