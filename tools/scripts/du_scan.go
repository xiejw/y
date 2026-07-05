// du_scan.go - Recursively runs `sudo du -h -d 1` starting at a root dir,
// descending into any subfolder larger than 1 GiB, up to a bounded depth.
// Every folder visited is written to a CSV file.
//
// Usage:
//
//	go run du_scan.go [root_dir]
//
// Output: /tmp/usage.csv with columns: folder,size,is_dir

package main

import (
	"encoding/csv"
	"log"
	"os"
	"os/exec"
	"strconv"
	"strings"
)

const (
	// maxDepth bounds how many times we recurse into subfolders.
	maxDepth = 6

	// thresholdBytes is the size above which a subfolder gets scanned again.
	thresholdBytes = 1 << 30 // 1 GiB

	outputCSVPath = "/tmp/usage.csv"
)

func main() {
	log.SetFlags(log.Ldate | log.Ltime | log.Lmicroseconds | log.Lshortfile)

	root := "."
	if len(os.Args) > 1 {
		root = os.Args[1]
	}

	f, err := os.Create(outputCSVPath)
	if err != nil {
		log.Fatalf("failed to create %s: %v", outputCSVPath, err)
	}
	defer f.Close()

	w := csv.NewWriter(f)
	defer w.Flush()

	if err := w.Write([]string{"folder", "size", "is_dir"}); err != nil {
		log.Fatalf("failed to write header: %v", err)
	}

	scan(root, 0, w)
}

func scan(dir string, depth int, w *csv.Writer) {
	if depth >= maxDepth {
		return
	}

	for _, e := range du(dir) {
		if e.path == dir {
			// The last line of `du -d 1 <dir>` is the total for dir
			// itself, not a child entry.
			continue
		}

		isDir := true
		if info, err := os.Stat(e.path); err == nil {
			isDir = info.IsDir()
		}

		if err := w.Write([]string{e.path, e.size, strconv.FormatBool(isDir)}); err != nil {
			log.Printf("failed to write row for %s: %v", e.path, err)
		}
		w.Flush()

		if isDir && e.bytes > thresholdBytes {
			scan(e.path, depth+1, w)
		}
	}
}

type duEntry struct {
	path  string
	size  string
	bytes int64
}

// du runs `sudo du -h -d 1 <dir>` and parses stdout into entries. Any error
// (permission denied on a child dir, non-zero exit, etc.) is ignored; whatever
// partial stdout du produced is still parsed.
func du(dir string) []duEntry {
	out, _ := exec.Command("sudo", "du", "-h", "-d", "1", dir).Output()

	var entries []duEntry
	for _, line := range strings.Split(string(out), "\n") {
		fields := strings.SplitN(strings.TrimSpace(line), "\t", 2)
		if len(fields) != 2 {
			continue
		}

		size, path := fields[0], fields[1]
		b, err := parseSize(size)
		if err != nil {
			continue
		}

		entries = append(entries, duEntry{path: path, size: size, bytes: b})
	}
	return entries
}

// parseSize parses human-readable `du -h` sizes (e.g. "1.2G", "512K", "0")
// into bytes.
func parseSize(s string) (int64, error) {
	units := map[byte]float64{
		'K': 1 << 10,
		'M': 1 << 20,
		'G': 1 << 30,
		'T': 1 << 40,
		'P': 1 << 50,
	}

	last := s[len(s)-1]
	mult, ok := units[last]
	if !ok {
		return strconv.ParseInt(s, 10, 64)
	}

	num, err := strconv.ParseFloat(s[:len(s)-1], 64)
	if err != nil {
		return 0, err
	}
	return int64(num * mult), nil
}
