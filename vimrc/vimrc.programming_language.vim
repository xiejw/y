" {{{2 go
" ------------------------------------------------------------------------------
" --- go
"
" - config flp: the list \s*-\s* in comments. The comment leader '//' is not
"   part of it
autocmd FileType go :setlocal tw=0
autocmd FileType go :setlocal shiftwidth=8 softtabstop=8 tabstop=8
autocmd FileType go :setlocal comments=:// formatoptions=tcrqon
autocmd FileType go :setlocal flp=^\\s*-\\s*
" autocmd FileType go :hi goField ctermfg=Cyan

" {{{2 cc
" ------------------------------------------------------------------------------
" --- cc
"
autocmd FileType cpp :setlocal shiftwidth=2 tabstop=2 softtabstop=2

function! GoogleCC()
  :set shiftwidth=2 tabstop=2 softtabstop=2
endfunction

" - added more int types and error_t as cType
autocmd FileType cpp :syn keyword	cType	i64_t u64_t i32_t u32_t f32_t f64_t
autocmd FileType cpp :syn keyword	cType	vec_t error_t sds_t

" - highlight field (-> or .) for c and adjust the colors for it and String.
autocmd FileType cpp :syn match cField /\(->\|\.\)\<\k\+\>/
autocmd FileType cpp :hi cField ctermfg=Green
autocmd FileType cpp :hi String ctermfg=Red

" - highlight function for c, check \ze in pattern
autocmd FileType cpp :syn match cFunction "\<\k\+\ze("
autocmd FileType cpp :hi link cFunction Function



" - highlight the entire type with type name for struct/union/enum (instead of
"   just the keywords)
autocmd FileType cpp :syn clear cStructure
autocmd FileType cpp :syn match cStructureType /\(struct\|union\|enum\) \<\k\+\>/
autocmd FileType cpp :hi cStructureType ctermfg=Yellow

" - also highlight namespace prefix
autocmd FileType cpp :syn match cStructureType /\<\k\+\>::/

" {{{2 rust
" ------------------------------------------------------------------------------
" --- rust
"
autocmd FileType rust :setlocal textwidth=80
autocmd FileType rust :setlocal nofoldenable foldmethod=syntax foldcolumn=1 foldlevel=0
autocmd FileType rust :unmap <space>

" {{{2 java
" ------------------------------------------------------------------------------
" --- java
"
" - put record (since java 14) as similar syntax as `class`
" - put sealed as similar syntax as `implements`
"
autocmd FileType java :setlocal tw=80 shiftwidth=4
autocmd FileType java :syntax keyword javaStructure sealed
autocmd FileType java :hi Constant ctermfg=DarkGreen

" {{{2 scala
" ------------------------------------------------------------------------------
" --- scala
"
autocmd FileType scala :setlocal tw=100 shiftwidth=2

" {{{2 text
" ------------------------------------------------------------------------------
" --- text
"
autocmd FileType text :setlocal tw=80 nocindent smartindent

" {{{2 markdown
" ------------------------------------------------------------------------------
" --- markdown
"
" - config flp: for number list, the regexp is for number 1.\ and bullet -\
autocmd FileType markdown :setlocal tw=80 nocindent
" autocmd FileType markdown :setlocal foldlevel=2
" autocmd FileType markdown :setlocal foldmethod=expr foldexpr=fold#GetMarkdownFold(v:lnum)
" autocmd FileType markdown :setlocal autoindent
autocmd FileType markdown :setlocal formatoptions+=n flp=^\\s*\\(\\d\\+\\.\\\|-\\)\\s

" {{{2 makefile
" ------------------------------------------------------------------------------
" --- makefile
"
autocmd FileType make :setlocal tw=80 nocindent
autocmd FileType make :setlocal shiftwidth=8 tabstop=8 softtabstop=8

" {{{2 help
" ------------------------------------------------------------------------------
" --- help
"
" - make Ignore highlight visible
" - disable hidding text with "conceal" syntax attr. For example, *tag* will
"   have the star showing instead of hiding.
" - make 'pattern', `pattern` and {pattern} more powerful than default configuration.
" - disable comments to avoid noise
" - set formatoptions as tcq2 to make the column format easier.
autocmd FileType help :setlocal conceallevel=0
" '...'
autocmd FileType help :syn match helpOption  "'[-a-zA-Z0-9\_\."\(\) |{}&*+/:%#=[\]<.,]\+'"
" {...}
autocmd FileType help :syn match helpSpecial "{[-a-zA-Z0-9\_\.'"\(\) *+/:%#=[\]<.,]\+}"
" `...`
autocmd FileType help :syn match helpCommand "`[^`]\+`"
autocmd FileType help :hi Ignore ctermfg=DarkGreen
autocmd FileType help :hi helpSpecial ctermfg=DarkMagenta
autocmd FileType help :setlocal comments=
autocmd FileType help :setlocal formatoptions=tcq
autocmd FileType help :setlocal autoindent nocindent
autocmd FileType help :setlocal formatoptions+=n flp=^\\s*\\(\\d\\+\\.\\\|-\\)\\s
autocmd FileType help :setlocal shiftwidth=4

" added hypen (ascii 45) to keyword, so tag jumping is easier
" read :syn-iskeyword for how iskeyword is set.
autocmd FileType help :setlocal iskeyword+=45

" {{{2 python
" ------------------------------------------------------------------------------
" --- python
"
autocmd FileType python :setlocal tw=80
autocmd FileType python :setlocal shiftwidth=4
autocmd FileType python :setlocal formatoptions=tcqo

" {{{2 tex/latex
" ------------------------------------------------------------------------------
" --- tex
"
" customize the listing formatting ^\*\s according OPmac. So ::help fo-table
" autocmd FileType plaintex :setlocal formatoptions+=n flp=^\\*\\s
autocmd BufRead,BufNewFile *.tex setlocal filetype=tex
autocmd FileType plaintex :setlocal tw=80 nocindent
autocmd FileType plaintex :setlocal autoindent
autocmd FileType plaintex :setlocal formatoptions=tcq2

" === --- bash ------------------------------------------------------------- ===
"
" customize the highlighting for function and its name. It was green, similar
" to the comment's color.
autocmd FileType bash :hi shFunctionKey ctermfg=Cyan
autocmd FileType bash :hi shFunction ctermfg=DarkBlue

" === --- html ------------------------------------------------------------- ===
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

