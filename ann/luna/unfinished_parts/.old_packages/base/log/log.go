// Package log provides eva log functionality.
package log

import (
	"bufio"
	"fmt"
	"os"
	"runtime"
	"strings"
	"sync"
	"time"
)

type Level int

const (
	TRACE Level = iota
	DEBUG
	INFO
	WARN
	ERROR
	FATAL
)

var (
	logStrings = []string{
		"TRACE ",
		"DEBUG ",
		"INFO  ",
		"WARN  ",
		"ERROR ",
		"FATAL "}
)

const (
	logColorEnabled = true
)

var (
	// See here for color codes.
	//
	// https://docs.microsoft.com/en-us/windows/terminal/images/one-half-dark-color-scheme.png
	logColors = []string{
		"\x1b[94m",
		"\x1b[36m",
		"\x1b[32m",
		"\x1b[33m",
		"\x1b[31m",
		"\x1b[35m",
	}

	logColorReset       = "\x1b[0m"
	logColorForFileLine = "\x1b[1;34m"
)

var (
	// all fields here are guarded by this mu.
	mu sync.Mutex

	logEnabled = true
	logLevel   = TRACE
	w          = bufio.NewWriter(os.Stderr)
)

func (l Level) IsOn() bool {
	mu.Lock()
	defer mu.Unlock()
	return logEnabled && l >= logLevel
}

func (l Level) printf(fmtStr string, args ...interface{}) {
	if l.IsOn() {
		// Stage 1: get file name and line number from runtime.Caller
		//
		// notice that we're using 2, so it will actually log the where
		// the call site happened, 0 = this function,  1 is the public
		// method, we don't want that.
		_, filename, line, ok := runtime.Caller(2)
		if !ok {
			filename = "???"
			line = 1
		} else {
			slash := strings.LastIndex(filename, "/")
			if slash >= 0 {
				filename = filename[slash+1:]
			}
		}

		// Stage 2: Prepare the time line
		timeStr := time.Now().Local().Format("2006-01-02 15:04:05")

		// Stage X: Grab the lock and write log line
		mu.Lock()
		defer mu.Unlock()

		w.WriteString(timeStr)
		w.WriteString(" ")

		if logColorEnabled {
			w.WriteString(logColors[l])
		}
		w.WriteString(logStrings[l])
		if logColorEnabled {
			w.WriteString(logColorReset)
			w.WriteString(logColorForFileLine)
		}
		w.WriteString(filename)
		w.WriteString(fmt.Sprintf(":%d: ", line))
		if logColorEnabled {
			w.WriteString(logColorReset)
		}
		fmt.Fprintf(w, fmtStr, args...)

		fmtLen := len(fmtStr)
		if fmtLen == 0 || fmtStr[fmtLen-1] != '\n' {
			w.WriteString("\n")
		}
		w.Flush()
	}
}

func Tracef(fmt string, args ...interface{}) {
	TRACE.printf(fmt, args...)
}

func Debugf(fmt string, args ...interface{}) {
	DEBUG.printf(fmt, args...)
}

func Infof(fmt string, args ...interface{}) {
	INFO.printf(fmt, args...)
}

func Warnf(fmt string, args ...interface{}) {
	WARN.printf(fmt, args...)
}

func Errorf(fmt string, args ...interface{}) {
	ERROR.printf(fmt, args...)
}

func Fatalf(fmt string, args ...interface{}) {
	FATAL.printf(fmt, args...)
	os.Exit(1)
}
