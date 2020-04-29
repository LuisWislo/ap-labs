// Clock2 is a concurrent TCP server that periodically writes the time.
package main

import (
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"time"
)

//Luis Wilson A00226649

var timeZone *time.Location

func handleConn(c net.Conn) {
	defer c.Close()
	for {

		_, err := io.WriteString(c, time.Now().In(timeZone).Format("15:04:05\n"))
		if err != nil {
			return // e.g., client disconnected
		}
		time.Sleep(1 * time.Second)
	}
}

func main() {
	ports := os.Args[1:]

	location, err := time.LoadLocation(os.Getenv("TZ"))

	if err != nil {
		log.Fatal(err)
	}

	timeZone = location

	if ports[0] == "-port" {
		address := "localhost:" + ports[1]
		listener, err := net.Listen("tcp", address)
		if err != nil {
			log.Fatal(err)
		}
		for {
			conn, err := listener.Accept()
			if err != nil {
				log.Print(err) // e.g., connection aborted
				continue
			}
			go handleConn(conn) // handle connections concurrently
		}

	} else {
		fmt.Println("Invalid arguments. Usage: go run clock2.go -port <port_number>")
	}

}
