default:
	ai

clean:
	rm -rf temp/acr_ed
	git clean -dfx build temp

readme:
	-atf_norm readme
