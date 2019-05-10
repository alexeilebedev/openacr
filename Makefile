default:
	ai

# create script to build abt for the first time
# delete unstable gitinfo information
bootstrap:
	bin/make-bootstrap

clean:
	rm -rf dflt.*-x86_64 .testgen
	git clean -dfx temp

readme:
	atf_norm readme
