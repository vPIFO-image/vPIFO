`timescale 1ns / 10ps






module TASK_GENERATOR
#(
    parameter PTW             = 16, // Payload data width
    parameter MTW             = TREE_NUM_BITS, // Metdata width should not less than TREE_NUM_BITS, cause tree_id will be placed in MTW
    parameter CTW             = 10, // Counter width
    parameter LEVEL           = 4, // Sub-tree level
    parameter TREE_NUM        = 4,
    parameter FIFO_SIZE       = 8,

    localparam FIFO_WIDTH       = $clog2(FIFO_SIZE),
    localparam LEVEL_BITS       = $clog2(LEVEL),
    localparam TREE_NUM_BITS    = $clog2(TREE_NUM),
    localparam ROOT_TREE_ID     = 0,
    localparam ROOT_RPU_ID      = 0
)(
    // Clock and Reset
    input                            i_clk,
    input                            i_arst_n,
    
    // Push and Pop port to the whole PIFO tree
    input [TREE_NUM_BITS-1:0]        i_push_tree_id,
    input [PTW-1:0]                  i_push_priority,
    input                            i_push,
    input [(MTW+PTW)-1:0]            i_push_data,
    
    input                            i_pop,
    output [TREE_NUM_BITS-1:0]       o_pop_tree_id,
    output [(MTW+PTW)-1:0]           o_pop_data,
    output                           o_pop_out,

    output                           o_task_fifo_full
);

    reg [LEVEL-1:0] push;
    reg [LEVEL-1:0] pop;
    reg [TREE_NUM_BITS-1:0] o_pop_tree_id_reg;
    reg [(MTW+PTW)-1:0] o_pop_data_reg;

    reg [TREE_NUM_BITS-1:0]        i_push_tree_id_reg;
    reg [PTW-1:0]                  i_push_priority_reg;
    reg                            i_push_reg;
    reg [(MTW+PTW)-1:0]            i_push_data_reg;
    reg                            o_pop_out_reg;

    wire [TREE_NUM_BITS-1:0] pop_tree_id;
    wire [TREE_NUM_BITS-1:0] pifo_o_tree_id [0:LEVEL-1];
    wire [TREE_NUM_BITS-1:0] pifo_i_push_tree_id [0:LEVEL-1];
    wire [TREE_NUM_BITS-1:0] pifo_i_pop_tree_id [0:LEVEL-1];
    wire [(MTW+PTW)-1:0] push_data [0:LEVEL-1];
    wire [(MTW+PTW)-1:0] pop_data [0:LEVEL-1];
    wire [LEVEL-1:0] task_fifo_full;
    wire [LEVEL-1:0] is_level0_pop;
    wire [LEVEL_BITS-1:0] push_rpu_id;
    wire [LEVEL_BITS-1:0] pop_rpu_id;


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
        
        .i_push_tree_id ( pifo_i_push_tree_id ),
        .i_push      ( push           ),
        .i_push_data ( push_data      ),
        
        .i_pop       ( pop            ),
        .i_pop_tree_id ( pifo_i_pop_tree_id ),

        .o_tree_id   ( pifo_o_tree_id ),
        .o_pop_data  ( pop_data       ),

        .o_is_level0_pop ( is_level0_pop ), 
        .o_task_fifo_full ( task_fifo_full )   
    );

    // ROOT_TREE_ID is 0
    // ROOT_RPU_ID is 0
    // should not be pushed by i_push
    assign push_rpu_id = i_push_tree_id % LEVEL;

    for (genvar i = 1; i < LEVEL; i++) begin
        assign push[i] = (i == push_rpu_id) ? i_push : '0;
        assign push_data[i] = (i == push_rpu_id && i_push) ? i_push_data : '1;
    end
    // if i_push, ROOT_TREE will do push
    assign push[ROOT_RPU_ID] = i_push;
    assign push_data[ROOT_RPU_ID] = i_push ? {i_push_tree_id, {MTW-TREE_NUM_BITS{1'b0}}, i_push_priority} : '1;

    // pop is for ROOT_TREE_ID
    // i.e. if i_pop is 1 then ROOT_TREE will do pop
    // and generate a pop for a subtree
    assign pop_tree_id = is_level0_pop[0] ? pop_data[0][(MTW+PTW-1) -: TREE_NUM_BITS] : ROOT_RPU_ID;
    assign pop_rpu_id = pop_tree_id % LEVEL;

    for (genvar i = 1; i < LEVEL; i++) begin
        assign pop[i] = (i == pop_rpu_id);
    end

    assign pop[ROOT_RPU_ID] = i_pop;


    for (genvar i = 1; i < LEVEL; i++) begin
        assign pifo_i_push_tree_id[i] = (i == push_rpu_id) ? i_push_tree_id : ROOT_TREE_ID;
        assign pifo_i_pop_tree_id[i] = (i == pop_rpu_id) ? pop_tree_id : ROOT_TREE_ID;
    end

    assign pifo_i_push_tree_id[ROOT_RPU_ID] = ROOT_TREE_ID;
    assign pifo_i_pop_tree_id[ROOT_RPU_ID] = ROOT_TREE_ID;

    // now i can select one tree to do pop
    // so only one is_level0_pop will be 1 per cycle
    always_comb begin
        o_pop_tree_id_reg = '0;
        o_pop_data_reg = '1;
        o_pop_out_reg = '0;
        for(int i=1; i<LEVEL; ++i)begin
            if(is_level0_pop[i]) begin
                o_pop_out_reg = 1'b1;
                o_pop_tree_id_reg = pifo_o_tree_id[i];
                o_pop_data_reg = pop_data[i];
            end
        end
    end

    // assign o_pop_tree_id_reg = pifo_o_tree_id[0];

    // now ROOT_RPU_ID's task_fifo has the largest length
    // so it will be full first
    assign o_task_fifo_full = task_fifo_full[ROOT_RPU_ID];
    assign o_pop_tree_id = o_pop_tree_id_reg;
    assign o_pop_data = o_pop_data_reg;
    assign o_pop_out = o_pop_out_reg;


endmodule