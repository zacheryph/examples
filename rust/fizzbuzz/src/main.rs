/// My fizzbuzz for rust cause...
use std::env::args;
use std::fmt::{Display, Error, Formatter};
use std::ops::Add;

struct FizzBuzz(i32);

impl FizzBuzz {
    fn new(n: i32) -> FizzBuzz {
        FizzBuzz(n)
    }
}

impl Display for FizzBuzz {
    fn fmt(&self, f: &mut Formatter) -> Result<(), Error> {
        match (self.0, self.0 % 3, self.0 % 5) {
            (_, 0, 0) => write!(f, "FizzBuzz"),
            (_, 0, _) => write!(f, "Fizz"),
            (_, _, 0) => write!(f, "Buzz"),
            (i, _, _) => write!(f, "{}", i),
        }
    }
}

impl Add<i32> for FizzBuzz {
    type Output = FizzBuzz;

    fn add(self, other: i32) -> FizzBuzz {
        FizzBuzz(self.0 + other)
    }
}

fn main() {
    let max: i32 = match args().nth(1) {
        Some(n) => n.parse::<i32>().unwrap(),
        None => {
            println!("usage: fizzbuzz <num>");
            ::std::process::exit(1)
        }
    };

    let mut num = FizzBuzz::new(0);
    for _ in 0..max {
        num = num + 1;
        println!("{}", num);
    }
}
