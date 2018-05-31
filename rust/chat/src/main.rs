extern crate bytes;
extern crate tokio;

use std::io::{BufReader, ErrorKind};
use std::net::SocketAddr;
// use std::sync::{Arc, Mutex};

use bytes::BytesMut;

use tokio::io::{lines, Lines, ReadHalf, WriteHalf};
use tokio::net::{TcpListener, TcpStream};
use tokio::prelude::*;

type TcpLines = Lines<BufReader<ReadHalf<TcpStream>>>;

/// Link
#[derive(Debug)]
struct Link {
    addr: SocketAddr,
    nick: String,
    rx: TcpLines,
    tx: WriteHalf<TcpStream>,
    tx_buf: BytesMut,
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
        addr: Some(addr),
        nick: None,
        rx: Some(rx),
        tx: Some(tx),
    };

    fut
}

struct RegisterFuture {
    addr: Option<SocketAddr>,
    rx: Option<TcpLines>,
    tx: Option<WriteHalf<TcpStream>>,
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
                    Err(_) => return Err(()),
                }
            }
        }

        if let Some(_) = self.nick {
            Ok(Async::Ready(Link {
                addr: self.addr.take().unwrap(),
                rx: self.rx.take().unwrap(),
                tx: self.tx.take().unwrap(),
                tx_buf: BytesMut::new(),
                nick: self.nick.take().unwrap(),
            }))
        } else {
            Ok(Async::NotReady)
        }
    }
}

/// Peer handles client connections
struct Peer {
    link: Link,
}

impl Peer {
    fn new(link: Link) -> Peer {
        Peer { link }
    }
}

impl Future for Peer {
    type Item = ();
    type Error = ();

    fn poll(&mut self) -> Poll<Self::Item, Self::Error> {
        const LINES_PER_TICK: usize = 10;

        for i in 0..LINES_PER_TICK {
            match self.link.rx.poll() {
                Ok(Async::Ready(Some(l))) => {
                    let b = l.as_bytes();
                    self.link.tx_buf.reserve(b.len());
                    self.link.tx_buf.extend_from_slice(&b);
                    self.link.tx_buf.extend_from_slice(b"\r\n");
                    if i + 1 == LINES_PER_TICK {
                        task::current().notify();
                    }
                }
                Err(_) => return Err(()),
                _ => break,
            }
        }

        if !self.link.tx_buf.is_empty() {
            match self.link.tx.write(&self.link.tx_buf) {
                Ok(0) => return Err(()),
                Ok(n) => self.link.tx_buf.advance(n),
                Err(ref e) if e.kind() == ErrorKind::Interrupted => {}
                Err(_) => return Err(()),
            }
        }

        if !self.link.tx_buf.is_empty() {
            task::current().notify();
        }

        Ok(Async::NotReady)
    }
}

fn main() {
    let addr: SocketAddr = "127.0.0.1:6142".parse().unwrap();

    let listener = TcpListener::bind(&addr).unwrap();
    let server = listener
        .incoming()
        .for_each(|conn| {
            let client = register(conn).and_then(|link| Peer::new(link));
            // .map_err(|err| println!("Connection failure: {}", err));

            tokio::spawn(client);
            Ok(())
        })
        .map_err(|err| println!("Failed to accept connection: {}", err));

    println!("Starting Server @ {}", addr);
    tokio::run(server)
}
