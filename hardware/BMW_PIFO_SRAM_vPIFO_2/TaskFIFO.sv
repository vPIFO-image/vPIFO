`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineer: Danish Azmi
// 
// Create Date:    21:42:56 11/05/2017 
// Design Name: Synchronus FIFO
// Module Name:    Syn_FIFO 
// Project Name: FIFO Design
// Target Devices: None
// Tool versions: ISE 14.2
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////

module TaskFIFO#(
   parameter PTW    = 16,       // Payload data width
   parameter MTW    = 16,       // Meta data width
   parameter TREE_NUM = 4,
   parameter BUF_SIZE    = 8,

   localparam TREE_NUM_BITS = $clog2(TREE_NUM),
   localparam BUF_WIDTH    = $clog2(BUF_SIZE),
   localparam TaskFIFO_DATA_BITS = (PTW+MTW)+(TREE_NUM_BITS+TREE_NUM_BITS)+2
)(
   input                 rst, clk, wr_en, rd_en,   
   // reset, system clock, write enable and read enable.
   input [TaskFIFO_DATA_BITS-1:0]           buf_in,                   
   // data input to be pushed to buffer
   output[TaskFIFO_DATA_BITS-1:0]           buf_out,                  
   // port to output the data using pop.
   output                buf_empty, buf_full,      
   // buffer empty and full indication 
   output[BUF_WIDTH:0] o_fifo_counter             
   // number of data pushed in to buffer   
);

reg[TaskFIFO_DATA_BITS-1:0]              buf_rd;
reg                   buf_empty_last;
reg                   rd_en_last, wr_en_last;
reg[BUF_WIDTH :0]    fifo_counter;
reg[BUF_WIDTH :0]    fifo_counter_nxt;
reg[BUF_WIDTH-1:0]  rd_ptr, wr_ptr;           // pointer to read and write addresses  
reg[BUF_WIDTH-1:0]  rd_ptr_nxt, wr_ptr_nxt;           // pointer to read and write addresses  
reg[TaskFIFO_DATA_BITS-1:0]              buf_mem[BUF_SIZE-1 : 0]; //  

assign buf_empty = (fifo_counter==0);   // Checking for whether buffer is empty or not
assign buf_full = (fifo_counter== BUF_SIZE);  //Checking for whether buffer is full or not
assign o_fifo_counter = fifo_counter;

always_comb begin
   if( wr_en && rd_en )
      fifo_counter_nxt = fifo_counter;
   else if( !buf_full && wr_en )			// When doing only write operation
      fifo_counter_nxt = fifo_counter + 1;
   else if( !buf_empty && rd_en )		//When doing only read operation
      fifo_counter_nxt = fifo_counter - 1;
   else
      fifo_counter_nxt = fifo_counter;	// When doing nothing or read & write or overflow
end

always_ff @( posedge clk ) begin
   rd_en_last <= rd_en;
   wr_en_last <= wr_en;
   buf_empty_last <= buf_empty;
end

//Setting FIFO counter value for different situations of read and write operations.
always @(posedge clk or posedge rst)
begin
   if( rst )
      fifo_counter <= 0;		// Reset the counter of FIFO
   else
      fifo_counter <= fifo_counter_nxt;
end

always @( posedge clk or posedge rst)
begin
   if( rst )
      buf_rd <= 0;		//On reset output of buffer is all 0.
   else begin
      buf_rd <= buf_mem[ rd_ptr ];	//Reading the 8 bit data from buffer location indicated by read pointer	
   end
end

assign buf_out = (rd_en_last && ((!buf_empty_last) | wr_en_last)) ? buf_rd : 0;

always @(posedge clk)
begin
   if( wr_en && (!buf_full | rd_en) )
      buf_mem[ wr_ptr ] <= buf_in;		//Writing 8 bit data input to buffer location indicated by write pointer
   else
      buf_mem[ wr_ptr ] <= buf_mem[ wr_ptr ];
end

always_comb begin
   if( wr_en && (!buf_full | rd_en) )    
		wr_ptr_nxt = wr_ptr + 1;		// On write operation, Write pointer points to next location
   else  
		wr_ptr_nxt = wr_ptr;   
end

always_comb begin
   if( rd_en && ((!buf_empty) | wr_en) )   
		rd_ptr_nxt = rd_ptr + 1;		// On read operation, read pointer points to next location to be read
   else 
		rd_ptr_nxt = rd_ptr;
end

always@(posedge clk or posedge rst)
begin
   if( rst ) begin
      wr_ptr <= 0;		// Initializing write pointer
      rd_ptr <= 0;		//Initializing read pointer
   end else begin
      wr_ptr <= wr_ptr_nxt;
      rd_ptr <= rd_ptr_nxt;
   end
end
endmodule
