lib := lib
app := app
test := test

.PHONY: all $(app) $(lib) $(test)

all: $(app) $(lib) $(test)

$(lib):
	$(MAKE) --directory=$@
$(app): $(lib)
	$(MAKE) --directory=$@
$(test): $(lib)
	$(MAKE) --directory=$@

.PHONY: clean
clean:
	rm -r build