
desktop nspire:
	$(MAKE) -f Makefile.$@ $(filter-out $@,$(MAKECMDGOALS))

%:      # thanks to chakrit
    @:    # thanks to William Pursell

.PHONY: desktop nspire

