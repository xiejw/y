" === --- Go
"
" - config flp: the list \s*-\s* in comments. The comment leader '//' is not
"   part of it
autocmd FileType go :setlocal tw=0
autocmd FileType go :setlocal shiftwidth=8 softtabstop=8 tabstop=8
autocmd FileType go :setlocal comments=:// formatoptions=tcrqon
autocmd FileType go :setlocal flp=^\\s*-\\s*
" autocmd FileType go :hi goField ctermfg=Cyan

" === --- Text
"
autocmd FileType text :setlocal tw=80 nocindent smartindent

" === --- Markdown
"
" - Config flp: for number list, the regexp is for number 1.\ and bullet -\
autocmd FileType markdown :setlocal tw=80 nocindent
autocmd FileType markdown :setlocal formatoptions+=n flp=^\\s*\\(\\d\\+\\.\\\|-\\)\\s

" === --- Makefile
"
autocmd FileType make :setlocal tw=80 nocindent
autocmd FileType make :setlocal shiftwidth=8 tabstop=8 softtabstop=8

" === -- Tex
"
" Customize the listing formatting ^\*\s according OPmac. So ::help fo-table
" autocmd FileType plaintex :setlocal formatoptions+=n flp=^\\*\\s
autocmd BufRead,BufNewFile *.tex setlocal filetype=tex
autocmd FileType plaintex :setlocal tw=80 nocindent
autocmd FileType plaintex :setlocal autoindent
autocmd FileType plaintex :setlocal formatoptions=tcq2

" === --- Bash
"
" Customize the highlighting for function and its name. It was green, similar
" to the comment's color.
autocmd FileType bash :hi shFunctionKey ctermfg=Cyan
autocmd FileType bash :hi shFunction ctermfg=DarkBlue

" === --- HTML
"
" To trigger abbrev without inserting space, use ctrl-] (see :help i_CTRL-]).
"
" aa stops at the href part
" cc stops in the block
" pp opens a new line and stops there
autocmd FileType html :iabbrev <buffer> aa <a href="" target="_blank"></a><esc>bbbbbblli
autocmd FileType html :iabbrev <buffer> cc <code></code><esc>bbli
autocmd FileType html :iabbrev <buffer> pp <p></p><esc>bbli<enter><esc>ko

" Make formatting ignore html tags (e.g., <a...>)
autocmd FileType html :set indentexpr=|set nocindent

