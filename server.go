package main

import (
	"fmt"
	"log"
	"net/http"
	"sync"

	"github.com/gorilla/websocket"
)

var (
	// Upgrader converts a standard HTTP connection to a WebSocket
	upgrader = websocket.Upgrader{
		CheckOrigin: func(r *http.Request) bool { return true }, // Allow all connections
	}
	clients   = make(map[*websocket.Conn]bool) // Map of connected clients
	clientsMu sync.Mutex                       // Protects the clients map
)

func handleConnections(w http.ResponseWriter, r *http.Request) {
	ws, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Fatal(err)
	}
	defer ws.Close()

	// Register new client
	clientsMu.Lock()
	clients[ws] = true
	clientsMu.Unlock()

	for {
		// Read message from client
		_, msg, err := ws.ReadMessage()
		if err != nil {
			clientsMu.Lock()
			delete(clients, ws)
			clientsMu.Unlock()
			break
		}
		// Broadcast to everyone else
		broadcast(msg, ws)
	}
}

func broadcast(msg []byte, sender *websocket.Conn) {
	clientsMu.Lock()
	defer clientsMu.Unlock()
	for client := range clients {
		if client != sender {
			err := client.WriteMessage(websocket.TextMessage, msg)
			if err != nil {
				client.Close()
				delete(clients, client)
			}
		}
	}
}

func main() {
	http.HandleFunc("/ws", handleConnections)
	fmt.Println("Server started on :9168")
	log.Fatal(http.ListenAndServe("127.0.0.1:9168", nil))
}
