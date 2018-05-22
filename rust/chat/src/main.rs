extern crate tokio;

use std::net::SocketAddr;
// use std::sync::{Arc, Mutex};

use tokio::io;
use tokio::io::{ReadHalf, WriteHalf};
use tokio::net::{TcpListener, TcpStream};
use tokio::prelude::*;

/// register a new connection. handles registration of a new connection
/// ensuring all required details are gathered before joining the client
/// to the network. (sort of like IRC)
fn register(tcp: TcpStream) -> RegisterFuture {
    let addr = tcp.peer_addr().unwrap();
    println!("Peer: {}", addr);
    let (rx, tx) = tcp.split();
    let copy = io::copy(rx, tx);

    let fut = RegisterFuture { copy };
    fut
}

struct RegisterFuture {
    copy: io::Copy<ReadHalf<TcpStream>, WriteHalf<TcpStream>>,
}

impl Future for RegisterFuture {
    type Item = ();
    type Error = ();

    fn poll(&mut self) -> Poll<Self::Item, Self::Error> {
        match self.copy.poll() {
            Ok(Async::Ready(_)) => Ok(Async::Ready(())),
            Ok(Async::NotReady) => Ok(Async::NotReady),
            Err(_) => Err(()),
        }
    }
}

fn main() {
    let addr: SocketAddr = "127.0.0.1:6142".parse().unwrap();

    let listener = TcpListener::bind(&addr).unwrap();
    let server = listener
        .incoming()
        .for_each(|conn| {
            let client = register(conn);
            //     .and_then(|link| Peer::new(link, &state))
            //     .map_err(|err| println!("Connection failure: {}", err));

            tokio::spawn(client);
            Ok(())
        })
        .map_err(|err| println!("Failed to accept connection: {}", err));

    println!("Starting Server @ {}", addr);
    tokio::run(server)
}
