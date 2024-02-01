CXX = clang++
CXXFLAGS = -std=c++17 -g
LINKFLAGS = -lpcap

TESTCASE_DIR := ../compiler_test
TESTCASES = $(wildcard $(TESTCASE_DIR)/*.cpp)
LLFILES = $(patsubst $(TESTCASE_DIR)/%.cpp,$(TESTCASE_DIR)/%.tb,$(TESTCASES))

.SECONDARY: $(LLFILES)

run: $(patsubst $(TESTCASE_DIR)/%.cpp,$(TESTCASE_DIR)/%.tb,$(TESTCASES))


$(TESTCASE_DIR)/%.tb: $(TESTCASE_DIR)/%.output vPIFO_trace.exe ref2mem.py trace2mem.py draw_path_table.py
	./vPIFO_trace.exe $<
#	python3 ref2mem.py $<
#	python3 trace2mem.py $<
	python3 draw.py $<
#	python3 draw_path_table.py $< $@
	rm $<

vPIFO_trace.exe: vPIFO_trace.cpp util.o
	$(CXX) $(CXXFLAGS) $(LINKFLAGS) -o $@ $^

$(TESTCASE_DIR)/%.output: $(TESTCASE_DIR)/%.exe
	$< $@

$(TESTCASE_DIR)/%.exe: $(TESTCASE_DIR)/%.cpp PerfInfo.o PIFOTree.o SchedStrategy.o PFabric.o SP.o WFQ.o util.o
	$(CXX) $(CXXFLAGS) $(LINKFLAGS) -o $@ $^

getFlowId: getFlowId.exe
	./getFlowId.exe
	rm getFlowId.exe

getFlowId.exe: getFlowId.cpp util.o
	$(CXX) $(CXXFLAGS) $(LINKFLAGS) -o $@ $^

PerfInfo.o: PerfInfo.cpp PerfInfo.h
	$(CXX) $(CXXFLAGS) -c $<

SchedStrategy.o: SchedStrategy.cpp SchedStrategy.h PFabric.o SP.o WFQ.o
	$(CXX) $(CXXFLAGS) -c $<

PIFOTree.o: PIFOTree.cpp PIFOTree.h PerfInfo.o SchedStrategy.o
	$(CXX) $(CXXFLAGS) -c $<

PFabric.o: PFabric.cpp PFabric.h util.cpp util.h
	$(CXX) $(CXXFLAGS) -c $<

SP.o: SP.cpp SP.h util.cpp util.h
	$(CXX) $(CXXFLAGS) -c $<

WFQ.o: WFQ.cpp WFQ.h util.cpp util.h
	$(CXX) $(CXXFLAGS) -c $<

util.o: util.cpp util.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm *.o *.h.gch $(TESTCASE_DIR)/*.tb $(TESTCASE_DIR)/*.trace
