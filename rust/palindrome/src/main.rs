use std::env;

fn is_palindrome(word: &str) -> bool {
    let chars: Vec<_> = word.chars().collect();
    let len = word.len() - 1;
    let mut cur = 0;

    while len - cur > 0 {
        if chars[cur] != chars[len - cur] {
            return false;
        }
        cur += 1;
    }

    true
}

fn main() {
    let words: Vec<_> = env::args().skip(1).collect();

    println!("Palindromes?");
    for w in words {
        println!("  {:>5?}: {}", is_palindrome(&w), &w);
    }
}
