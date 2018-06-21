use std::any::{Any, TypeId};

#[derive(Debug)]
struct One {
    name: String,
    uid: u32,
}

#[derive(Debug)]
struct Two {
    name: String,
}

fn get_type<T: ?Sized + Any>(_s: &T) -> TypeId {
    TypeId::of::<T>()
}

fn main() {
    let o1 = One {
        name: "One".into(),
        uid: 1,
    };
    let o2 = One {
        name: "Two".into(),
        uid: 2,
    };
    let o3 = One {
        name: "For".into(),
        uid: 4,
    };
    let t1 = Two { name: "One".into() };
    let t2 = Two { name: "Two".into() };
    let t3 = Two { name: "For".into() };
    println!("One: {:?}", get_type(&o1));
    println!("One: {:?}", get_type(&o2));
    println!("One: {:?}", get_type(&o3));
    println!("Two: {:?}", get_type(&t1));
    println!("Two: {:?}", get_type(&t2));
    println!("Two: {:?}", get_type(&t3));
}
