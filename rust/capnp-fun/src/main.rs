extern crate capnp;
extern crate capnp_rpc;
extern crate futures;
extern crate tokio;

extern crate rpc_fun;

use std::net::SocketAddr;

// use capnp::capability::Promise;
// use capnp::Error;
use capnp_rpc::twoparty::VatNetwork;
use capnp_rpc::{rpc_twoparty_capnp, RpcSystem};
use futures::{Future, Stream};
use tokio::executor::current_thread;
use tokio::net::{TcpListener, TcpStream};
use tokio::prelude::*;

use rpc_fun::proto_capnp::connect;
use rpc_fun::Service;

fn main() {
    let args: Vec<String> = ::std::env::args().collect();
    if args.len() < 2 {
        println!("need to set server or client");
        return;
    }

    match args[1].as_ref() {
        "server" => run_server(),
        "client" => run_client(args[2].to_string()),
        _ => println!("unknown"),
    }
}

fn run_server() {
    println!("server");

    let addr: SocketAddr = String::from("127.0.0.1:4389").parse().unwrap();
    let listener = TcpListener::bind(&addr).unwrap();

    let server = listener
        .incoming()
        .for_each(|conn| {
            let (reader, writer) = conn.split();
            let network = VatNetwork::new(
                reader,
                writer,
                rpc_twoparty_capnp::Side::Server,
                Default::default(),
            );

            let serv = connect::ToClient::new(Service).from_server::<capnp_rpc::Server>();

            let rpc_sys = RpcSystem::new(Box::new(network), Some(serv.clone().client));

            current_thread::spawn(rpc_sys.map_err(|_| ()));
            Ok(())
        })
        .map_err(|e| println!("Failed Accept: {:?}", e));

    let res = current_thread::block_on_all(server);
    match res {
        Ok(x) => println!("Server: {:?}", x),
        Err(err) => println!("Server Err: {:?}", err),
    }
}

fn run_client(n: String) {
    println!("client: {}", n);
    let addr: SocketAddr = String::from("127.0.0.1:4389").parse().unwrap();
    let stream = TcpStream::connect(&addr)
        .and_then(move |conn| {
            let (reader, writer) = conn.split();
            let network = Box::new(VatNetwork::new(
                reader,
                writer,
                rpc_twoparty_capnp::Side::Client,
                Default::default(),
            ));
            let mut rpc_sys = RpcSystem::new(network, None);
            let serv: connect::Client = rpc_sys.bootstrap(rpc_twoparty_capnp::Side::Server);

            current_thread::spawn(rpc_sys.map_err(|e| println!("Client Err: {:?}", e)));

            let mut reg = serv.register_request();
            reg.get().set_name(&n);
            let reg = reg.send()
                .promise
                .and_then(|res| {
                    let sess = res.get().unwrap().get_session().unwrap();

                    let num = sess.get_number_request().send().promise.and_then(|res| {
                        println!("Got Number: {}", res.get().unwrap().get_val());
                        Ok(())
                    });
                    current_thread::spawn(num.map_err(|e| println!("NumErr: {:?}", e)));

                    let nam = sess.get_name_request().send().promise.and_then(|res| {
                        println!("Got Name: {}", res.get().unwrap().get_name().unwrap());
                        Ok(())
                    });
                    current_thread::spawn(nam.map_err(|e| println!("NameErr: {:?}", e)));

                    Ok(())
                })
                .map_err(|e| println!("Reg Err: {:?}", e));

            current_thread::spawn(reg.map_err(|e| println!("ERR: {:?}", e)));

            Ok(())
        })
        .map_err(|e| println!("Err: {:?}", e));

    let res = current_thread::block_on_all(stream);
    match res {
        Ok(x) => println!("Client: {:?}", x),
        Err(err) => println!("Client Err: {:?}", err),
    }
}
