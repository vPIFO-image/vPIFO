`timescale 1ns/10ps    //cycle

`define MAX2(v1, v2) ((v1) > (v2) ? (v1) : (v2))

module SIM_TOP();

parameter LEVEL = 5;
parameter TREE_NUM = 3;
parameter FIFO_SIZE = 2048;
parameter PTW = 28;
parameter MTW = 20;
parameter CTW = 16;
parameter IDLECYCLE = 256;
parameter ROM_SIZE = 65536;
parameter MEM_INIT_FILE = "test2_1.mem";
parameter OUTPUT_FILE = "output2_1.txt";



parameter TREE_NUM_BITS = $clog2(TREE_NUM);
parameter ROM_WIDTH = $clog2(ROM_SIZE);
parameter IDLECYCLE_BITS   = $clog2(IDLECYCLE); // idle cycles
parameter TRACE_DATA_BITS = `MAX2(IDLECYCLE_BITS, (PTW+TREE_NUM_BITS+MTW+PTW)) + 2;

reg            clk;
reg            arst_n;

integer        seed;
integer        R;
integer f;
reg push;
reg pop;
reg [16:0] push_cnt;
reg [16:0] pop_cnt;
reg [(MTW+PTW)-1:0] push_data;
reg [TREE_NUM_BITS-1:0] push_tree_id;
reg [PTW-1:0] push_priority;
reg [TREE_NUM_BITS-1:0] pop_tree_id;
integer data_gen [49:0];
integer i, j;
wire [(MTW+PTW)-1:0] pop_data;
wire task_fifo_full;
wire [ROM_WIDTH-1:0] trace_addr;
wire [TRACE_DATA_BITS-1:0] trace_data;
wire read;
wire finish;
wire pop_out;

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
   .i_clk       ( clk            ),
   .i_arst_n    ( arst_n         ),
   
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
   .i_clk       ( clk ),
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
   .i_clk       ( clk ),
   .i_arst_n    ( arst_n ),

   .i_read_en    ( read ),

   .i_addr  ( trace_addr ),

   .o_trace_data  ( trace_data )
);

//-----------------------------------------------------------------------------
// Clocks
//-----------------------------------------------------------------------------
always #4 begin clk = ~clk; end
  
//-----------------------------------------------------------------------------
// Initial
//-----------------------------------------------------------------------------  

// initial begin            
//    $dumpfile("wave.vcd");
//    $dumpvars; // dump all vars
// end

initial begin
   for (i=0;i<50;i=i+1) begin
     data_gen[i] = $dist_uniform(seed,0,256);
   end
end

always_ff @( posedge clk ) begin
   if (pop_out) begin
      // $fwrite(f, "meta:%d, priority:%d\n", pop_data[(MTW+PTW)-1:PTW], pop_data[PTW-1:0]);
      $fwrite(f,"%0d %0x\n", pop_data[PTW-1:0], pop_cnt);
   end
end

always @ (posedge clk or negedge arst_n) begin
   if (!arst_n) begin
      push_cnt <= '0;
      pop_cnt <= '0;
   end else begin
      if (push) begin
         push_cnt <= push_cnt + 1;
      end
      if (pop_out) begin
         pop_cnt <= pop_cnt + 1;
      end
   end
end

initial
begin
  clk    = 1'b0;
  arst_n = 1'b0;
   f = $fopen(OUTPUT_FILE, "w");
 
  #400;
  arst_n = 1'b1;   

  
  #500000;
   $fclose(f);
  $stop;

  end

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
