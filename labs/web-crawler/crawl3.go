// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 241.

// Crawl2 crawls web links starting with the command-line arguments.
//
// This version uses a buffered channel as a counting semaphore
// to limit the number of concurrent calls to links.Extract.
//
// Crawl3 adds support for depth limiting.
// Luis Wilson A00226649
package main

import (
	"flag"
	"fmt"
	"log"
	"os"

	"gopl.io/ch5/links"
)

//Luis Wilson A00226649

type linkAndDepth struct {
	links []string
	depth int
}

//!+sema
// tokens is a counting semaphore used to
// enforce a limit of 20 concurrent requests.
var tokens = make(chan struct{}, 20)

func crawl(url string, currentDepth int) []string {
	fmt.Println(url)
	if currentDepth == 0 {
		empty := make([]string, 0)
		return empty
	}
	tokens <- struct{}{} // acquire a token
	list, err := links.Extract(url)
	<-tokens // release the token

	if err != nil {
		log.Print(err)
	}
	return list
}

//!-sema

//!+
func main() {

	worklist := make(chan linkAndDepth)
	var n int // number of pending sends to worklist

	// Start with the command-line arguments.
	n++
	go func() {
		depthPtr := flag.Int("depth", -1, "max depth for web crawler")
		flag.Parse()

		if *depthPtr < 0 {
			fmt.Println("Bad arguments. Usage: go run crawl3.go -depth=[n>=0] [url]")
		}
		links := os.Args[2:]
		together := linkAndDepth{links, *depthPtr}
		worklist <- together

	}()

	// Crawl the web concurrently.
	seen := make(map[string]bool)
	for ; n > 0; n-- {
		list := <-worklist
		for _, link := range list.links {
			if !seen[link] && list.depth > -1 {
				seen[link] = true
				n++
				go func(link string) {
					newLinks := crawl(link, list.depth)
					newDepth := list.depth - 1
					newWork := linkAndDepth{newLinks, newDepth}
					worklist <- newWork
				}(link)
			}
		}

	}
}

//!-
