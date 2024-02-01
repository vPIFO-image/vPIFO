TESTCASE_DIR := ./tests
TESTCASES = $(wildcard $(TESTCASE_DIR)/*.txt)
LLFILES = $(patsubst $(TESTCASE_DIR)/%.txt,$(TESTCASE_DIR)/%.tb,$(TESTCASES))

.SECONDARY: $(LLFILES)

run: $(patsubst $(TESTCASE_DIR)/%.txt,$(TESTCASE_DIR)/%.tb,$(TESTCASES))

$(TESTCASE_DIR)/%.tb: $(TESTCASE_DIR)/%.txt
	python3 gen_SRAM_layout.py $<

clean:
	rm $(TESTCASE_DIR)/*.tb $(TESTCASE_DIR)/*.mem
