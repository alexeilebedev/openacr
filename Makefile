default:
	ai
	abt -cfg:$(CFG) -install %

clean:
	git clean -dfx build temp

readme:
	atf_norm %readme

update_hdr:
	src_hdr -write
	mkdir -p $(OUTDIR) $(EXTRADIR)
