
nspire linux windows:
	$(MAKE) -f Makefile.$@ $(filter-out $@,$(MAKECMDGOALS))

%:      # thanks to chakrit
    @:    # thanks to William Pursell

.PHONY: linux nspire windows

