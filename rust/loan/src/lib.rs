#[derive(Debug)]
pub struct Loan {
    pub amount: f64,
    pub periods: u32,
    pub period_rate: f64,
    pub payment: f64,
}

#[derive(Debug)]
pub struct Payment {
    pub amount: f64,
    pub interest: f64,
    pub principal: f64,
    pub balance: f64,
}

impl Loan {
    pub fn new(amount: f64, rate: f64, periods: u32) -> Loan {
        let fixed_rate = if rate >= 0.2 { rate / 100.0 } else { rate };
        let period_rate = fixed_rate / 12.0;

        Loan {
            amount,
            periods,
            period_rate,
            payment: (amount / Loan::discount(rate, periods) * 100.0).round() / 100.0,
        }
    }

    pub fn set_payment(&mut self, payment: f64) {
        self.payment = payment;
    }

    /// Discount Factor
    fn discount(apr: f64, periods: u32) -> f64 {
        let period_rate = apr / 12.0;
        let daily = (period_rate + 1.0).powi(periods as i32);
        (daily - 1.0) / (period_rate * daily)
    }
}

impl Iterator for Loan {
    type Item = Payment;

    fn next(&mut self) -> Option<Self::Item> {
        if self.amount <= 0.0 {
            return None;
        }

        let interest = (self.amount * self.period_rate * 100.0).round() / 100.0;
        let amount = if self.amount > self.payment {
            self.payment
        } else {
            self.amount + interest
        };

        let p = Payment {
            amount,
            interest,
            principal: amount - interest,
            balance: self.amount - amount + interest,
        };
        self.amount -= p.principal;

        Some(p)
    }
}

/// Just prints the initial payment information
pub fn print_first(mut loan: Loan) {
    if let Some(p) = loan.next() {
        println!("Initial Loan Payment:");
        println!("     Interest: ${:.2}", p.interest);
        println!("    Principal: ${:.2}", p.principal);
        println!("Total Payment: ${:.2}", p.amount);
    }
}

/// Prints full amortization table
pub fn print_table(loan: Loan) {
    println!(
        "{:<7} {:>12} {:>12} {:>12} {:>12}",
        "Payment", "Total", "Interest", "Principal", "Balance"
    );

    for (i, p) in loan.enumerate() {
        println!(
            "{:<7} {:>12.2} {:>12.2} {:>12.2} {:>12.2}",
            i + 1,
            p.amount,
            p.interest,
            p.principal,
            p.balance
        );
    }
}

/// Prints totals of the loan
pub fn print_totals(loan: Loan) {
    let mut total_interest: f64 = 0.0;
    let mut total_principal: f64 = 0.0;

    for p in loan {
        total_interest += p.interest;
        total_principal += p.principal;
    }

    println!("Total Interest Payed: ${:.2}", total_interest);
    println!(
        "Total Amount Payed:   ${:.2}",
        total_interest + total_principal
    );
}
