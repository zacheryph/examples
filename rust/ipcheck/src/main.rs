extern crate reqwest;
extern crate serde;
extern crate serde_json;

#[macro_use]
extern crate serde_derive;

#[derive(Serialize, Deserialize, Debug)]
struct IpInfo {
    address: String,
    version: u32,
    #[serde(rename = "errorCode")]
    error_code: u32,
}

fn main() {
    let res = reqwest::get("http://jsonip.io").unwrap();

    if res.status() != reqwest::StatusCode::Ok {
        println!("ERROR: {}", res.status())
    }

    let ip: IpInfo = serde_json::from_reader(res).unwrap();

    println!(
        "Addr:{}, Ver:{}, Err:{}",
        ip.address, ip.version, ip.error_code
    )
}
