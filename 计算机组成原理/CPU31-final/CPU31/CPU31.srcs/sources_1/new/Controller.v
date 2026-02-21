`timescale 1ns / 1ps
module Controller(              
    input  [31:0] inst,
    input zero,                // ALU标志位ZF
    input overflow,            // ALU标志位
    output [4:0]  RsC,     
    output [4:0]  RtC,    
    output [4:0]  RdC,     
    output rf_w,              // 寄存器堆写信号
    output [3:0] aluc,         
    output [10:0] mux,         //m1(0),m2(1）,m3（2）,m4_1（3）,m4_2（4）,m5（5）,m6（6）,m7（7）,m8(8),m9(9),m10(10)
    output cs,                 // 高电平时DMEM启用，低电平时禁用
    output dm_r,               // DMEM写入
    output dm_w               // DMEM读出
    );
wire add_flag,  addu_flag, sub_flag, subu_flag,
      and_flag, or_flag, xor_flag, nor_flag,
      slt_flag,  sltu_flag,
      sll_flag,  srl_flag,  sra_flag, 
      sllv_flag, srlv_flag, srav_flag,
      jr_flag,
      addi_flag, addiu_flag,
      andi_flag, ori_flag,  xori_flag,
      lw_flag,   sw_flag,
      beq_flag,  bne_flag,
      slti_flag, sltiu_flag,
      lui_flag,
      j_flag,    jal_flag;
      
Decoder Decoder_inst(
    .inst(inst), 
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
    .addi_flag(addi_flag), 
    .addiu_flag(addiu_flag),
    .andi_flag(andi_flag), 
    .ori_flag(ori_flag),   
    .xori_flag(xori_flag),  
    .lw_flag(lw_flag),     
    .sw_flag(sw_flag),    
    .beq_flag(beq_flag),   
    .bne_flag(bne_flag),    
    .slti_flag(slti_flag), 
    .sltiu_flag(sltiu_flag), 
    .lui_flag(lui_flag),      
    .j_flag(j_flag),     
    .jal_flag(jal_flag)
    );


assign RsC = (add_flag  || addu_flag || sub_flag  || subu_flag  ||
              and_flag  || or_flag   || xor_flag  || nor_flag   ||
              slt_flag  || sltu_flag || sllv_flag || srlv_flag  ||
              srav_flag || jr_flag   || addi_flag || addiu_flag ||
              andi_flag || ori_flag  || xori_flag || lw_flag    ||
              sw_flag   || beq_flag  || bne_flag  || slti_flag  ||
              sltiu_flag) ? inst[25:21] : 5'hz;

assign RtC = (add_flag  || addu_flag  || sub_flag   || subu_flag ||
              and_flag  || or_flag    || xor_flag   || nor_flag  ||
              slt_flag  || sltu_flag  || sll_flag   || srl_flag  ||
              sra_flag  || sllv_flag  || srlv_flag  || srav_flag ||
              sw_flag   || beq_flag   || bne_flag ) ? inst[20:16] : 5'hz;

assign RdC = (add_flag  || addu_flag  || sub_flag  || subu_flag  ||
              and_flag  || or_flag    || xor_flag  || nor_flag   ||
              slt_flag  || sltu_flag  || sll_flag  || srl_flag   ||
              sra_flag  || sllv_flag  || srlv_flag || srav_flag) ? inst[15:11] : ((
              addi_flag || addiu_flag || andi_flag || ori_flag   || 
              xori_flag || lw_flag    || lui_flag  || slti_flag  ||
              sltiu_flag) ? inst[20:16] : (jal_flag ? 5'd31 : 5'hz));

assign rf_w = (!(jr_flag || sw_flag || beq_flag || bne_flag || j_flag || 
                ((add_flag || addi_flag || sub_flag) && overflow))) ? 1'b1 : 1'b0;

assign aluc[3] = (slt_flag  || sltu_flag  || sll_flag  || srl_flag  ||
                  sra_flag  || sllv_flag  || srlv_flag || srav_flag ||
                  slti_flag || sltiu_flag || lui_flag) ? 1'b1 : 1'b0;
assign aluc[2] = (and_flag  || or_flag    || xor_flag  || nor_flag  ||
                  sll_flag  || srl_flag   || sra_flag  || sllv_flag ||
                  srlv_flag  || srav_flag || andi_flag || ori_flag  ||
                  xori_flag) ? 1'b1 : 1'b0;
assign aluc[1] = (add_flag  || sub_flag   || xor_flag  || nor_flag  ||
                  slt_flag  || sltu_flag  || sll_flag  || sllv_flag ||
                  addi_flag || xori_flag  || slti_flag || sltiu_flag) ? 1'b1 : 1'b0;
assign aluc[0] = (sub_flag  || subu_flag  || or_flag   || nor_flag  ||
                  slt_flag  || srl_flag   || srlv_flag || ori_flag  ||
                  beq_flag  || bne_flag   || slti_flag) ? 1'b1 : 1'b0;

assign mux[0] = !(j_flag || jal_flag) ? 1'b1 : 1'b0; // m1
assign mux[1] = !lw_flag ? 1'b1 : 1'b0; // m2
assign mux[2] = !(sll_flag || srl_flag || sra_flag) ? 1'b1 : 1'b0; // m3
assign mux[3] = (andi_flag  || ori_flag  || xori_flag || lui_flag) ? 1'b1 : 1'b0; // m4_0
assign mux[4] = (addi_flag  || addiu_flag || lw_flag  || sw_flag    ||
                 slti_flag  || sltiu_flag) ? 1'b1 : 1'b0; // m4_1
assign mux[5] = ((beq_flag && zero) || (bne_flag && !zero)) ? 1'b1 : 1'b0; // m5
assign mux[6] = (addi_flag  || addiu_flag || andi_flag || ori_flag  ||
                xori_flag   || lw_flag    || lui_flag) ? 1'b1 : 1'b0; // m6
assign mux[7] = !jr_flag ? 1'b1 : 1'b0;
//assign mux[8] = (slt_flag   || sltu_flag  || slti_flag || sltiu_flag) ? 1'b1 : 1'b0;
assign mux[8] = 0;
assign mux[9] = jal_flag ? 1'b1 : 1'b0;
assign mux[10] = (slti_flag || sltiu_flag) ? 1'b1 : 1'b0;

assign cs = (lw_flag || sw_flag) ? 1'b1 : 1'b0;
assign dm_r = lw_flag ? 1'b1 : 1'b0;
assign dm_w = sw_flag ? 1'b1 : 1'b0;  

endmodule