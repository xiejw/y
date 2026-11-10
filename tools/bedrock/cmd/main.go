// bedrock installs macOS developer tools from source without Homebrew.
// All binaries are placed in ~/Workspace/build/usr/bin.
//
// Usage:
//
//	go run ./cmd                    # install all missing tools
//	go run ./cmd <name>             # install one tool by name
//	go run ./cmd <name> <build-dir> # install one tool to custom prefix
//
// Flags (must precede positional args):
//
//	--keep_compiled_assets  keep tarballs, sources and build outputs under
//	                        <build-dir>/<tool>/ (default: delete after install)
//
// To add a tool, append to the tools slice in main(). Installer constructors
// (CAutoconf, CMakeBuild, BinaryDownload, GoInstall) live in bedrock/src.

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"

	bedrock "bedrock/src"
)

func main() {
	keepAssets := flag.Bool("keep_compiled_assets", false,
		"keep downloaded tarballs, extracted sources and build outputs after install")
	flag.Parse()
	args := flag.Args()

	env, err := bedrock.NewEnv()
	if err != nil {
		log.Fatalf("env: %v", err)
	}
	env.KeepCompiledAssets = *keepAssets

	if len(args) > 1 {
		env.BuildDir = args[1]
		env.UsrDir = filepath.Join(args[1], "usr")
		env.BinDir = filepath.Join(args[1], "usr", "bin")
		env.LibDir = filepath.Join(args[1], "usr", "lib")
	}

	fmt.Printf("build: %s\n", env.BuildDir)
	fmt.Printf("usr:   %s\n", env.UsrDir)
	fmt.Printf("bin:   %s\n", env.BinDir)
	fmt.Printf("lib:   %s\n", env.LibDir)

	if err := os.MkdirAll(env.BinDir, 0755); err != nil {
		log.Fatalf("mkdir bin: %v", err)
	}

	tools := []*bedrock.Tool{
		{
			Name: "git",
			Bin:  "git",
			Install: bedrock.CAutoconf(bedrock.CConfig{
				WorkDir:    "git",
				TarURL:     "https://www.kernel.org/pub/software/scm/git/git-2.51.0.tar.gz",
				ExtractDir: "git-2.51.0",
			}),
		},
		{
			Name: "cmake",
			Bin:  "cmake",
			Install: bedrock.CAutoconf(bedrock.CConfig{
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
			Install: bedrock.CMakeBuild(bedrock.CMakeBuildConfig{
				WorkDir:    "ninja",
				TarURL:     "https://github.com/ninja-build/ninja/archive/refs/tags/v1.13.1.tar.gz",
				ExtractDir: "ninja-1.13.1",
			}),
		},
		{
			Name: "rsync",
			Bin:  "rsync",
			Install: bedrock.CAutoconf(bedrock.CConfig{
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
			Install: bedrock.BinaryDownload(bedrock.BinaryDownloadConfig{
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
			Install: bedrock.BinaryDownload(bedrock.BinaryDownloadConfig{
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
			Install: bedrock.BinaryDownload(bedrock.BinaryDownloadConfig{
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
			Install: bedrock.BinaryDownload(bedrock.BinaryDownloadConfig{
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
			Install: bedrock.CAutoconf(bedrock.CConfig{
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
			Install: bedrock.CAutoconf(bedrock.CConfig{
				WorkDir:    "vim",
				TarURL:     "https://github.com/vim/vim/archive/refs/tags/v9.1.1857.tar.gz",
				ExtractDir: "vim-9.1.1857",
				ConfigArgs: []string{"--with-features=huge", "--enable-multibyte", "--without-wayland"},
			}),
		},
		{
			Name: "bash",
			Bin:  "bash",
			Install: bedrock.CAutoconf(bedrock.CConfig{
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
			Install: bedrock.CAutoconf(bedrock.CConfig{
				WorkDir:    "pcre2",
				TarURL:     "https://github.com/PCRE2Project/pcre2/releases/download/pcre2-10.46/pcre2-10.46.tar.gz",
				ExtractDir: "pcre2-10.46",
			}),
		},
		{
			Name: "libevent",
			Lib:  "libevent.a",
			Install: bedrock.CAutoconf(bedrock.CConfig{
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
			Install: func(env *bedrock.Env) error {
				return bedrock.CAutoconf(bedrock.CConfig{
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
	if len(args) > 0 {
		filter = args[0]
	}

	bedrock.InstallToolsWithFilter(env, tools, filter)
}
