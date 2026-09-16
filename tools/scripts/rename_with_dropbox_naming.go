// rename_with_dropbox_naming.go - Renames video files to use Dropbox's
// camera-upload naming convention: a "YYYY-MM-DD HH.MM.SS" timestamp prefix
// followed by the original file name.
//
// Usage:
//
//	go run rename_with_dropbox_naming.go [flags]
//
// Flags:
//
//	-dir string    Directory to scan (default ".").
//	-dry_run bool  If true, only print the renames that would happen.
//
// For each file matching one of the tracked extensions, if the file name
// does not already start with a "YYYY-MM-DD HH.MM.SS" prefix, the file is
// renamed to "<prefix> <old_name>" where <prefix> is derived from the
// file's creation time.
//
// NOTE: Reading file creation time relies on syscall.Stat_t.Birthtimespec,
// which is only available on macOS (Darwin).

package main

import (
	"flag"
	"log"
	"os"
	"path/filepath"
	"regexp"
	"runtime/debug"
	"strings"
	"syscall"
	"time"
)

var (
	flagDir    = flag.String("dir", ".", "directory to scan for files to rename.")
	flagDryRun = flag.Bool("dry_run", false, "if true, only print the renames without applying them.")
)

// trackedExtensions lists the (lower-case, without dot) file extensions this
// script scans for.
var trackedExtensions = []string{"mp4"}

var prefixPattern = regexp.MustCompile(`^\d{4}-\d{2}-\d{2} \d{2}\.\d{2}\.\d{2}`)

const prefixTimeLayout = "2006-01-02 15.04.05"

func main() {
	flag.Parse()
	log.SetFlags(log.Ldate | log.Ltime | log.Lmicroseconds | log.Lshortfile)

	entries, err := os.ReadDir(*flagDir)
	ValidateError(err)

	for _, entry := range entries {
		if entry.IsDir() {
			continue
		}

		name := entry.Name()
		if !hasTrackedExtension(name) {
			continue
		}

		if prefixPattern.MatchString(name) {
			continue
		}

		path := filepath.Join(*flagDir, name)
		newName := createdTime(path).Format(prefixTimeLayout) + " " + name
		newPath := filepath.Join(*flagDir, newName)

		log.Printf("rename: %q -> %q", name, newName)
		if *flagDryRun {
			continue
		}

		err := os.Rename(path, newPath)
		ValidateError(err)
	}
}

func hasTrackedExtension(name string) bool {
	ext := strings.TrimPrefix(strings.ToLower(filepath.Ext(name)), ".")
	for _, tracked := range trackedExtensions {
		if ext == tracked {
			return true
		}
	}
	return false
}

func createdTime(path string) time.Time {
	info, err := os.Stat(path)
	ValidateError(err)

	stat := info.Sys().(*syscall.Stat_t)
	return time.Unix(stat.Birthtimespec.Sec, stat.Birthtimespec.Nsec)
}

// -----------------------------------------------------------------------------
// helper
// -----------------------------------------------------------------------------
func ValidateError(err error) {
	if err != nil {
		debug.PrintStack()
		log.Fatalf("unexpected err: %v", err)
	}
}
