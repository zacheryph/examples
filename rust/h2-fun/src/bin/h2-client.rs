use futures::*;
use h2::client;
use http::*;
use tokio::net::TcpStream;

pub fn main() {
    let addr = "127.0.0.1:5928".parse().unwrap();

    let connection = TcpStream::connect(&addr)
        .map_err(|_| panic!("failed to establish TCP connection"))
        .and_then(|tcp| client::handshake(tcp))
        .and_then(|(h2, connection)| {
            let connection = connection.map_err(|_| panic!("HTTP/2.0 connection failed"));

            // Spawn a new task to drive the connection state
            tokio::spawn(connection);

            // Wait until the `SendRequest` handle has available
            // capacity.
            h2.ready()
        })
        .and_then(|mut h2| {
            // Prepare the HTTP request to send to the server.
            let request = Request::builder()
                .method(Method::GET)
                .uri("https://_/users/add")
                .body(())
                .unwrap();

            // Send the request. The second tuple item allows the caller
            // to stream a request body.
            // let (response, _) = h2.send_request(request, true).unwrap();
            let (response, mut stream) = h2.send_request(request, false).unwrap();
            stream
                .send_data("some important information is going here".into(), false)
                .unwrap();
            stream.send_data("a".into(), false).unwrap();
            stream.send_data("b".into(), false).unwrap();
            stream
                .send_data("we want to see what is going on".into(), false)
                .unwrap();
            stream.send_trailers(HeaderMap::new()).unwrap();

            response.and_then(|response| {
                let (head, mut body) = response.into_parts();

                println!("Received response: {:?}", head);

                // The `release_capacity` handle allows the caller to manage
                // flow control.
                //
                // Whenever data is received, the caller is responsible for
                // releasing capacity back to the server once it has freed
                // the data from memory.
                let mut release_capacity = body.release_capacity().clone();

                body.for_each(move |chunk| {
                    println!("RX: {:?}", chunk);

                    // Let the server send more data.
                    let _ = release_capacity.release_capacity(chunk.len());

                    Ok(())
                })
            })
        })
        .map_err(|e| println!("h2 error? {:?}", e));

    tokio::run(connection);
}
