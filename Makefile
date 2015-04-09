site:
	bundle exec jekyll serve
	
docs:
	git fetch origin
	git checkout origin/master -- docs
 
.PHONY: site docs
