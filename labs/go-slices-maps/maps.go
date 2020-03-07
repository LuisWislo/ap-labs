package main

import (
	"golang.org/x/tour/wc"
	"strings"
)

func WordCount(s string) map[string]int {
	output := make(map[string] int)
	words := strings.Fields(s)
	
	for _, w := range words {
		_,exists := output[w] 
		
		if exists {
			output[w] = output[w] +1
		} else {
			output[w] = 1
		}
		
	}
		
	return output
}

func main() {
	wc.Test(WordCount)
}

