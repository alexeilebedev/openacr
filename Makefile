default:
	ai

# create script to build abt for the first time
# delete unstable gitinfo information
bootstrap:
	(echo "echo '# this script has been created using make bootstrap'"; \
	echo "echo '# now building an abt executable which will build the rest'"; \
	abt -install '(abt|src_hdr)' -printcmd -report:N -nover) > bin/abt-bootstrap; \
	cpp-indent bin/abt-bootstrap >/dev/null 2>&1

clean:
	rm -rf dflt.*-x86_64 .testgen
	git clean -dfx temp

readme:
	atf_norm readme
