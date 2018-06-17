extern crate capnpc;

use capnpc::CompilerCommand;

fn main() {
    CompilerCommand::new().file("proto.capnp").run().unwrap()
}
