SHELL:=/usr/bin/env bash
ifeq ($(CFG),)
CFG:=release
endif

default:
	ai
	abt -cfg:$(CFG) -install %

clean:
	git clean -dfx build temp
