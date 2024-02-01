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

parameter LEVEL = 4;

reg            clk;
reg            arst_n;

integer        seed;
integer        R;
reg [LEVEL-1:0] push;
reg [LEVEL-1:0] pop;
reg [7:0]      push_data [0:LEVEL-1];
reg [1:0]      tree_id [0:LEVEL-1];
integer        data_gen [49:0];
integer        i;
wire [7:0]      pop_data [0:LEVEL-1];
wire [LEVEL-1:0] task_fifo_full;

reg [7:0]      push_data_0;
reg [7:0]      push_data_1;
reg [7:0]      push_data_2;
reg [7:0]      push_data_3;

reg [7:0]      pop_data_0;
reg [7:0]      pop_data_1;
reg [7:0]      pop_data_2;
reg [7:0]      pop_data_3;

//-----------------------------------------------------------------------------
// Instantiations
//-----------------------------------------------------------------------------
PIFO_SRAM_TOP 
#(
   .PTW   (8),
   .MTW   (0),
   .CTW   (8),
   .LEVEL (4)
) u_PIFO_TOP (
   .i_clk       ( clk            ),
   .i_arst_n    ( arst_n         ),
   
   .i_tree_id   ( tree_id        ),
   .i_push      ( push           ),
   .i_push_data ( push_data      ),
   
   .i_pop       ( pop            ),
   .o_pop_data  ( pop_data       ),
   .o_task_fifo_full (task_fifo_full)      
);

//-----------------------------------------------------------------------------
// Clocks
//-----------------------------------------------------------------------------
always #4 begin clk = ~clk; end

assign push_data_0 = push_data[0];
assign push_data_1 = push_data[1];
assign push_data_2 = push_data[2];
assign push_data_3 = push_data[3];

assign pop_data_0 = pop_data[0];
assign pop_data_1 = pop_data[1];
assign pop_data_2 = pop_data[2];
assign pop_data_3 = pop_data[3];
  
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

  for (integer j = 0; j < LEVEL; j++) begin
    tree_id[j] = '0;
  end
 
   #400;
   arst_n = 1'b1;

// push 2 for prepare
  for (i=0; i<3; i=i+1) begin
    @ (posedge clk);
    fork
      // push 2
      push[2] = 1'b1;
      push_data[2] = i+1;
      tree_id[2] = 2;
      pop[2] = 1'b0;
    join
  end

  @ (posedge clk);
  fork 
    for (integer j = 0; j < LEVEL; j++) begin
      pop[j] = 1'b0;
      push[j] = 1'b0;
      push_data[j] = 0;
      tree_id[j] = j;
    end
  join

// push 0 for prepare
  for (i=0; i<7; i=i+1) begin
    @ (posedge clk);
    fork
      // push 0
      push[0] = 1'b1;
      push_data[0] = i+1;
      tree_id[0] = 0;
      pop[0] = 1'b0;
    join
  end

  for (i=0; i<30; i=i+1) begin
    @ (posedge clk);
    fork 
      for (integer j = 0; j < LEVEL; j++) begin
        pop[j] = 1'b0;
        push[j] = 1'b0;
        push_data[j] = 0;
        tree_id[j] = j;
      end
    join
  end

// push 0 and pop 2 conflict
   for (i=0; i<3; i=i+1) begin
     @ (posedge clk);
      fork
        // push 0
        for (integer j = 0; j < LEVEL; j++) begin
          if(j == 0)begin
            push[j] = 1'b1;
            push_data[j] = i+8;
            tree_id[j] = j;
            pop[j] = 1'b0;
          end else begin
            pop[j] = 1'b0;
            push[j] = 1'b0;
            push_data[j] = 0;
            tree_id[j] = j;
          end
        end
        
      join
      @ (posedge clk);
      fork
        // pop 2
        for (integer j = 0; j < LEVEL; j++) begin
          if(j == 2)begin
            pop[j] = 1'b1;
            push[j] = 1'b0;
            push_data[j] = 0;
            tree_id[j] = j;
          end else begin
            pop[j] = 1'b0;
            push[j] = 1'b0;
            push_data[j] = 0;
            tree_id[j] = j;
          end
        end
      join
    end


// pop 0 until empty
    for (i=0; i<30; i=i+1) begin
      @ (posedge clk);
      fork 
        for (integer j = 0; j < LEVEL; j++) begin
          pop[j] = 1'b0;
          push[j] = 1'b0;
          push_data[j] = 0;
          tree_id[j] = j;
        end
      join
    end

    for (i=0; i<6; i=i+1) begin
      @ (posedge clk);
      fork
        // pop 0
        for (integer j = 0; j < LEVEL; j++) begin
          if(j == 0)begin
            pop[j] = 1'b1;
            push[j] = 1'b0;
            push_data[j] = 0;
            tree_id[j] = j;
          end else begin
            pop[j] = 1'b0;
            push[j] = 1'b0;
            push_data[j] = 0;
            tree_id[j] = j;
          end
        end
      join
    end

    for (i=0; i<30; i=i+1) begin
      @ (posedge clk);
      fork 
        for (integer j = 0; j < LEVEL; j++) begin
          pop[j] = 1'b0;
          push[j] = 1'b0;
          push_data[j] = 0;
          tree_id[j] = j;
        end
      join
    end

    for (i=0; i<6; i=i+1) begin
      @ (posedge clk);
      fork
        // pop 0
        for (integer j = 0; j < LEVEL; j++) begin
          if(j == 0)begin
            pop[j] = 1'b1;
            push[j] = 1'b0;
            push_data[j] = 0;
            tree_id[j] = j;
          end else begin
            pop[j] = 1'b0;
            push[j] = 1'b0;
            push_data[j] = 0;
            tree_id[j] = j;
          end
        end
      join
    end


   @ (posedge clk);
   fork 
      for (integer j = 0; j < LEVEL; j++) begin
        pop[j] = 1'b0;
        push[j] = 1'b0;
        push_data[j] = 0;
        tree_id[j] = j;
      end
   join
  
   #1000;   
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
