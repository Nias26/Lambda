package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"os"
	"strings"

	"github.com/gorilla/websocket"
)

func main() {
	address := flag.String("address", "localhost", "The address to connect to");
	name := flag.String("name", "Anonymus", "Tne name to show");

	flag.Parse();

	// Connect to the server
	url := fmt.Sprintf("ws://%s/ws", *address);
	conn, _, err := websocket.DefaultDialer.Dial(url, nil)
	if err != nil {
		log.Fatal("Error connecting:", err)
	}
	defer conn.Close()

	// Goroutine to receive messages
	go func() {
		for {
			_, message, err := conn.ReadMessage()
			if err != nil {
				log.Println("Read error:", err)
				return
			}
			str := string(message);
			name, content, found := strings.Cut(str, "\n");
			if found {
				fmt.Printf("\r%s: %s\n> ", name, content);
			}
		}
	}()

	// Main loop to send messages
	scanner := bufio.NewScanner(os.Stdin)
	fmt.Print("> ")
	for scanner.Scan() {
		text := *name + "\n";
		text += scanner.Text()
		err := conn.WriteMessage(websocket.TextMessage, []byte(text))
		if err != nil {
			log.Println("Write error:", err)
			break
		}
		fmt.Print("> ")
	}
}
