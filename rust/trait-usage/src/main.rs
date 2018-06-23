extern crate bincode;
extern crate serde;
extern crate sled;
#[macro_use]
extern crate serde_derive;

use std::io;

use bincode::{deserialize, serialize};
use serde::Serialize;
use sled::{ConfigBuilder, Tree};

fn open_sled() -> Result<Tree, io::Error> {
    let config = ConfigBuilder::new().path("./sled.db").build();

    let tree = Tree::start(config);
    match tree {
        Ok(t) => Ok(t),
        Err(_) => Err(io::Error::new(io::ErrorKind::Other, "Error Opening Tree")),
    }
}

#[derive(Debug, Serialize, Deserialize)]
struct One {
    name: String,
}

#[derive(Debug, Serialize, Deserialize)]
struct Two {
    name: String,
}

#[derive(Debug, Serialize, Deserialize)]
enum Types {
    One(Option<String>),
    Two(Option<String>),
    Three(Option<String>),
}

trait Keyed {
    fn store_key(&self) -> Types;
}

impl<'a> Keyed for &'a One {
    fn store_key(&self) -> Types {
        Types::One(Some(self.name.clone()))
    }
}

impl<'a> Keyed for &'a Two {
    fn store_key(&self) -> Types {
        Types::Two(Some(self.name.clone()))
    }
}

fn store_rec<T>(db: &Tree, rec: T)
where
    T: Keyed + Serialize,
{
    db.set(
        serialize(&rec.store_key()).unwrap(),
        serialize(&rec).unwrap(),
    ).unwrap();
}

fn main() {
    // let o1 = One { name: "o1".into() };
    // let o2 = One { name: "o2".into() };
    // let o3 = One { name: "o3".into() };
    // let t1 = Two { name: "t1".into() };
    // let t2 = Two { name: "t2".into() };
    // let t3 = Two { name: "t3".into() };

    let db = open_sled().unwrap();
    // store_rec(&db, &o1);
    // store_rec(&db, &o2);
    // store_rec(&db, &o3);
    // store_rec(&db, &t1);
    // store_rec(&db, &t2);
    // store_rec(&db, &t3);

    let iter = db.scan(&serialize(&Types::One(None)).unwrap());
    for rec in iter {
        let (k, v) = rec.unwrap();
        let rec: Types = deserialize(&k).unwrap();
        match rec {
            Types::One(tk) => {
                let r: One = deserialize(&v).unwrap();
                println!("ONE: {:?} => {:?}", tk, r)
            }
            _ => break,
        }
    }

    let iter = db.scan(&serialize(&Types::Two(None)).unwrap());
    for rec in iter {
        let (k, v) = rec.unwrap();
        let rec: Types = deserialize(&k).unwrap();
        match rec {
            Types::Two(tk) => {
                let r: Two = deserialize(&v).unwrap();
                println!("TWO: {:?} => {:?}", tk, r)
            }
            _ => break,
        }
    }
}
