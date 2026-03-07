package main

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

const targetStr = `# === --- xiejw/dotfiles --------------------------------------------------- ===
export DOTFILES=~/Workspace/y/dotfiles
source ~/Workspace/y/dotfiles/conf/bash_profile
`

func main() {
	home, err := os.UserHomeDir()
	if err != nil {
		fmt.Fprintf(os.Stderr, "error: %v\n", err)
		os.Exit(1)
	}

	fileName := filepath.Join(home, ".profile")

	content, err := os.ReadFile(fileName)
	if err != nil {
		fmt.Fprintf(os.Stderr, "error reading %v: %v\n", fileName, err)
		os.Exit(1)
	}

	if strings.Contains(string(content), targetStr) {
		fmt.Println("We are good for " + fileName)
		return
	}

	f, err := os.OpenFile(fileName, os.O_APPEND|os.O_WRONLY, 0644)
	if err != nil {
		fmt.Fprintf(os.Stderr, "error opening %v: %v\n", fileName, err)
		os.Exit(1)
	}
	defer f.Close()

	_, err = f.WriteString("\n" + targetStr)
	if err != nil {
		fmt.Fprintf(os.Stderr, "error writing %v: %v\n", fileName, err)
		os.Exit(1)
	}

	fmt.Println("Patched " + fileName)
}
