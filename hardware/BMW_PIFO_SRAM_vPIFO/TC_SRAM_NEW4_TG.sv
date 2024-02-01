`timescale 1ns/10ps    //cycle

/*-----------------------------------------------------------------------------

Proprietary and Confidential Information

Module: TC.v
Author: Xiaoguang Li
Date  : 06/2/2019

Description: Top-level module simulation. 

			 
Issues:  

-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Module Port Definition
//-----------------------------------------------------------------------------
module TC();

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Register and Wire Declarations
//-----------------------------------------------------------------------------

// 4 LEVEL push to full and pop to empty



parameter LEVEL = 5;
parameter TREE_NUM = 5;
parameter FIFO_SIZE = 2048;
parameter TREE_NUM_BITS = $clog2(TREE_NUM);
parameter PTW = 16;
parameter MTW = TREE_NUM_BITS;
parameter CTW = 16;

reg            clk;
reg            arst_n;

integer        seed;
integer        R;
reg push;
reg pop;
reg [(MTW+PTW)-1:0] push_data;
reg [TREE_NUM_BITS-1:0]      push_tree_id;
reg [PTW-1:0]      push_priority;
reg [TREE_NUM_BITS-1:0]      pop_tree_id;
integer        data_gen [49:0];
integer        i, j;
wire [(MTW+PTW)-1:0]      pop_data;
wire task_fifo_full;

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
   .i_push_priority   ( push_priority        ),
   .i_push      ( push           ),
   .i_push_data ( push_data      ),
   
   .i_pop       ( pop            ),
   .o_pop_tree_id  ( pop_tree_id       ),
   .o_pop_data  ( pop_data       ),
   .o_task_fifo_full (task_fifo_full)      
);

//-----------------------------------------------------------------------------
// Clocks
//-----------------------------------------------------------------------------
always #4 begin clk = ~clk; end
  
//-----------------------------------------------------------------------------
// Initial
//-----------------------------------------------------------------------------  

initial begin            
   $dumpfile("wave.vcd");
   $dumpvars; // dump all vars
end

initial begin
    for (i=0;i<50;i=i+1) begin
      data_gen[i] = $dist_uniform(seed,0,256);
    end
end

initial
begin
  clk    = 1'b0;
  arst_n = 1'b0;    
  seed   = 1;
  push   = 1'b0;
  pop    = 1'b0;
  push_tree_id = '0;
  push_priority = '0;
 
  #400;
  arst_n = 1'b1;

  // push all for prepare
  for (j=1; j< 5; j=j+1) begin
    for (i=1; i< TREE_NUM; i=i+1) begin
      @ (posedge clk);
      fork
        pop = 1'b0;
        push = 1'b1;
        push_data = 4096 * i + j;
        push_tree_id = i;
        push_priority = i;
      join
    end
  end

  @ (posedge clk);
  fork 
      pop = 1'b0;
      push = 1'b0;
      push_data = 0;
      push_tree_id = 0;
      push_priority = 0;
  join

  // pop all
  for (j=1; j< 5; j=j+1) begin
    for (i=1; i< TREE_NUM; i=i+1) begin
      @ (posedge clk);
      fork
        pop = 1'b1;
        push = 1'b0;
        push_data = 0;
        push_tree_id = 0;
        push_priority = 0;
      join
    end
  end

  @ (posedge clk);
  fork 
      pop = 1'b0;
      push = 1'b0;
      push_data = 0;
      push_tree_id = 0;
      push_priority = 0;
  join

  #5000;   
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
