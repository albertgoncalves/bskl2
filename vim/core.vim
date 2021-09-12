" $ cp core.vim ~/.vim/syntax/core.vim
" $ grep '.core' ~/.vimrc
" autocmd BufNewFile,BufRead *.core setlocal filetype=core

if exists("b:current_syntax")
    finish
endif

syn match Comment  "#.*$"
syn match Operator "[(){}=<>+*/;]"
syn match Operator "-[^\-]"
syn match Number   "\<[0-9]\+\>"

syn keyword Keyword
    \ undef
    \ if
    \ negate
syn keyword Statement
    \ let
    \ letrec
    \ pack
    \ unpack

let b:current_syntax = "core"
