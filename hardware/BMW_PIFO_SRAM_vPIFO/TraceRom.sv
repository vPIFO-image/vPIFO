`timescale 1ns / 10ps


`define MAX2(v1, v2) ((v1) > (v2) ? (v1) : (v2))


module TRACE_ROM
#(
    parameter PTW             = 16, // Payload data width
    parameter MTW             = 16, // Metdata width should not less than TREE_NUM_BITS, cause tree_id will be placed in MTW
    parameter TREE_NUM        = 4,
    parameter ROM_SIZE        = 8,
    parameter IDLECYCLE       = 1024, // idle cycles
    parameter MEM_INIT_FILE   = "",

    localparam ROM_WIDTH        = $clog2(ROM_SIZE),
    localparam TREE_NUM_BITS    = $clog2(TREE_NUM),
    localparam IDLECYCLE_BITS   = $clog2(IDLECYCLE), // idle cycles
	localparam TRACE_DATA_BITS = `MAX2(IDLECYCLE_BITS, (PTW+TREE_NUM_BITS+MTW+PTW)) + 2
)(
   // Clock and Reset
    input                            i_clk,
    input                            i_arst_n,
    
    input                            i_read_en,
    input [ROM_WIDTH-1:0]            i_addr,

    output [TRACE_DATA_BITS-1:0]     o_trace_data
);

// a trace is a push
// trace data format:
// {bit, priority, tree_id, data} or {bit, idle_cycle}
// bit is used to mark if this entry is a packet

reg [TRACE_DATA_BITS-1:0] mem [0:ROM_SIZE-1];

assign o_trace_data = (i_read_en && i_arst_n) ? mem[i_addr] : '0;

initial begin
    if (MEM_INIT_FILE != "") begin
        $readmemb(MEM_INIT_FILE, mem);
    end
end

endmodule