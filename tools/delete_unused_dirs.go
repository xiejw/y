package main

import (
	"io/fs"
	"log"
	"os"
	"path/filepath"
	"runtime/debug"
)

var candidateDirs = make(map[string]bool)

func main() {
	log.SetFlags(log.Ldate | log.Ltime | log.Lmicroseconds | log.Lshortfile)

	gw := &GabageWiper{
		RootDir: ".",
	}

	err := gw.Init()
	ValidateError(err)

	err = gw.Work()
	ValidateError(err)
}

type GabageWiper struct {
	RootDir       string
	candidateDirs map[string]bool
}

func (gw *GabageWiper) Init() error {
	gw.candidateDirs = make(map[string]bool)

	// ---------------------------------------------------------------------
	// emit ths dirs for the first round of workOnCandidates
	// ---------------------------------------------------------------------
	walkFunc := func(path string, d fs.DirEntry, err error) error {
		if !d.IsDir() {
			// TODO: Support a.out
			return nil
		}

		if path == ".git" {
			return fs.SkipDir
		}

		candidateDirs[path] = true
		return nil
	}

	return filepath.WalkDir(gw.RootDir, walkFunc)
}

func (gw *GabageWiper) Work() error {
	for {
		workDone := workOnCandidates()
		if workDone {
			break
		}
	}
	return nil
}

func (gw *GabageWiper) workOnCandidates() bool {
	workDone := true
	actions := make([]Action, 0)

	dirs := gw.candidateDirs
	gw.candidateDirs = make(map[string]bool)

	for dirToConsider, _ := range dirs {

		files, err := os.ReadDir(dirToConsider)
                ValidateError(err)

		// Rule 1: Empty Dir
		if len(files) == 0 {
			actions = append(actions, Action{
				kind: DelDirEmpty,
				path: dirToConsider,
			})
			workDone = false
		}

		//if
	}

	for _, act := range actions {
		log.Printf("%10v: %v", act.kind, act.path)
		switch act.kind {
		case DelDirEmpty:
			err := os.Remove(act.path)
                        ValidateError(err)

			nextDirToWork := filepath.Dir(act.path)
			candidateDirs[nextDirToWork] = true
		default:
			panic("unsupported ActionKind")
		}
	}

	return workDone
}

// -----------------------------------------------------------------------------
// action kind ds.
// -----------------------------------------------------------------------------

type ActionKind int

const (
	DelDirEmpty ActionKind = iota
	DelDirAll
)

type Action struct {
	kind ActionKind
	path string
}

func (k ActionKind) String() string {
	switch k {
	case DelDirEmpty:
		return "DelDirEmpty"
	case DelDirAll:
		return "DelDirAll"
	default:
		panic("unsupported ActionKind")
	}
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
