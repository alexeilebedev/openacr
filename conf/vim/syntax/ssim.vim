" Algo SSIM syntax file
" $Id$
" $DateTime$

if exists("b:current_syntax")
    finish
endif

syntax region ssimString start=/\v"/ skip=/\v\\./ end=/\v"/
syntax region ssimString start=/\v'/ skip=/\v\\./ end=/\v'/
highlight link ssimString String

syn match ssimKeyword /\v(\w|\-)*:/me=e-1
highlight link ssimKeyword Keyword

syntax match ssimOperator "\v\:"
highlight link ssimOperator Operator

" leading word on a line
syn match ssimIdentifier "\v^ *(\w|\-)+\.(\w|\-)+ "
highlight link ssimIdentifier Identifier
 
" anything to the right of :
syn match ssimType /\v\:(\w|\.|\%|\-|\@|\/|\*)+/lc=1
highlight link ssimType Type

let b:current_syntax = "ssim"
