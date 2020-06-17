use std::net::Ipv4Addr;

const BROADCAST: u32 = 0xffffffff;

fn usage() -> ! {
    println!("usage: cidr ip.ad.dr.ess/mask");
    std::process::exit(0);
}

trait IntoIp {
    fn into_ip(&self) -> Ipv4Addr;
}

impl IntoIp for u32 {
    fn into_ip(&self) -> Ipv4Addr {
        let octets = self.to_be_bytes();
        Ipv4Addr::new(octets[0], octets[1], octets[2], octets[3])
    }
}

fn main() {
    let args: Vec<String> = std::env::args().into_iter().collect();
    if args.len() < 2 { usage() }

    let pieces: Vec<&str> = args[1].split('/').collect();
    if pieces.len() != 2 { usage() }

    let ip: Ipv4Addr = pieces[0].parse().unwrap();
    let ip: u32 = u32::from_be_bytes(ip.octets());
    let maskbits: u32 = pieces[1].parse().unwrap();

    let netmask = BROADCAST << (32 - maskbits);
    let network = netmask & ip;
    let broadcast = network | (BROADCAST >> maskbits);

    println!("   cidr: {}/{}", ip.into_ip(), maskbits);
    println!(" config: {} {}", ip.into_ip(), netmask.into_ip());
    println!("network: {} - {}", network.into_ip(), broadcast.into_ip());
    println!(" usable: {}", (BROADCAST >> maskbits) - 1);
}
