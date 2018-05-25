extern crate tokio;

use std::io::{BufRead, BufReader};
use std::net::SocketAddr;
// use std::sync::{Arc, Mutex};

use tokio::io::{lines, Lines, ReadHalf};
use tokio::net::{TcpListener, TcpStream};
use tokio::prelude::*;

/// Link
#[derive(Debug)]
struct Link {
    nick: String,
    rx: Lines<BufReader<ReadHalf<TcpStream>>>,
}

/// register a new connection. handles registration of a new connection
/// ensuring all required details are gathered before joining the client
/// to the network. (sort of like IRC)
fn register(tcp: TcpStream) -> RegisterFuture {
    let addr = tcp.peer_addr().unwrap();
    println!("Peer: {}", addr);
    let (rx, tx) = tcp.split();
    let rx = lines(BufReader::new(rx));

    let fut = RegisterFuture {
        nick: None,
        rx: Some(rx),
    };

    fut
}

struct RegisterFuture {
    rx: Option<Lines<BufReader<ReadHalf<TcpStream>>>>,
    // tx: Option<?!?!>,
    nick: Option<String>,
}

impl Future for RegisterFuture {
    type Item = Link;
    type Error = ();

    fn poll(&mut self) -> Poll<Self::Item, Self::Error> {
        {
            let lines = self.rx.as_mut().unwrap();
            loop {
                match lines.poll() {
                    Ok(Async::Ready(n)) => {
                        self.nick = n;
                        break;
                    }
                    Ok(Async::NotReady) => return Ok(Async::NotReady),
                    Err(e) => return Err(()),
                }
            }
        }

        if let Some(_) = self.nick {
            Ok(Async::Ready(Link {
                rx: self.rx.take().unwrap(),
                nick: self.nick.take().unwrap(),
            }))
        } else {
            Ok(Async::NotReady)
        }
    }
}

fn main() {
    let addr: SocketAddr = "127.0.0.1:6142".parse().unwrap();

    let listener = TcpListener::bind(&addr).unwrap();
    let server = listener
        .incoming()
        .for_each(|conn| {
            let client = register(conn)
                // .and_then(|link| Peer::new(link, &state))
                .and_then(|link| {
                    println!("Link: {:?}", link); return Ok(())
                });
            // .map_err(|err| println!("Connection failure: {}", err));

            tokio::spawn(client);
            Ok(())
        })
        .map_err(|err| println!("Failed to accept connection: {}", err));

    println!("Starting Server @ {}", addr);
    tokio::run(server)
}
