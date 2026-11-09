// kit installs macOS developer tools from source without Homebrew.
// All binaries are placed in ~/Workspace/build/usr/bin.
//
// Usage:
//
//	go run ./cmd                    # install all missing tools
//	go run ./cmd <name>             # install one tool by name
//	go run ./cmd <name> <build-dir> # install one tool to custom prefix
//
// To add a tool, append to the tools slice in main(). Installer constructors
// (CAutoconf, CMakeBuild, BinaryDownload, GoInstall) live in kit/src.

package main

import (
	"fmt"
	"log"
	"os"
	"path/filepath"

	kit "kit/src"
)

func main() {
	env, err := kit.NewEnv()
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

	tools := []*kit.Tool{
		{
			Name: "git",
			Bin:  "git",
			Install: kit.CAutoconf(kit.CConfig{
				WorkDir:    "git",
				TarURL:     "https://www.kernel.org/pub/software/scm/git/git-2.51.0.tar.gz",
				ExtractDir: "git-2.51.0",
			}),
		},
		{
			Name: "cmake",
			Bin:  "cmake",
			Install: kit.CAutoconf(kit.CConfig{
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
			Install: kit.CMakeBuild(kit.CMakeBuildConfig{
				WorkDir:    "ninja",
				TarURL:     "https://github.com/ninja-build/ninja/archive/refs/tags/v1.13.1.tar.gz",
				ExtractDir: "ninja-1.13.1",
			}),
		},
		{
			Name: "rsync",
			Bin:  "rsync",
			Install: kit.CAutoconf(kit.CConfig{
				WorkDir:    "rsync",
				TarURL:     "https://download.samba.org/pub/rsync/src/rsync-3.4.1.tar.gz",
				ExtractDir: "rsync-3.4.1",
				ConfigArgs: []string{"--disable-openssl", "--disable-xxhash", "--disable-zstd", "--disable-lz4"},
			}),
		},
		{
			// Prebuilt; source: https://github.com/sharkdp/fd
			Name: "fd",
			Bin:  "fd",
			Install: kit.BinaryDownload(kit.BinaryDownloadConfig{
				BinName: "fd",
				URLs: map[string]string{
					"arm64-darwin": "https://github.com/xiejw/y/releases/download/2026-11-01/fd_arm64_darwin",
					"amd64-linux":  "https://github.com/xiejw/y/releases/download/2026-11-09-linux/fd_amd64_linux",
				},
			}),
		},
		{
			// Prebuilt; source: https://github.com/eza-community/eza
			Name: "eza",
			Bin:  "eza",
			Install: kit.BinaryDownload(kit.BinaryDownloadConfig{
				BinName: "eza",
				URLs: map[string]string{
					"arm64-darwin": "https://github.com/xiejw/y/releases/download/2026-11-01/eaz_arm64_darwin",
					"amd64-linux":  "https://github.com/xiejw/y/releases/download/2026-11-09-linux/eza_amd64_linux",
				},
			}),
		},
		{
			// Prebuilt; source: https://github.com/ajeetdsouza/zoxide
			Name: "zoxide",
			Bin:  "zoxide",
			Install: kit.BinaryDownload(kit.BinaryDownloadConfig{
				BinName: "zoxide",
				URLs: map[string]string{
					"arm64-darwin": "https://github.com/xiejw/y/releases/download/2026-11-01/zoxide_arm64_darwin",
					"amd64-linux":  "https://github.com/xiejw/y/releases/download/2026-11-09-linux/zoxide_amd64_linux",
				},
			}),
		},
		{
			// Prebuilt; source: https://github.com/BurntSushi/ripgrep
			Name: "ripgrep",
			Bin:  "rg",
			Install: kit.BinaryDownload(kit.BinaryDownloadConfig{
				BinName: "rg",
				URLs: map[string]string{
					"arm64-darwin": "https://github.com/xiejw/y/releases/download/2026-11-01/rg_arm64_darwin",
					"amd64-linux":  "https://github.com/xiejw/y/releases/download/2026-11-09-linux/rg_amd64_linux",
				},
			}),
		},
		{
			Name: "telnet",
			Bin:  "telnet",
			Install: kit.CAutoconf(kit.CConfig{
				WorkDir:    "inetutils",
				TarURL:     "https://ftp.gnu.org/gnu/inetutils/inetutils-2.6.tar.gz",
				ExtractDir: "inetutils-2.6",
				ConfigArgs: []string{"--disable-servers"},
				InstallBin: "telnet/telnet",
			}),
		},
		{
			Name: "vim",
			Bin:  "vim",
			Install: kit.CAutoconf(kit.CConfig{
				WorkDir:    "vim",
				TarURL:     "https://github.com/vim/vim/archive/refs/tags/v9.1.1857.tar.gz",
				ExtractDir: "vim-9.1.1857",
				ConfigArgs: []string{"--with-features=huge", "--enable-multibyte", "--without-wayland"},
			}),
		},
		{
			Name: "bash",
			Bin:  "bash",
			Install: kit.CAutoconf(kit.CConfig{
				WorkDir:    "bash",
				TarURL:     "https://ftp.gnu.org/gnu/bash/bash-5.2.tar.gz",
				ExtractDir: "bash-5.2",
				// No system gettext on macOS; the bundled lib/intl races
				// under make -j (libintl.h not yet generated).
				ConfigArgs: []string{"--disable-nls"},
			}),
		},
		{
			Name: "pcre2",
			Lib:  "libpcre2-8.a",
			Install: kit.CAutoconf(kit.CConfig{
				WorkDir:    "pcre2",
				TarURL:     "https://github.com/PCRE2Project/pcre2/releases/download/pcre2-10.46/pcre2-10.46.tar.gz",
				ExtractDir: "pcre2-10.46",
			}),
		},
		{
			Name: "libevent",
			Lib:  "libevent.a",
			Install: kit.CAutoconf(kit.CConfig{
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
			Install: func(env *kit.Env) error {
				return kit.CAutoconf(kit.CConfig{
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

	kit.InstallToolsWithFilter(env, tools, filter)
}
