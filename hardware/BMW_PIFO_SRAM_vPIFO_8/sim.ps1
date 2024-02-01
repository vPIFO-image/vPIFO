iverilog -g2005-sv INFER_SDPRAM.v PIFO_SRAM_TOP.sv PIFO_SRAM.sv TaskDistribute.sv TaskFIFO.sv TC_SRAM_NEW4.sv
vvp a.out

# iverilog -g2005-sv INFER_SDPRAM.v PIFO_SRAM_TOP.sv PIFO_SRAM.sv TaskDistribute.sv TaskFIFO.sv TaskGenerator.sv TC_SRAM_NEW4_TG.sv
# vvp a.out