// [Mortgage] Loan calculator / amortization table

#[macro_use]
extern crate structopt;

extern crate loan;

use structopt::StructOpt;

use loan::{print_first, print_table, print_totals, Loan};

#[derive(Debug, StructOpt)]
enum Detail {
    #[structopt(name = "table", about = "show amortization table")]
    Table,

    #[structopt(name = "total", about = "show final loan numbers")]
    Total,
}

#[derive(Debug, StructOpt)]
#[structopt(name = "loan", about = "simple loan calculator")]
struct Cmd {
    #[structopt(help = "loan amount")]
    amount: f64,

    #[structopt(short = "r", long = "rate", help = "interest rate")]
    rate: Option<f64>,

    #[structopt(short = "m", long = "months", help = "length of loan in months")]
    months: Option<u32>,

    #[structopt(short = "y", long = "years", help = "length of the loan in years")]
    years: Option<u32>,

    #[structopt(short = "p", long = "payment",
                help = "override calculated payment (to show quicker payoff)")]
    payment: Option<f64>,

    #[structopt(subcommand)]
    detail: Option<Detail>,
}

fn main() {
    let opt = Cmd::from_args();

    let months: u32 = match (opt.months, opt.years) {
        (None, Some(y)) => y * 12,
        (Some(m), None) => m,
        (None, None) => {
            println!("** defaulting to 30 year loan");
            30 * 12
        }
        (_, _) => {
            eprintln!("Cannot define months and years for loan length");
            std::process::exit(1);
        }
    };

    let rate: f64 = match opt.rate {
        Some(r) => if r >= 0.2 {
            r / 100.0
        } else {
            r
        },
        None => 0.04,
    };

    let mut loan = Loan::new(opt.amount, rate, months);
    if let Some(p) = opt.payment {
        loan.set_payment(p);
    }

    println!();

    match opt.detail {
        Some(Detail::Table) => print_table(loan),
        Some(Detail::Total) => print_totals(loan),
        None => print_first(loan),
    }

    // for p in loan {
    //     println!(
    //         "Pay:{:.2} Int:{:.2} Pri:{:.2} Left:{:.2}",
    //         p.amount, p.interest, p.principal, p.balance
    //     );
    // }
}
