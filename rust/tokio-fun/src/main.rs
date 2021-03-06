extern crate futures;
extern crate tokio;
extern crate inotify;

use std::time::{Duration, Instant};

use futures::future::lazy;
use futures::prelude::*;
use futures::stream::Stream;
use inotify::{Inotify, WatchMask};
use tokio::timer::Interval;

fn main() {
    // inotify testing
    // we do the directory. if you do a single file you would have to do
    // a notifier & future for each file as the Event doesnt give a `name`
    //
    // NOTE: inotify-0.5 has an EventStream implementation but it is not
    //       a usable solution until the following PR is merged/released.
    //       until then the future always reschedules itself pegging a CPU.
    //       https://github.com/inotify-rs/inotify/pull/105
    // let file_name = String::from(".");
    // let notifier = AsyncINotify::init().unwrap();
    // notifier
    //     .add_watch(Path::new(&file_name), IN_MODIFY | IN_CLOSE_WRITE)
    //     .unwrap();
    // 
    // let events = notifier
    //     .for_each(|ev| {
    //         println!("Event:{:?} => {:?}", ev.name.to_str(), ev);
    //         Ok(())
    //     })
    //     .map_err(|_| ());
    let mut notifier = Inotify::init().unwrap();
    notifier.add_watch(".", WatchMask::MODIFY | WatchMask::CLOSE_WRITE).unwrap();
    let events = notifier.event_stream().for_each(|ev| {
        println!("Event: {:?}", ev);
        Ok(())
    })
    .map_err(|e| println!("Inotify Error: {:?}", e));

    // timer::Interval / ticker
    // this just shows having a future/stream interval executing
    // soemthing every 10 seconds
    let tick = Interval::new(Instant::now(), Duration::from_secs(10))
        .for_each(|ts| {
            println!("tick: {:?}", ts);
            Ok(())
        })
        .map_err(|_| ());

    // since tokio::run() blocks, we need a way to initiate both
    // futures. you cannot tokio::spawn() before tokio::run() because
    // the reactor is not started yet. using lazy here just lets us
    // spawn the independent futures to go off and do their own thing
    let kicker = lazy(|| {
        tokio::spawn(tick);
        tokio::spawn(events);
        Ok(())
    });

    // tokio::run(events);
    tokio::run(kicker);
    println!("Hello, world!");
}
