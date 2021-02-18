lib := lib
app := app

.PHONY: all $(app) $(lib)

all: $(app) $(lib)

$(lib):
	$(MAKE) --directory=$@
$(app): $(lib)
	$(MAKE) --directory=$@


.PHONY: clean
clean:
	rm -r build