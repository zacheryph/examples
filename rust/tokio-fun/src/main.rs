extern crate futures;
extern crate tokio;
extern crate tokio_inotify;

use std::path::Path;
use std::time::{Duration, Instant};

use futures::future::lazy;
use futures::prelude::*;
use futures::stream::Stream;
use tokio::timer::Interval;
use tokio_inotify::AsyncINotify;
use tokio_inotify::{IN_CLOSE_WRITE, IN_MODIFY};

fn main() {
    // inotify testing
    // we do the directory. if you do a single file you would have to do
    // a notifier & future for each file as the Event doesnt give a `name`
    let file_name = String::from(".");
    let notifier = AsyncINotify::init().unwrap();
    notifier
        .add_watch(Path::new(&file_name), IN_MODIFY | IN_CLOSE_WRITE)
        .unwrap();

    let events = notifier
        .for_each(|ev| {
            println!("Event:{:?} => {:?}", ev.name.to_str(), ev);
            Ok(())
        })
        .map_err(|_| ());

    let tick = Interval::new(Instant::now(), Duration::from_secs(10))
        .for_each(|ts| {
            println!("tick: {:?}", ts);
            Ok(())
        })
        .map_err(|_| ());

    let kicker = lazy(|| {
        tokio::spawn(tick);
        tokio::spawn(events);
        Ok(())
    });

    // tokio::run(events);
    tokio::run(kicker);
    println!("Hello, world!");
}
