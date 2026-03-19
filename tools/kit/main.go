// kit installs macOS developer tools from source without Homebrew.
// All binaries are placed in ~/Workspace/build/usr/bin.
//
// Usage:
//
//	go run .                    # install all missing tools
//	go run . <name>             # install one tool by name
//	go run . <name> <usr-dir>   # install one tool to custom prefix
//
// To add a tool, append to the tools slice in main(). Use GoInstall for Go
// tools (git clone + go build) and CAutoconf for C tools (./configure + make).

package main

import (
	"fmt"
	"log"
	"os"
	"path/filepath"
)

func main() {
	env, err := NewEnv()
	if err != nil {
		log.Fatalf("env: %v", err)
	}

	if len(os.Args) > 2 {
		env.BuildDir = os.Args[2]
		env.UsrDir = filepath.Join(os.Args[2], "usr")
		env.BinDir = filepath.Join(os.Args[2], "usr", "bin")
		env.LibDir = filepath.Join(os.Args[2], "usr", "lib")
	}

	fmt.Printf("build: %s\n", env.BuildDir)
	fmt.Printf("usr:   %s\n", env.UsrDir)
	fmt.Printf("bin:   %s\n", env.BinDir)
	fmt.Printf("lib:   %s\n", env.LibDir)

	if err := os.MkdirAll(env.BinDir, 0755); err != nil {
		log.Fatalf("mkdir bin: %v", err)
	}

	tools := []*Tool{
		{
			Name: "git",
			Bin:  "git",
			Install: CAutoconf(CConfig{
				WorkDir:    "git",
				TarURL:     "https://www.kernel.org/pub/software/scm/git/git-2.51.0.tar.gz",
				ExtractDir: "git-2.51.0",
			}),
		},
		{
			Name: "cmake",
			Bin:  "cmake",
			Install: CAutoconf(CConfig{
				WorkDir:      "cmake",
				TarURL:       "https://github.com/Kitware/CMake/releases/download/v4.1.1/cmake-4.1.1.tar.gz",
				ExtractDir:   "cmake-4.1.1",
				ConfigureCmd: "./bootstrap",
				SingleJob:    true,
			}),
		},
		{
			Name: "ninja",
			Bin:  "ninja",
			Install: CMakeBuild(CMakeBuildConfig{
				WorkDir:    "ninja",
				TarURL:     "https://github.com/ninja-build/ninja/archive/refs/tags/v1.13.1.tar.gz",
				ExtractDir: "ninja-1.13.1",
			}),
		},
		{
			Name: "rsync",
			Bin:  "rsync",
			Install: CAutoconf(CConfig{
				WorkDir:    "rsync",
				TarURL:     "https://download.samba.org/pub/rsync/src/rsync-3.4.1.tar.gz",
				ExtractDir: "rsync-3.4.1",
				ConfigArgs: []string{"--disable-openssl", "--disable-xxhash", "--disable-zstd", "--disable-lz4"},
			}),
		},
		{
			Name: "glow",
			Bin:  "glow",
			Install: GoInstall(GoConfig{
				RepoURL: "https://github.com/charmbracelet/glow.git",
				BinName: "glow",
			}),
		},
		{
			Name: "rust",
			Bin:  "cargo",
			Install: RustInstall(RustConfig{
				Version: "1.91.0",
			}),
		},
		{
			Name: "fd",
			Bin:  "fd",
			Install: RustCargoInstall(RustCargoConfig{
				RepoURL: "https://github.com/sharkdp/fd.git",
				BinName: "fd",
			}),
		},
		{
			Name: "eza",
			Bin:  "eza",
			Install: RustCargoInstall(RustCargoConfig{
				RepoURL: "https://github.com/eza-community/eza.git",
				BinName: "eza",
			}),
		},
		{
			Name: "zoxide",
			Bin:  "zoxide",
			Install: RustCargoInstall(RustCargoConfig{
				RepoURL: "https://github.com/ajeetdsouza/zoxide.git",
				BinName: "zoxide",
			}),
		},
		{
			Name: "ripgrep",
			Bin:  "rg",
			Install: RustCargoInstall(RustCargoConfig{
				RepoURL: "https://github.com/BurntSushi/ripgrep.git",
				BinName: "rg",
				RepoDir: "ripgrep",
			}),
		},
		{
			Name: "delta",
			Bin:  "delta",
			Install: RustCargoInstall(RustCargoConfig{
				RepoURL: "https://github.com/dandavison/delta.git",
				BinName: "delta",
			}),
		},
		{
			Name: "telnet",
			Bin:  "telnet",
			Install: CAutoconf(CConfig{
				WorkDir:    "inetutils",
				TarURL:     "https://ftp.gnu.org/gnu/inetutils/inetutils-2.6.tar.gz",
				ExtractDir: "inetutils-2.6",
				ConfigArgs: []string{"--disable-servers"},
				InstallBin: "telnet/telnet",
			}),
		},
		{
			Name: "jq",
			Bin:  "jq",
			Install: CAutoconf(CConfig{
				WorkDir:    "jq",
				TarURL:     "https://github.com/jqlang/jq/releases/download/jq-1.8.1/jq-1.8.1.tar.gz",
				ExtractDir: "jq-1.8.1",
			}),
		},
		{
			Name: "vim",
			Bin:  "vim",
			Install: CAutoconf(CConfig{
				WorkDir:    "vim",
				TarURL:     "https://github.com/vim/vim/archive/refs/tags/v9.1.1857.tar.gz",
				ExtractDir: "vim-9.1.1857",
				ConfigArgs: []string{"--with-features=huge", "--enable-multibyte", "--without-wayland"},
			}),
		},
		{
			Name: "bash",
			Bin:  "bash",
			Install: CAutoconf(CConfig{
				WorkDir:    "bash",
				TarURL:     "https://ftp.gnu.org/gnu/bash/bash-5.2.tar.gz",
				ExtractDir: "bash-5.2",
			}),
		},
		{
			Name: "pcre2",
			Lib:  "libpcre2-8.a",
			Install: CAutoconf(CConfig{
				WorkDir:    "pcre2",
				TarURL:     "https://github.com/PCRE2Project/pcre2/releases/download/pcre2-10.46/pcre2-10.46.tar.gz",
				ExtractDir: "pcre2-10.46",
			}),
		},
		{
			Name: "libevent",
			Lib:  "libevent.a",
			Install: CAutoconf(CConfig{
				WorkDir:    "libevent",
				TarURL:     "https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz",
				ExtractDir: "libevent-2.1.12-stable",
				ConfigArgs: []string{"--disable-openssl"},
			}),
		},
		{
			Name: "tmux",
			Bin:  "tmux",
			Deps: []string{"libevent"},
			Install: func(env *Env) error {
				return CAutoconf(CConfig{
					WorkDir:    "tmux",
					TarURL:     "https://github.com/tmux/tmux/releases/download/3.5a/tmux-3.5a.tar.gz",
					ExtractDir: "tmux-3.5a",
					ConfigArgs: []string{"--disable-utf8proc"},
					CFLAGS:     "-I" + env.UsrDir + "/include",
					LDFLAGS:    "-L" + env.UsrDir + "/lib -levent -lncurses",
				})(env)
			},
		},
	}

	// Optional: install only the named tool.
	filter := ""
	if len(os.Args) > 1 {
		filter = os.Args[1]
	}

	toolsByName := map[string]*Tool{}
	for _, t := range tools {
		toolsByName[t.Name] = t
	}

	var installTool func(t *Tool) error
	installTool = func(t *Tool) error {
		for _, depName := range t.Deps {
			dep := toolsByName[depName]
			if dep == nil {
				log.Fatalf("unknown dep %q for %s", depName, t.Name)
			}
			if dep.IsInstalled(env) {
				continue
			}
			fmt.Printf("[install] %s (dep of %s)\n", dep.Name, t.Name)
			if err := installTool(dep); err != nil {
				log.Fatalf("[error]   %s: %v", dep.Name, err)
			}
			fmt.Printf("[done]    %s\n", dep.Name)
		}
		return t.Install(env)
	}

	for _, t := range tools {
		if filter != "" && t.Name != filter {
			continue
		}
		if t.IsInstalled(env) {
			dir := env.BinDir
			if t.Lib != "" {
				dir = env.LibDir
			}
			fmt.Printf("[skip]    %s (already in %s)\n", t.Name, dir)
			continue
		}
		fmt.Printf("[install] %s\n", t.Name)
		if err := installTool(t); err != nil {
			log.Fatalf("[error]   %s: %v", t.Name, err)
		}
		fmt.Printf("[done]    %s\n", t.Name)
	}

	fmt.Printf("\n")
	fmt.Printf("=== --- Notes --- ===\n")
	fmt.Printf("export PATH=%s:$PATH\n", env.BinDir)
	fmt.Printf("export LD_LIBRARY_PATH=%s:$LD_LIBRARY_PATH\n", env.LibDir)
}
