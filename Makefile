
nspire linux windows:
	./make-bundles.sh
	$(MAKE) -f Makefile.$@ $(filter-out $@,$(MAKECMDGOALS))

%:      # thanks to chakrit
    @:    # thanks to William Pursell

.PHONY: linux nspire windows