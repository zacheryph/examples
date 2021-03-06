extern crate bincode;
extern crate serde;
extern crate sled;
#[macro_use]
extern crate serde_derive;

use std::io;

use bincode::{deserialize, serialize};
use serde::de::DeserializeOwned;
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
enum KeyType {
    One,
    Two,
    Three,
}

#[derive(Debug, Serialize, Deserialize)]
struct Key(KeyType, Option<String>);

trait Keyed {
    fn store_key(&self) -> Key;
}

trait Storable {}

impl<'a> Keyed for &'a One {
    fn store_key(&self) -> Key {
        Key(KeyType::One, Some(self.name.clone()))
    }
}

impl<'a> Keyed for &'a Two {
    fn store_key(&self) -> Key {
        Key(KeyType::Two, Some(self.name.clone()))
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

// NOTE: DeserializeOwned is used here cause we return ownership of T
//       which is being deserialized from the sled store. Otherwise
//       we would have to keep instances of T around somewhere.
fn find_rec<'a, K, T>(db: &Tree, key: K) -> Result<Option<T>, io::Error>
where
    K: Serialize,
    T: DeserializeOwned,
{
    match db.get(&serialize(&key).unwrap()) {
        Ok(Some(v)) => {
            let ret: T = deserialize(&v).unwrap();
            Ok(Some(ret))
        }
        Ok(None) => Ok(None),
        Err(e) => Err(io::Error::new(io::ErrorKind::Other, e)),
    }
}

fn main() {
    let o1 = One { name: "o1".into() };
    let o2 = One { name: "o2".into() };
    let o3 = One { name: "o3".into() };
    let t1 = Two { name: "t1".into() };
    let t2 = Two { name: "t2".into() };
    let t3 = Two { name: "t3".into() };

    let db = open_sled().unwrap();
    store_rec(&db, &o1);
    store_rec(&db, &o2);
    store_rec(&db, &o3);
    store_rec(&db, &t1);
    store_rec(&db, &t2);
    store_rec(&db, &t3);

    let ro1: Option<One> = find_rec(&db, Key(KeyType::One, Some("o1".into()))).unwrap();
    match ro1 {
        Some(o) => println!("Found One: {:?}", o),
        None => println!("Not Found!"),
    }

    let ro1: Option<One> = find_rec(&db, Key(KeyType::One, Some("o5".into()))).unwrap();
    match ro1 {
        Some(o) => println!("Found One: {:?}", o),
        None => println!("Not Found!"),
    }

    let iter = db.scan(&serialize(&Key(KeyType::One, None)).unwrap());
    for rec in iter {
        let (k, v) = rec.unwrap();
        let rec: Key = deserialize(&k).unwrap();
        match rec {
            Key(KeyType::One, tk) => {
                let r: One = deserialize(&v).unwrap();
                println!("ONE: {:?} => {:?}", tk, r)
            }
            _ => break,
        }
    }

    let iter = db.scan(&serialize(&Key(KeyType::Two, None)).unwrap());
    for rec in iter {
        let (k, v) = rec.unwrap();
        let rec: Key = deserialize(&k).unwrap();
        match rec {
            Key(KeyType::Two, tk) => {
                let r: Two = deserialize(&v).unwrap();
                println!("TWO: {:?} => {:?}", tk, r)
            }
            _ => break,
        }
    }

    println!("OUR TREE: {:?}", db);
}
