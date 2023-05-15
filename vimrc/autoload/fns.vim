" =============================================================================
" define a func: delete tailing white space.
"
" this can be used to delete tailing spaces for all lines via autocmd, e.g.,
"
"     autocmd BufWritePre * :call fns#DelEmptyLinesEnd()
function! fns#DelEmptyLinesEnd()
  let l = line(".")
  let c = col(".")
  :%s/\s\+$//e
  call cursor(l, c)
endfunction

" =============================================================================
" define a Func: showing the syn group of the char under cursor.
function! fns#SynGroup()
    let l:s = synID(line('.'), col('.'), 1)
    echo synIDattr(l:s, 'name') . ' -> ' . synIDattr(synIDtrans(l:s), 'name')
endfun

" =============================================================================
" Shows a list to select for all pending files in current repo, and then opens it.
"
"   - use $FZF_PENDING_FILE_COMMAND env var if presents
"   - let g:load_pending_files_cmd=get(g:, 'load_pending_files_cmd', 'git diff --name-only')
"
" The following one prints not only pending files, but also staged.
"
if !empty($FZF_PENDING_FILE_COMMAND)
  let g:load_pending_files_cmd=$FZF_PENDING_FILE_COMMAND
else
  let g:load_pending_files_cmd=get(g:, 'load_pending_files_cmd', 'echo $(git diff --cached --name-only)  $(git diff  --name-only) | tr " " "\n" | sort| uniq')
endif

function! fns#LoadPendingFiles()
  call fzf#run({
        \ 'source': g:load_pending_files_cmd,
        \ 'sink': 'e',
        \ 'down': '40%'})
endfunction

" This function is deprecated in favor of fns#LoadPendingFiles.
function! fns#LoadPendingFilesDeprecated()
  let file_list = split(system(g:load_pending_files_cmd))
  if len(file_list) == 0
    echo "no pending files"
    return
  endif
  let user_list = ['Choose file']
  let c = 1
  for file_path in file_list
    let item = "" . c . ". " . file_path
    call add(user_list, item)
    let c += 1
  endfor
  let answer = inputlist(user_list)
  if answer == 0
    return
  endif
  execute "e " . file_list[answer - 1]
endfunction

"" =============================================================================
" Shows a list to select for most recent files.
" Here we use the v:oldfiles to do the proxy.
"
function! fns#LoadRecentFiles()
  call fzf#run({
        \ 'source': v:oldfiles,
        \ 'sink': 'e',
        \ 'down': '40%'})
endfunction

