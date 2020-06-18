use std::net::Ipv4Addr;

const BROADCAST: u32 = 0xffffffff;

fn usage() -> ! {
    println!("usage: cidr ip.ad.dr.ess/mask");
    std::process::exit(0);
}


fn into_ip(ip: u32) -> Ipv4Addr {
    let octets = ip.to_be_bytes();
    Ipv4Addr::new(octets[0], octets[1], octets[2], octets[3])
}
fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 { usage() }

    let pieces: Vec<&str> = args[1].split('/').collect();
    if pieces.len() != 2 { usage() }

    let ip: Ipv4Addr = pieces[0].parse().unwrap();
    let ip: u32 = u32::from_be_bytes(ip.octets());
    let maskbits: u32 = pieces[1].parse().unwrap();

    let netmask = BROADCAST << (32 - maskbits);
    let network = netmask & ip;
    let broadcast = network | (BROADCAST >> maskbits);

    println!("   cidr: {}/{}", into_ip(ip), maskbits);
    println!("netmask: {}", into_ip(netmask));
    println!("network: {}", into_ip(network));
    println!("  bcast: {}", into_ip(broadcast));
    println!("gateway? {}", into_ip(network + 1));
    println!(" usable: {}", (BROADCAST >> maskbits) - 1);
}
