// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"fmt"
	"io"
	"log"
	"net"
	"os"
)

//Luis Wilson A00226649

var clientUsage string = "\nUsage: go run client.go -user <username> -server <address>"

//!+
func main() {

	configuration := os.Args[1:]

	if len(configuration) != 4 {
		fmt.Println("Invalid number of arguments.", clientUsage)
	} else if configuration[0] != "-user" || configuration[2] != "-server" {
		fmt.Println("Unsupported flags.", clientUsage)
	} else {

		conn, err := net.Dial("tcp", configuration[3])
		if err != nil {
			log.Fatal(err)
		}

		io.WriteString(conn, configuration[1])

		done := make(chan struct{})
		go func() {
			io.Copy(os.Stdout, conn) // NOTE: ignoring errors
			log.Println("done")
			os.Exit(0)
			done <- struct{}{} // signal the main goroutine
		}()
		mustCopy(conn, os.Stdin)
		conn.Close()
		<-done // wait for background goroutine to finish
	}
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
