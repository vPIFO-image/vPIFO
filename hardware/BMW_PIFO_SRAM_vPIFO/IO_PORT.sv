`timescale 1ns / 10ps

// LEVEL == SRAM num, LEVEL == RPU num

module IO_PORT
#(
    parameter PTW    = 16,       // Payload data width
    parameter MTW    = 0,        // Metadata width
    parameter CTW    = 10,       // Counter width
    parameter LEVEL  = 4,         // Sub-tree level
    parameter TREE_NUM = 4,
    parameter FIFO_SIZE    = 8,

    localparam FIFO_WIDTH       = $clog2(FIFO_SIZE),
    localparam LEVEL_BITS       = $clog2(LEVEL),
    localparam TREE_NUM_BITS    = $clog2(TREE_NUM),
)(
    // Clock and Reset
    input                            i_clk,
    input                            i_arst_n,
    
    // Push and Pop port to the whole PIFO tree
    input [TREE_NUM_BITS-1:0]        i_tree_id,
    input                            i_push,
    input [(MTW+PTW)-1:0]            i_push_data,
    input                            i_pop,

    output                           o_task_fail,
    output [(MTW+PTW)-1:0]           o_pop_data
);

    reg [LEVEL-1:0] push;
    reg [LEVEL-1:0] pop;
    reg [(MTW+PTW)-1:0] push_data [0:LEVEL-1];
    reg [TREE_NUM_BITS-1:0] tree_id [0:LEVEL-1];
    reg [TREE_NUM_BITS-1:0] pop_tree_id;
    wire [(MTW+PTW)-1:0] pop_data [0:LEVEL-1];
    reg [(MTW+PTW)-1:0] pop_data_o;
    wire [LEVEL-1:0] task_fifo_full;
    wire [LEVEL-1:0] root_id;

    assign root_id = i_tree_id % LEVEL;

    for (genvar i = 0; i < LEVEL; i++) begin
        assign tree_id[i] = (root_id == i) ? i_tree_id : '0;
        assign push[i] = (root_id == i) ? i_push : '0;
        assign push_data[i] = (root_id == i) ? i_push_data : '0;
        assign pop[i] = (root_id == i) ? i_pop : '0;  
    end

    always_comb begin
        pop_data_o = '1;
        for(int i=0; i<LEVEL; ++i)begin
            if(pop_data[i] != '1)
                pop_data_o = pop_data[i];
        end
    end

    // if task fifo full then task fail
    // o_task_fail connect to all [LEVEL-1:0] task_fifo_full
    assign o_task_fail = task_fifo_full[root_id];

    assign o_pop_data = pop_data_o;



    PIFO_SRAM_TOP 
    #(
        .PTW   (PTW),
        .MTW   (MTW),
        .CTW   (CTW),
        .LEVEL (LEVEL),
        .TREE_NUM (TREE_NUM),
        .FIFO_SIZE (FIFO_SIZE)
    ) u_PIFO_TOP (
        .i_clk       ( i_clk          ),
        .i_arst_n    ( i_arst_n       ),
        
        .i_tree_id   ( tree_id        ),
        .i_push      ( push           ),
        .i_push_data ( push_data      ),
        
        .i_pop       ( pop            ),
        .o_pop_data  ( pop_data       ),
        .o_task_fifo_full (task_fifo_full)      
    );


endmodule