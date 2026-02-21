`include "mips_def.vh"
`timescale 1ns / 1ps

module Decoder (
    input [31:0] inst,  // 32位指令输入

    // R-type指令标志输出
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
    output jalr_flag,
    output mfhi_flag,
    output mflo_flag,
    output mthi_flag,
    output mtlo_flag,
    output multu_flag,
    output div_flag,
    output divu_flag,
    output syscall_flag,
    output break_flag,
    output teq_flag,

    // I-type指令标志输出
    output addi_flag,
    output addiu_flag,
    output andi_flag,
    output ori_flag,
    output xori_flag,
    output slti_flag,
    output sltiu_flag,
    output lui_flag,
    output lw_flag,
    output sw_flag,
    output lb_flag,
    output lbu_flag,
    output lh_flag,
    output lhu_flag,
    output sb_flag,
    output sh_flag,
    output beq_flag,
    output bne_flag,
    output bgez_flag,
    output j_flag,
    output jal_flag,

    // 其他特殊指令标志输出
    output clz_flag,
    output mul_flag,
    output eret_flag,
    output mfc0_flag,
    output mtc0_flag
);

    wire [5:0] op = inst[31:26];    // 操作码字段
    wire [5:0] func = inst[5:0];    // 功能码字段

    // R-type指令解码（操作码全为0，通过功能码区分）
    assign add_flag    = (op == `OP_ADD)   && (func == `FUNC_ADD);
    assign addu_flag   = (op == `OP_ADDU)  && (func == `FUNC_ADDU);
    assign sub_flag    = (op == `OP_SUB)   && (func == `FUNC_SUB);
    assign subu_flag   = (op == `OP_SUBU)  && (func == `FUNC_SUBU);
    assign and_flag    = (op == `OP_AND)   && (func == `FUNC_AND);
    assign or_flag     = (op == `OP_OR)    && (func == `FUNC_OR);
    assign xor_flag    = (op == `OP_XOR)   && (func == `FUNC_XOR);
    assign nor_flag    = (op == `OP_NOR)   && (func == `FUNC_NOR);
    assign slt_flag    = (op == `OP_SLT)   && (func == `FUNC_SLT);
    assign sltu_flag   = (op == `OP_SLTU)  && (func == `FUNC_SLTU);
    assign sll_flag    = (op == `OP_SLL)   && (func == `FUNC_SLL);
    assign srl_flag    = (op == `OP_SRL)   && (func == `FUNC_SRL);
    assign sra_flag    = (op == `OP_SRA)   && (func == `FUNC_SRA);
    assign sllv_flag   = (op == `OP_SLLV)  && (func == `FUNC_SLLV);
    assign srlv_flag   = (op == `OP_SRLV)  && (func == `FUNC_SRLV);
    assign srav_flag   = (op == `OP_SRAV)  && (func == `FUNC_SRAV);
    assign jr_flag     = (op == `OP_JR)    && (func == `FUNC_JR);
    assign jalr_flag   = (op == `OP_JALR)  && (func == `FUNC_JALR);
    assign mfhi_flag   = (op == `OP_MFHI)  && (func == `FUNC_MFHI);
    assign mflo_flag   = (op == `OP_MFLO)  && (func == `FUNC_MFLO);
    assign mthi_flag   = (op == `OP_MTHI)  && (func == `FUNC_MTHI);
    assign mtlo_flag   = (op == `OP_MTLO)  && (func == `FUNC_MTLO);
    assign multu_flag  = (op == `OP_MULTU) && (func == `FUNC_MULTU);
    assign div_flag    = (op == `OP_DIV)   && (func == `FUNC_DIV);
    assign divu_flag   = (op == `OP_DIVU)  && (func == `FUNC_DIVU);
    assign syscall_flag= (op == `OP_SYSCALL) && (func == `FUNC_SYSCALL);
    assign break_flag  = (op == `OP_BREAK) && (func == `FUNC_BREAK);
    assign teq_flag    = (op == `OP_TEQ)   && (func == `FUNC_TEQ);

    // I-type指令解码（通过操作码区分，立即数指令）
    assign addi_flag   = (op == `OP_ADDI);
    assign addiu_flag  = (op == `OP_ADDIU);
    assign andi_flag   = (op == `OP_ANDI);
    assign ori_flag    = (op == `OP_ORI);
    assign xori_flag   = (op == `OP_XORI);
    assign slti_flag   = (op == `OP_SLTI);
    assign sltiu_flag  = (op == `OP_SLTIU);
    assign lui_flag    = (op == `OP_LUI);
    assign lw_flag     = (op == `OP_LW);
    assign sw_flag     = (op == `OP_SW);
    assign lb_flag     = (op == `OP_LB);
    assign lbu_flag    = (op == `OP_LBU);
    assign lh_flag     = (op == `OP_LH);
    assign lhu_flag    = (op == `OP_LHU);
    assign sb_flag     = (op == `OP_SB);
    assign sh_flag     = (op == `OP_SH);
    assign beq_flag    = (op == `OP_BEQ);
    assign bne_flag    = (op == `OP_BNE);
    assign bgez_flag   = (op == `OP_BGEZ) && (inst[20:16] == 5'b00001);  // rt字段为1
    assign j_flag      = (op == `OP_J);
    assign jal_flag    = (op == `OP_JAL);

    // 其他特殊指令解码
    assign clz_flag    = (op == `OP_CLZ) && (func == `FUNC_CLZ);
    assign mul_flag    = (op == `OP_MUL) && (func == `FUNC_MUL);
    assign eret_flag   = (op == `OP_ERET) && (func == `FUNC_ERET);
    assign mfc0_flag   = (inst[31:21] == 11'b01000000000) && (inst[10:3] == 8'b0);  // CP0读指令
    assign mtc0_flag   = (inst[31:21] == 11'b01000000100) && (inst[10:3] == 8'b0);  // CP0写指令

endmodule