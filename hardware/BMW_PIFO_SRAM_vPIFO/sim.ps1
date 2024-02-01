# iverilog -g2005-sv INFER_SDPRAM.v PIFO_SRAM_TOP.sv PIFO_SRAM.sv TaskDistribute.sv TaskFIFO.sv TC_SRAM_NEW4.sv
# vvp a.out

# iverilog -g2005-sv INFER_SDPRAM.v PIFO_SRAM_TOP.sv PIFO_SRAM.sv TaskDistribute.sv TaskFIFO.sv TaskGenerator.sv TC_SRAM_NEW3_TG.sv
# vvp a.out

# python .\readalbe2mem.py
iverilog -g2005-sv INFER_SDPRAM.v PIFO_SRAM_TOP.sv PIFO_SRAM.sv TaskDistribute.sv TaskFIFO.sv TaskGenerator.sv TraceReader.sv TraceRom.sv TC_SimTop.sv
vvp a.out