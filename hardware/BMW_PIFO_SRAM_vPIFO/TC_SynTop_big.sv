`timescale 1ns/10ps    //cycle

`define MAX2(v1, v2) ((v1) > (v2) ? (v1) : (v2))

module SYN_TOP(
	input clk_n, clk_p, btnC
);

parameter LEVEL = 4;
parameter TREE_NUM = 4;
parameter FIFO_SIZE = 8;
parameter PTW = 16;
parameter MTW = $clog2(TREE_NUM);
parameter CTW = 16;
parameter IDLECYCLE = 1024;
parameter ROM_SIZE = 16;
parameter MEM_INIT_FILE = "test2_1.mem";



parameter TREE_NUM_BITS = $clog2(TREE_NUM);
parameter ROM_WIDTH = $clog2(ROM_SIZE);
parameter IDLECYCLE_BITS   = $clog2(IDLECYCLE); // idle cycles
parameter TRACE_DATA_BITS = `MAX2(IDLECYCLE_BITS, (PTW+TREE_NUM_BITS+MTW+PTW)) + 2;



(*mark_debug = "true"*) reg push;
(*mark_debug = "true"*) reg pop;
(*mark_debug = "true"*) reg my_clk;
(*mark_debug = "true"*) reg [(MTW+PTW)-1:0] push_data;
(*mark_debug = "true"*) reg [TREE_NUM_BITS-1:0] push_tree_id;
(*mark_debug = "true"*) reg [PTW-1:0] push_priority;
(*mark_debug = "true"*) reg [TREE_NUM_BITS-1:0] pop_tree_id;
(*mark_debug = "true"*) wire [(MTW+PTW)-1:0] pop_data;
(*mark_debug = "true"*) wire task_fifo_full;
(*mark_debug = "true"*) wire [ROM_WIDTH-1:0] trace_addr;
(*mark_debug = "true"*) wire [TRACE_DATA_BITS-1:0] trace_data;
(*mark_debug = "true"*) wire read;
(*mark_debug = "true"*) wire finish;
(*mark_debug = "true"*) wire pop_out;
(*mark_debug = "true"*) wire arst_n;


assign arst_n = btnC;


//-----------------------------------------------------------------------------
// Instantiations
//-----------------------------------------------------------------------------



TASK_GENERATOR 
#(
   .PTW   (PTW),
   .MTW   (MTW),
   .CTW   (CTW),
   .LEVEL (LEVEL),
   .TREE_NUM (TREE_NUM),
   .FIFO_SIZE (FIFO_SIZE)
) u_TASK_GENERATOR (
   .i_clk       ( my_clk            ),
   .i_arst_n    ( arst_n          ),
   
   .i_push_tree_id   ( push_tree_id        ),
   .i_push_priority  ( push_priority        ),
   .i_push      ( push           ),
   .i_push_data ( push_data      ),
   
   .i_pop       ( pop            ),

   .o_pop_tree_id  ( pop_tree_id       ),
   .o_pop_data  ( pop_data       ),
   .o_pop_out  ( pop_out       ),
   .o_task_fifo_full (task_fifo_full)      
);



TRACE_READER
#(
    .PTW   (PTW),
    .MTW   (MTW),
    .TREE_NUM (TREE_NUM),
    .IDLECYCLE (IDLECYCLE),
    .ROM_SIZE (ROM_SIZE)
) u_TRACE_READER (
   .i_clk       ( my_clk ),
   .i_arst_n    ( arst_n ),

   .i_trace_data    ( trace_data ),
   
   .o_push      ( push ),
   .o_push_priority ( push_priority ),
   .o_push_tree_id ( push_tree_id ),
   .o_push_data ( push_data ),
   
   .o_pop       ( pop ),

   .o_read_addr  ( trace_addr ),
   .o_read  ( read ),
   .o_finish ( finish )
);



TRACE_ROM
#(
    .PTW   (PTW),
    .MTW   (MTW),
    .TREE_NUM (TREE_NUM),
    .IDLECYCLE (IDLECYCLE),
    .ROM_SIZE (ROM_SIZE),
    .MEM_INIT_FILE (MEM_INIT_FILE)
) u_TRACE_ROM (
   .i_clk       ( my_clk ),
   .i_arst_n    ( arst_n ),

   .i_read_en    ( read ),

   .i_addr  ( trace_addr ),

   .o_trace_data  ( trace_data )
);

clk_wiz_0 clk_wiz_0_clk(.clk_in1_n(clk_n), .clk_in1_p(clk_p), .clk_out1(my_clk));

//-----------------------------------------------------------------------------
// Clocks
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Initial
//-----------------------------------------------------------------------------  


//-----------------------------------------------------------------------------
// Functions and Tasks
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Sequential Logic
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Combinatorial Logic / Continuous Assignments
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Output Assignments
//-----------------------------------------------------------------------------  

endmodule
