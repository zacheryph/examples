use futures::{Future, Stream};
use h2::server;
use http::{Response, StatusCode};
use tokio::net::TcpListener;

pub fn main() {
    let addr = "127.0.0.1:5928".parse().unwrap();
    let listener = TcpListener::bind(&addr).unwrap();

    // Accept all incoming TCP connections.
    let connection = listener
        .incoming()
        .for_each(move |socket| {
            println!("connection opened: {:?}", socket);
            // Spawn a new task to process each connection.
            tokio::spawn({
                // Start the HTTP/2.0 connection handshake
                server::handshake(socket)
                    .and_then(|h2| {
                        // Accept all inbound HTTP/2.0 streams sent over the
                        // connection.
                        h2.for_each(|(request, mut respond)| {
                            println!("Received request: {:?}", request);
                            println!("Request URI: {:?}", request.uri().path());

                            let stream = request
                                .into_body()
                                .for_each(|msg| {
                                    println!("Msg Received: {:?}", msg);
                                    Ok(())
                                })
                                .and_then(|m| {
                                    println!("stream all finished up: {:?}", m);
                                    Ok(())
                                })
                                .map_err(|e| println!("ERR RecvStream: {:?}", e));

                            tokio::spawn(stream);

                            // Build a response with no body
                            let response =
                                Response::builder().status(StatusCode::OK).body(()).unwrap();

                            // Send the response back to the client
                            respond.send_response(response, true).unwrap();

                            Ok(())
                        })
                    })
                    .map_err(|e| panic!("unexpected error = {:?}", e))
            });

            Ok(())
        })
        .map_err(|e| println!("connection error: {:?}", e));

    tokio::run(connection);
}
