// This script takes a file name called File and multi-line string from stdio.
// It checks whether the file

package main

import (
	"io"
	"log"
	"os"
	"strings"
)

func main() {
	// TODO check arg length.
	// TODO check file exist.
	dataRaw, err := os.ReadFile(os.Args[1])
	if err != nil {
		log.Fatal(err)
	}
	data := string(dataRaw)
	log.Printf("%v", data)

	stdin, err := io.ReadAll(os.Stdin)
	if err != nil {
		panic(err)
	}
	str := string(stdin)
	log.Printf("%#v", str)

	// TODO quit if contains
	contains := strings.Contains(data, str)
	log.Printf("found: %v\n", contains)

	// TODO append to the file.
}
