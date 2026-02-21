`include "mips_def.vh"
`timescale 1ns / 1ps

module Controller (
    input           branch,           // 分支信号
    input [31:0]    status,           // 状态寄存器
    input [31:0]    inst,            // 指令字
    output [2:0]    pc_sel,           // PC选择信号
    output          immed_sign,       // 立即数符号扩展控制
    output          ext5_sel,         // 5位扩展选择
    output          rs_rena,rt_rena,  // 寄存器读使能
    output          alu_a_sel,        // ALU输入A选择
    output [1:0]    alu_b_sel,        // ALU输入B选择
    output [3:0]    aluc,             // ALU操作控制
    output          mul_ena,div_ena,  // 乘除法使能
    output          mul_sign,div_sign,// 乘除法符号控制
    output          cutter_sign,      // 数据截取符号扩展
    output          cutter_addr_sel,  // 数据截取地址选择
    output [2:0]    cutter_sel,       // 数据截取选择
    output          dmem_ena,dmem_wena, // 数据存储器使能和写使能
    output [1:0]    dmem_wsel,        // 数据存储器写选择
    output [1:0]    dmem_rsel,        // 数据存储器读选择
    output          eret,             // 异常返回
    output [4:0]    cause,            // 异常原因
    output          exception,        // 异常信号
    output [4:0]    cp0_addr,         // CP0寄存器地址
    output          mfc0, mtc0,       // CP0读写控制
    output          hi_wena,lo_wena,  // HI/LO寄存器写使能
    output [1:0]    hi_sel,lo_sel,    // HI/LO寄存器输入选择
    output [2:0]    rd_sel,           // 寄存器写数据选择
    output [4:0]    rdc,              // 寄存器写地址
    output          rd_wena           // 寄存器写使能
    );

    // 指令译码标志位
    wire add_flag, addu_flag, sub_flag, subu_flag, and_flag, or_flag, xor_flag, nor_flag,
     slt_flag, sltu_flag, sll_flag, srl_flag, sra_flag, sllv_flag, srlv_flag, srav_flag,
     slti_flag, sltiu_flag, lui_flag, lw_flag, sw_flag, lb_flag, lbu_flag, lh_flag, lhu_flag, sb_flag, sh_flag,
     jr_flag, jalr_flag, mfhi_flag, mflo_flag, mthi_flag, mtlo_flag, multu_flag, div_flag, divu_flag,
     beq_flag, bne_flag, bgez_flag, j_flag, jal_flag, clz_flag, mul_flag, eret_flag, mfc0_flag, mtc0_flag,
     syscall_flag, break_flag, teq_flag, addi_flag, addiu_flag, andi_flag, ori_flag, xori_flag;

    // 指令译码器实例化
    Decoder u_decoder (
        .inst(inst),
        
        // R-type指令标志
        .add_flag(add_flag),
        .addu_flag(addu_flag),
        .sub_flag(sub_flag),
        .subu_flag(subu_flag),
        .and_flag(and_flag),
        .or_flag(or_flag),
        .xor_flag(xor_flag),
        .nor_flag(nor_flag),
        .slt_flag(slt_flag),
        .sltu_flag(sltu_flag),
        .sll_flag(sll_flag),
        .srl_flag(srl_flag),
        .sra_flag(sra_flag),
        .sllv_flag(sllv_flag),
        .srlv_flag(srlv_flag),
        .srav_flag(srav_flag),
        .jr_flag(jr_flag),
        .jalr_flag(jalr_flag),
        .mfhi_flag(mfhi_flag),
        .mflo_flag(mflo_flag),
        .mthi_flag(mthi_flag),
        .mtlo_flag(mtlo_flag),
        .multu_flag(multu_flag),
        .div_flag(div_flag),
        .divu_flag(divu_flag),
        .syscall_flag(syscall_flag),
        .break_flag(break_flag),
        .teq_flag(teq_flag),
        
        // I-type指令标志
        .addi_flag(addi_flag),
        .addiu_flag(addiu_flag),
        .andi_flag(andi_flag),
        .ori_flag(ori_flag),
        .xori_flag(xori_flag),
        .slti_flag(slti_flag),
        .sltiu_flag(sltiu_flag),
        .lui_flag(lui_flag),
        .lw_flag(lw_flag),
        .sw_flag(sw_flag),
        .lb_flag(lb_flag),
        .lbu_flag(lbu_flag),
        .lh_flag(lh_flag),
        .lhu_flag(lhu_flag),
        .sb_flag(sb_flag),
        .sh_flag(sh_flag),
        .beq_flag(beq_flag),
        .bne_flag(bne_flag),
        .bgez_flag(bgez_flag),
        .j_flag(j_flag),
        .jal_flag(jal_flag),
        
        // 其他指令标志
        .clz_flag(clz_flag),
        .mul_flag(mul_flag),
        .eret_flag(eret_flag),
        .mfc0_flag(mfc0_flag),
        .mtc0_flag(mtc0_flag)
    );
    
    // PC选择逻辑
    assign pc_sel[2] = (beq_flag & branch) | (bne_flag & branch) | (bgez_flag & branch) | eret_flag;
    assign pc_sel[1] = ~(j_flag | jr_flag | jal_flag | jalr_flag | (beq_flag & branch) | (bne_flag & branch) | (bgez_flag & branch) | eret_flag);
    assign pc_sel[0] = eret_flag | exception | jr_flag | jalr_flag;

    // HI/LO寄存器控制
    assign hi_wena   = mul_flag | multu_flag | div_flag | divu_flag | mthi_flag;
    assign hi_sel[1] = mthi_flag;
    assign hi_sel[0] = mul_flag | multu_flag;
    assign lo_wena   = mul_flag | multu_flag | div_flag | divu_flag | mtlo_flag; 
    assign lo_sel[1] = mtlo_flag;
    assign lo_sel[0] = mul_flag | multu_flag;
        
    // 立即数扩展控制
    assign ext5_sel     = sllv_flag | srav_flag | srlv_flag;
    assign immed_sign   = addi_flag | addiu_flag | sltiu_flag | slti_flag;

    // ALU控制信号生成
    assign aluc[3]      = lui_flag | srl_flag | slt_flag | sltu_flag | sllv_flag | srlv_flag | srav_flag | sra_flag | slti_flag | sltiu_flag | sll_flag;
    assign aluc[2]      = and_flag | or_flag | xor_flag | nor_flag | sll_flag | srl_flag | sra_flag | sllv_flag | srlv_flag | srav_flag | andi_flag | ori_flag | xori_flag;
    assign aluc[1]      = add_flag | sub_flag | xor_flag | nor_flag | slt_flag | sltu_flag | sll_flag | sllv_flag | addi_flag | xori_flag | beq_flag | bne_flag | slti_flag | sltiu_flag | bgez_flag | teq_flag;
    assign aluc[0]      = subu_flag | sub_flag | or_flag | nor_flag | slt_flag | sllv_flag | srlv_flag | sll_flag | srl_flag | slti_flag | ori_flag | beq_flag | bne_flag | bgez_flag | teq_flag;
    assign alu_a_sel    = ~(sll_flag | srl_flag | sra_flag | div_flag | divu_flag | mul_flag | multu_flag | j_flag | jr_flag | jal_flag | jalr_flag | mfc0_flag | mtc0_flag | mfhi_flag | mflo_flag | mthi_flag | mtlo_flag | clz_flag | eret_flag | syscall_flag | break_flag);
    assign alu_b_sel[1] = bgez_flag;
    assign alu_b_sel[0] = addi_flag | addiu_flag | andi_flag | ori_flag | xori_flag | slti_flag | sltiu_flag | lb_flag | lbu_flag | lh_flag | lhu_flag | lw_flag | sb_flag | sh_flag | sw_flag | lui_flag;
   
    // 乘除法控制
    assign mul_ena   = mul_flag | multu_flag;
    assign div_ena   = div_flag | divu_flag;
    assign mul_sign  = mul_flag;
    assign div_sign  = div_flag;

    // 数据截取控制（用于load/store字节/半字处理）
    assign cutter_addr_sel  = ~(sb_flag | sh_flag | sw_flag);
    assign cutter_sel[2]    = sh_flag;
    assign cutter_sel[1]    = lb_flag | lbu_flag | sb_flag;
    assign cutter_sel[0]    = lh_flag | lhu_flag | sb_flag;
    
    // 数据存储器控制
    assign dmem_ena     = lw_flag | sw_flag | lh_flag | sh_flag | lb_flag | sb_flag | lhu_flag | lbu_flag;
    assign dmem_wena    = sw_flag | sh_flag | sb_flag;
    assign dmem_wsel[1] = sh_flag | sb_flag;
    assign dmem_wsel[0] = sw_flag | sb_flag;
    assign dmem_rsel[1] = lh_flag | lb_flag | lhu_flag | lbu_flag;
    assign dmem_rsel[0] = lw_flag | lb_flag | lbu_flag;     
    assign cutter_sign  = lh_flag | lb_flag;  // 有符号扩展

    // 寄存器文件控制
    assign rd_wena   = addi_flag | addiu_flag | andi_flag | ori_flag | sltiu_flag | lui_flag | xori_flag | slti_flag | addu_flag | and_flag | xor_flag | nor_flag | or_flag | sll_flag | sllv_flag | sltu_flag | sra_flag | srl_flag | subu_flag | add_flag | sub_flag | slt_flag | srlv_flag | srav_flag | lb_flag | lbu_flag | lh_flag | lhu_flag | lw_flag | mfc0_flag | clz_flag | jal_flag | jalr_flag | mfhi_flag | mflo_flag | mul_flag;
    assign rdc = (add_flag | addu_flag | sub_flag | subu_flag | and_flag | or_flag | xor_flag | nor_flag | slt_flag | sltu_flag | sll_flag | srl_flag | sra_flag | sllv_flag | srlv_flag | srav_flag | clz_flag | jalr_flag | mfhi_flag | mflo_flag | mul_flag) ? 
                   inst[15:11] : (( addi_flag | addiu_flag | andi_flag | ori_flag | xori_flag | lb_flag | lbu_flag | lh_flag | lhu_flag | lw_flag | slti_flag | sltiu_flag | lui_flag | mfc0_flag) ? 
                   inst[20:16] : (jal_flag ? 5'd31 : 5'b0));
    assign rd_sel[2] = ~(beq_flag | bne_flag | bgez_flag | div_flag | divu_flag | sb_flag | multu_flag | sh_flag | sw_flag | j_flag | jr_flag | jal_flag | jalr_flag | mfc0_flag | mtc0_flag | mflo_flag | mthi_flag | mtlo_flag | clz_flag | eret_flag | syscall_flag | teq_flag | break_flag);
    assign rd_sel[1] = mul_flag | mfc0_flag | mtc0_flag | clz_flag | mfhi_flag;
    assign rd_sel[0] = ~(beq_flag | bne_flag | bgez_flag | div_flag | divu_flag | multu_flag | lb_flag | lbu_flag | lh_flag | lhu_flag | lw_flag | sb_flag | sh_flag | sw_flag | j_flag | mtc0_flag | mfhi_flag | mflo_flag | mthi_flag | mtlo_flag | clz_flag | eret_flag | syscall_flag | teq_flag | break_flag);
    assign rs_rena   = addi_flag | addiu_flag | andi_flag | ori_flag | sltiu_flag | xori_flag | slti_flag | addu_flag | and_flag | beq_flag | bne_flag | jr_flag | lw_flag | xor_flag | nor_flag | or_flag | sllv_flag | sltu_flag | subu_flag | sw_flag | add_flag | sub_flag | slt_flag | srlv_flag | srav_flag | clz_flag | divu_flag | jalr_flag | lb_flag | lbu_flag | lhu_flag | sb_flag | sh_flag | lh_flag | mul_flag | multu_flag | teq_flag | div_flag;
    assign rt_rena   = addu_flag | and_flag | beq_flag | bne_flag | xor_flag | nor_flag | or_flag | sll_flag | sllv_flag | sltu_flag | sra_flag | srl_flag | subu_flag | sw_flag | add_flag | sub_flag | slt_flag | srlv_flag | srav_flag | divu_flag | sb_flag | sh_flag | mtc0_flag | mul_flag | multu_flag | teq_flag | div_flag;
           
    // CP0控制信号
    assign mfc0  = mfc0_flag;
    assign mtc0  = mtc0_flag;
    
    // 异常处理逻辑
    assign cause        = break_flag ? `CAUSE_BREAK : (syscall_flag ? `CAUSE_SYSCALL : (teq_flag ? `CAUSE_TEQ : 5'bz));
    assign eret         = eret_flag; 
    assign cp0_addr     = inst[15:11];
    assign exception    = status[0] && ((syscall_flag && status[1]) || (break_flag && status[2]) || (teq_flag && status[3]));

endmodule