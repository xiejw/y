// This is a very simple nginx conf formatter for my own conf files.
//
// It does super simple job to re-indent all lines, collapse empty lines into
// one of none.
//
// Usage:
//
//	go run nginxfmt.go < input_file > output_file
package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func main() {
	scanner := bufio.NewScanner(os.Stdin)

	started := false
	emptyLineCount := 0
	indent := 0

	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())

		isEmpty := len(line) == 0

		// Skip all empty head lines.
		if isEmpty && !started {
			continue
		}

		started = true

		// If empty line, just record and skip it.
		if isEmpty {
			emptyLineCount++
			continue
		}

		if strings.Contains(line, "}") {
			indent -= 4
			if indent < 0 {
				indent = 0
			}
			// Eat all empty lines before '}'.
			emptyLineCount = 0
		} else if emptyLineCount > 0 {
			// Collapse all empty lines.
			fmt.Println()
			emptyLineCount = 0
		}

		fmt.Printf("%s%s\n", strings.Repeat(" ", indent), line)

		if strings.Contains(line, "{") {
			indent += 4
		}
	}
}
