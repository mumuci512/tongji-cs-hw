`timescale 1ns / 1ps
module Decoder(   // 不需要则为高阻抗   
    input  [31:0] inst,
    output add_flag,           
    output addu_flag,        
    output sub_flag,        
    output subu_flag,       
    output and_flag,       
    output or_flag,        
    output xor_flag,     
    output nor_flag,     
    output slt_flag,   
    output sltu_flag,  
    output sll_flag,     
    output srl_flag,  
    output sra_flag,   
    output sllv_flag, 
    output srlv_flag, 
    output srav_flag,  
    output jr_flag,     
    output addi_flag,   
    output addiu_flag, 
    output andi_flag,   
    output ori_flag,    
    output xori_flag,  
    output lw_flag,  
    output sw_flag,    
    output beq_flag,  
    output bne_flag,  
    output slti_flag, 
    output sltiu_flag,   
    output lui_flag,  
    output j_flag,   
    output jal_flag,
    output clz_flag,
    output jalr_flag,
    output mthi_flag,
    output mtlo_flag,
    output mfhi_flag,
    output mflo_flag,
    output sb_flag,
    output sh_flag,
    output lb_flag,
    output lh_flag,
    output lbu_flag,
    output lhu_flag,
    output eret_flag,
    output break_flag,
    output syscall_flag,
    output teq_flag,
    output mfc0_flag,
    output mtc0_flag,
    output mult_flag,
    output multu_flag,
    output div_flag,
    output divu_flag,
    output bgez_flag
    );
// R型指令
parameter add_op   = 6'b100000;
parameter addu_op  = 6'b100001;
parameter sub_op   = 6'b100010;
parameter subu_op  = 6'b100011;
parameter and_op   = 6'b100100;
parameter or_op    = 6'b100101;
parameter xor_op   = 6'b100110;
parameter nor_op   = 6'b100111;
parameter slt_op   = 6'b101010;
parameter sltu_op  = 6'b101011;
parameter sll_op   = 6'b000000;
parameter srl_op   = 6'b000010;
parameter sra_op   = 6'b000011;
parameter sllv_op  = 6'b000100;
parameter srlv_op  = 6'b000110;
parameter srav_op  = 6'b000111;
parameter jr_op    = 6'b001000;
// I型指令
parameter addi_op  = 6'b001000;
parameter addiu_op = 6'b001001;
parameter andi_op  = 6'b001100;
parameter ori_op   = 6'b001101;
parameter xori_op  = 6'b001110;
parameter lw_op    = 6'b100011;
parameter sw_op    = 6'b101011;
parameter beq_op   = 6'b000100;
parameter bne_op   = 6'b000101;
parameter slti_op  = 6'b001010;
parameter sltiu_op = 6'b001011;
parameter lui_op   = 6'b001111;
parameter j_op     = 6'b000010;
parameter jal_op   = 6'b000011;

// 23条额外指令
parameter CLZ_OPE     = 6'b100000;
parameter JALR_OPE    = 6'b001001;
parameter MTHI_OPE    = 6'b010001;
parameter MFHI_OPE    = 6'b010000;
parameter MTLO_OPE    = 6'b010011;
parameter MFLO_OPE    = 6'b010010;
parameter SB_OPE      = 6'b101000;
parameter SH_OPE      = 6'b101001;
parameter LB_OPE      = 6'b100000;
parameter LH_OPE      = 6'b100001;
parameter LBU_OPE     = 6'b100100;
parameter LHU_OPE     = 6'b100101;
parameter ERET_OPE    = 6'b011000;
parameter BREAK_OPE   = 6'b001101;
parameter SYSCALL_OPE = 6'b001100;
parameter TEQ_OPE     = 6'b110100;
parameter MFC0_OPE    = 5'b00000;
parameter MTC0_OPE    = 5'b00100; //注意这两个指令是靠另外五位order区分的，op和func都一样
parameter MULT_OPE     = 6'b011000;
parameter MULTU_OPE   = 6'b011001;
parameter DIV_OPE     = 6'b011010;
parameter DIVU_OPE    = 6'b011011;
parameter BGEZ_OPE    = 6'b000001;

// R型指令inst[31,26]为全0，inst[5,0]为opcode
assign add_flag  = ((inst[31:26] == 6'h0) && (inst[5:0] == add_op )) ? 1'b1 : 1'b0;
assign addu_flag = ((inst[31:26] == 6'h0) && (inst[5:0] == addu_op)) ? 1'b1 : 1'b0;
assign sub_flag  = ((inst[31:26] == 6'h0) && (inst[5:0] == sub_op )) ? 1'b1 : 1'b0;
assign subu_flag = ((inst[31:26] == 6'h0) && (inst[5:0] == subu_op)) ? 1'b1 : 1'b0;
assign and_flag  = ((inst[31:26] == 6'h0) && (inst[5:0] == and_op )) ? 1'b1 : 1'b0;
assign or_flag   = ((inst[31:26] == 6'h0) && (inst[5:0] == or_op  )) ? 1'b1 : 1'b0;
assign xor_flag  = ((inst[31:26] == 6'h0) && (inst[5:0] == xor_op )) ? 1'b1 : 1'b0;
assign nor_flag  = ((inst[31:26] == 6'h0) && (inst[5:0] == nor_op )) ? 1'b1 : 1'b0;
assign slt_flag  = ((inst[31:26] == 6'h0) && (inst[5:0] == slt_op )) ? 1'b1 : 1'b0;
assign sltu_flag = ((inst[31:26] == 6'h0) && (inst[5:0] == sltu_op)) ? 1'b1 : 1'b0;
assign sll_flag  = ((inst[31:26] == 6'h0) && (inst[5:0] == sll_op )) ? 1'b1 : 1'b0;
assign srl_flag  = ((inst[31:26] == 6'h0) && (inst[5:0] == srl_op )) ? 1'b1 : 1'b0;
assign sra_flag  = ((inst[31:26] == 6'h0) && (inst[5:0] == sra_op )) ? 1'b1 : 1'b0;
assign sllv_flag = ((inst[31:26] == 6'h0) && (inst[5:0] == sllv_op)) ? 1'b1 : 1'b0;
assign srlv_flag = ((inst[31:26] == 6'h0) && (inst[5:0] == srlv_op)) ? 1'b1 : 1'b0;
assign srav_flag = ((inst[31:26] == 6'h0) && (inst[5:0] == srav_op)) ? 1'b1 : 1'b0;
assign jr_flag   = ((inst[31:26] == 6'h0) && (inst[5:0] == jr_op  )) ? 1'b1 : 1'b0;

// I型指令inst[31,26]为opcode
assign addi_flag  = (inst[31:26] == addi_op ) ? 1'b1 : 1'b0;
assign addiu_flag = (inst[31:26] == addiu_op) ? 1'b1 : 1'b0;
assign andi_flag  = (inst[31:26] == andi_op ) ? 1'b1 : 1'b0;
assign ori_flag   = (inst[31:26] == ori_op  ) ? 1'b1 : 1'b0;
assign xori_flag  = (inst[31:26] == xori_op ) ? 1'b1 : 1'b0;
assign lw_flag    = (inst[31:26] == lw_op   ) ? 1'b1 : 1'b0;
assign sw_flag    = (inst[31:26] == sw_op   ) ? 1'b1 : 1'b0;
assign beq_flag   = (inst[31:26] == beq_op  ) ? 1'b1 : 1'b0;
assign bne_flag   = (inst[31:26] == bne_op  ) ? 1'b1 : 1'b0;
assign slti_flag  = (inst[31:26] == slti_op ) ? 1'b1 : 1'b0;
assign sltiu_flag = (inst[31:26] == sltiu_op) ? 1'b1 : 1'b0;
assign lui_flag   = (inst[31:26] == lui_op  ) ? 1'b1 : 1'b0;
assign j_flag     = (inst[31:26] == j_op    ) ? 1'b1 : 1'b0;
assign jal_flag   = (inst[31:26] == jal_op  ) ? 1'b1 : 1'b0;

// 23条额外指令
assign clz_flag     = ((inst[31:26] == 6'b011100) && (inst[5:0] == CLZ_OPE )) ? 1'b1 : 1'b0;
assign jalr_flag    = ((inst[31:26] == 6'h0) && (inst[5:0] == JALR_OPE )) ? 1'b1 : 1'b0;
assign mthi_flag    = ((inst[31:26] == 6'h0) && (inst[5:0] == MTHI_OPE )) ? 1'b1 : 1'b0;
assign mtlo_flag    = ((inst[31:26] == 6'h0) && (inst[5:0] == MTLO_OPE )) ? 1'b1 : 1'b0;
assign mfhi_flag    = ((inst[31:26] == 6'h0) && (inst[5:0] == MFHI_OPE )) ? 1'b1 : 1'b0;
assign mflo_flag    = ((inst[31:26] == 6'h0) && (inst[5:0] == MFLO_OPE )) ? 1'b1 : 1'b0;
assign sb_flag      = (inst[31:26] == SB_OPE  ) ? 1'b1 : 1'b0;
assign sh_flag      = (inst[31:26] == SH_OPE  ) ? 1'b1 : 1'b0;
assign lb_flag      = (inst[31:26] == LB_OPE  ) ? 1'b1 : 1'b0;
assign lh_flag      = (inst[31:26] == LH_OPE  ) ? 1'b1 : 1'b0;
assign lbu_flag     = (inst[31:26] == LBU_OPE ) ? 1'b1 : 1'b0;
assign lhu_flag     = (inst[31:26] == LHU_OPE ) ? 1'b1 : 1'b0;
assign eret_flag    = ((inst[31:26] == 6'b010000) && (inst[5:0] == ERET_OPE    )) ? 1'b1 : 1'b0;
assign break_flag   = ((inst[31:26] == 6'h0) && (inst[5:0] == BREAK_OPE   )) ? 1'b1 : 1'b0;
assign syscall_flag = ((inst[31:26] == 6'h0) && (inst[5:0] == SYSCALL_OPE )) ? 1'b1 : 1'b0;
assign teq_flag     = ((inst[31:26] == 6'h0) && (inst[5:0] == TEQ_OPE     )) ? 1'b1 : 1'b0;
assign mfc0_flag    = ((inst[31:26] == 6'b010000) && (inst[10:3] == 8'h0) && (inst[25:21] == MFC0_OPE)) ? 1'b1 : 1'b0;
assign mtc0_flag    = ((inst[31:26] == 6'b010000) && (inst[10:3] == 8'h0) && (inst[25:21] == MTC0_OPE)) ? 1'b1 : 1'b0;
assign mult_flag     = ((inst[31:26] == 6'b0) && (inst[5:0] == MULT_OPE     )) ? 1'b1 : 1'b0;
assign multu_flag   = ((inst[31:26] == 6'h0) && (inst[5:0] == MULTU_OPE   )) ? 1'b1 : 1'b0;
assign div_flag     = ((inst[31:26] == 6'h0) && (inst[5:0] == DIV_OPE     )) ? 1'b1 : 1'b0;
assign divu_flag    = ((inst[31:26] == 6'h0) && (inst[5:0] == DIVU_OPE    )) ? 1'b1 : 1'b0;
assign bgez_flag    = ((inst[31:26] == BGEZ_OPE) && (inst[20:16] == 5'b00001) ) ? 1'b1 : 1'b0;
endmodule