extern crate capnp;
#[macro_use]
extern crate capnp_rpc;

pub mod proto_capnp {
    include!(concat!(env!("OUT_DIR"), "/proto_capnp.rs"));
}

use capnp::capability::Promise;
use capnp::Error;
use proto_capnp::{connect, session};

pub struct Service;

impl connect::Server for Service {
    fn register(
        &mut self,
        args: connect::RegisterParams,
        mut res: connect::RegisterResults,
    ) -> Promise<(), Error> {
        let n = pry!(args.get()).get_name().unwrap().to_string();
        println!("Connect: {:?}", n);
        let h = Host { name: n };
        res.get()
            .set_session(session::ToClient::new(h).from_server::<capnp_rpc::Server>());
        Promise::ok(())
    }
}

pub struct Host {
    name: String,
}

impl session::Server for Host {
    fn get_number(
        &mut self,
        _args: session::GetNumberParams,
        mut res: session::GetNumberResults,
    ) -> Promise<(), Error> {
        res.get().set_val(42);
        Promise::ok(())
    }

    fn get_name(
        &mut self,
        _args: session::GetNameParams,
        mut res: session::GetNameResults,
    ) -> Promise<(), Error> {
        res.get().set_name(&self.name);
        Promise::ok(())
    }
}
