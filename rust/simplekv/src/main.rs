use std::{str, sync::Arc};

use axum::{Router, extract::{Path, State}, response::{IntoResponse, Redirect}, http::{StatusCode, Response}, routing::get, body::Body};
use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize)]
struct Entry {
    url: String,
}

impl IntoResponse for Entry {
    fn into_response(self) -> Response<Body> {
        Redirect::temporary(&self.url).into_response()
    }
}

struct AppState {
    links: sled::Tree,
}

#[tokio::main]
async fn main() {
    let db = sled::open("./simplekv.sled").unwrap();
    let links = db.open_tree("links").unwrap();
    let state = Arc::new(AppState { links });

    let router = Router::new()
        .route("/*key", get(redirect_link).post(set_redirect))
        .with_state(state);

    let listener = tokio::net::TcpListener::bind("0.0.0.0:3000").await.unwrap();
    println!("Listening on :3000");
    axum::serve(listener, router).await.unwrap()
}

async fn redirect_link(State(state): State<Arc<AppState>>, Path(key): Path<String>) -> impl IntoResponse {
    if let Some(entry) = state.links.get(key).unwrap() {
        let entry: Entry = bincode::deserialize(&entry).unwrap();
        return entry.into_response();
    }

    (StatusCode::NOT_FOUND, Body::from("Entry Not Found")).into_response()
}

async fn set_redirect(State(state): State<Arc<AppState>>, Path(key): Path<String>, url: String) -> impl IntoResponse {
    let entry = Entry { url };
    let value = bincode::serialize(&entry).unwrap();

    if let Err(err) = state.links.insert(&key, value) {
        return (StatusCode::INTERNAL_SERVER_ERROR, err.to_string()).into_response();
    }

    (StatusCode::OK, "ok").into_response()
}
