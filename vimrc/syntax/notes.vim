" Vim syntax file
" Language: notes (plain-text notes with markdown-style section headers)
" Maintainer: xiejw.io
"
" This is a special, personal notes format for myself -- not a general
" purpose language. Minimal example:
"
"   # Section
"   ## Sub Section
"
"   Setext Title
"   ------------
"
"   - [TODO] pack for the trip
"   - plain item
"
"   see https://example.com for details
"
"       this is a code block line with 4 spaces indent

if exists("b:current_syntax")
  finish
endif

" === --- ATX Headers --------------------------------------------------- ===
"

" Section headers, e.g. "# Title", "## Title", up to 6 levels deep, same
" convention as markdown ATX headers.
syntax match notesHeaderDelim "^#\{1,6}\ze\s" contained
syntax match notesHeader1 "^#\s.*$" contains=notesHeaderDelim
syntax match notesHeader2 "^##\s.*$" contains=notesHeaderDelim
syntax match notesHeader3 "^###\s.*$" contains=notesHeaderDelim
syntax match notesHeader4 "^####\s.*$" contains=notesHeaderDelim
syntax match notesHeader5 "^#####\s.*$" contains=notesHeaderDelim
syntax match notesHeader6 "^######\s.*$" contains=notesHeaderDelim

" NOTE: headers use explicit colors rather than linking to groups
" like "Title"/"Function"/"Statement" -- colorschemes (e.g. nord) may give
" those the same foreground as Normal text in cterm mode, making headers
" invisible in a terminal.
highlight default link notesHeaderDelim Comment
highlight default notesHeader1 ctermfg=cyan guifg=cyan
highlight default notesHeader2 ctermfg=darkblue guifg=darkblue
highlight default notesHeader3 ctermfg=magenta guifg=purple
highlight default notesHeader4 ctermfg=magenta guifg=purple
highlight default notesHeader5 ctermfg=magenta guifg=purple
highlight default notesHeader6 ctermfg=magenta guifg=purple

" === --- Setext Header ------------------------------------------------- ===
"

" A text line followed immediately by a line of dashes, e.g.:
"   Title
"   ------
" Only the text line is highlighted (same color as notesHeader1); the
" dash line itself is left unstyled.
syntax match notesSetextHeader "^.\+\n\ze-\{3,}\s*$"
highlight default link notesSetextHeader notesHeader1

" === --- List Marker --------------------------------------------------- ===
"

" A line starting with "- ". Only the marker is colored red.
syntax match notesListMarker "^-\s"
highlight default notesListMarker ctermfg=red guifg=red

" === --- List Tag ------------------------------------------------------ ===
"

" List item tag, e.g. "- [XXX] rest of item": the bracketed part right
" after the marker is bolded and green.
syntax match notesListTag "\%(^-\s\)\@<=\[[^]]*\]"
highlight default notesListTag term=bold cterm=bold ctermfg=green gui=bold guifg=green

" === --- URL Link ------------------------------------------------------ ===
"

" http(s) link, colored yellow.
syntax match notesUrl "https\?://\S\+"
highlight default notesUrl ctermfg=yellow guifg=yellow

" === --- Code Block ---------------------------------------------------- ===
"

" A line indented with 4 spaces is a code block line, colored grey.
syntax match notesCodeBlock "^    .*$"
highlight default notesCodeBlock ctermfg=grey guifg=grey

let b:current_syntax = "notes"
