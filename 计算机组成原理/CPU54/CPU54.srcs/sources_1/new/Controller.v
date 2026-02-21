`timescale 1ns / 1ps
module Controller(       
    input clk,
    input rst,       
    input  [31:0] inst,
    input zero,                // ALU标志位ZF
    input overflow,            // ALU标志位
    input negative,            // ALU标志位
    input busy,
    output [4:0]  RsC,     
    output [4:0]  RtC,    
    output [4:0]  RdC,     
    output rf_w,              // 寄存器堆写信号
    output [3:0] aluc,         
    output [10:0] mux,         //m1(0),m2(1）,m3（2）,m4_1（3）,m4_2（4）,m5（5）,m6（6）,m7（7）,m8(8),m9(9),m10(10)
    output [2:0] m11,
    output [2:0] m12,
    output [3:0] m13,
    output m14,
    output [1:0] m15,
    output cs,                 // 高电平时DMEM启用，低电平时禁用
    output dm_r,               // DMEM写入
    output dm_w,               // DMEM读出
    output HI_we,
    output LO_we,
    output mfc0,
    output mtc0,
    output exception,
    output eret,
    output [4:0] cause,
//    output [4:0] CP0_Rd,
    output mult_start,
    output multu_start,
    output div_start,
    output divu_start,
    output sw_flag,
    output sb_flag,
    output sh_flag,
    output lb_flag,              //当前读取指令是否是LB发出
    output lh_flag,              //当前读取指令是否是LH发出
    output lbu_flag,             //当前读取指令是否是LBU发出
    output lhu_flag,             //当前读取指令是否是LHU发出
    output lw_flag,              //当前读取指令是否是LW发出
    output [31:0] ext5_out,
    output [31:0] ext16_out,
    output signed [31:0] s_ext16_out,
    output signed [31:0] s_ext18_out,
    output [25:0] index
    );
wire add_flag,  addu_flag, sub_flag, subu_flag,
      and_flag, or_flag, xor_flag, nor_flag,
      slt_flag,  sltu_flag,
      sll_flag,  srl_flag,  sra_flag, 
      sllv_flag, srlv_flag, srav_flag,
      jr_flag,
      addi_flag, addiu_flag,
      andi_flag, ori_flag,  xori_flag,
      beq_flag,  bne_flag,
      slti_flag, sltiu_flag,
      lui_flag,
      j_flag,    jal_flag,
     // 23条新增信号
      clz_flag, jalr_flag, mthi_flag, mtlo_flag,
      mfhi_flag, mflo_flag,
      eret_flag, break_flag, syscall_flag, teq_flag,
      mfc0_flag, mtc0_flag, mult_flag, multu_flag,
      div_flag, divu_flag, bgez_flag;
      
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
    .jal_flag(jal_flag),
    .clz_flag(clz_flag),
    .jalr_flag(jalr_flag),
    .mthi_flag(mthi_flag),
    .mtlo_flag(mtlo_flag),
    .mfhi_flag(mfhi_flag),
    .mflo_flag(mflo_flag),
    .sb_flag(sb_flag),
    .sh_flag(sh_flag),
    .lb_flag(lb_flag),
    .lh_flag(lh_flag),
    .lbu_flag(lbu_flag),
    .lhu_flag(lhu_flag),
    .eret_flag(eret_flag),
    .break_flag(break_flag),
    .syscall_flag(syscall_flag),
    .teq_flag(teq_flag),
    .mfc0_flag(mfc0_flag),
    .mtc0_flag(mtc0_flag),
    .mult_flag(mult_flag),
    .multu_flag(multu_flag),
    .div_flag(div_flag),
    .divu_flag(divu_flag),
    .bgez_flag(bgez_flag)
    );


assign RsC = (add_flag  || addu_flag || sub_flag  || subu_flag  ||
              and_flag  || or_flag   || xor_flag  || nor_flag   ||
              slt_flag  || sltu_flag || sllv_flag || srlv_flag  ||
              srav_flag || jr_flag   || addi_flag || addiu_flag ||
              andi_flag || ori_flag  || xori_flag || lw_flag    ||
              sw_flag   || beq_flag  || bne_flag  || slti_flag  ||
              sltiu_flag||
                            // 新增信号开始
              mult_flag  || multu_flag || div_flag || divu_flag ||
              lb_flag   || lbu_flag  || lh_flag  || lhu_flag   ||
              sb_flag   || sh_flag   || bgez_flag|| jalr_flag  ||
              teq_flag  || clz_flag  || mtlo_flag || mthi_flag) ? inst[25:21] : 5'hz;

assign RtC = (add_flag  || addu_flag  || sub_flag   || subu_flag ||
              and_flag  || or_flag    || xor_flag   || nor_flag  ||
              slt_flag  || sltu_flag  || sll_flag   || srl_flag  ||
              sra_flag  || sllv_flag  || srlv_flag  || srav_flag ||
              sw_flag   || beq_flag   || bne_flag   ||
                            // 新增信号开始
              mult_flag  || multu_flag || div_flag   || divu_flag ||
              sb_flag   || sh_flag    || bgez_flag  || teq_flag  || 
              mtc0_flag) ? inst[20:16] : 5'hz;

assign RdC = (add_flag  || addu_flag  || sub_flag  || subu_flag  ||
              and_flag  || or_flag    || xor_flag  || nor_flag   ||
              slt_flag  || sltu_flag  || sll_flag  || srl_flag   ||
              sra_flag  || sllv_flag  || srlv_flag || srav_flag  ||
              // 新增第一部分信号
              mflo_flag || mfhi_flag  || clz_flag) ? inst[15:11] : ((
              addi_flag || addiu_flag || andi_flag || ori_flag   || 
              xori_flag || lw_flag    || lui_flag  || slti_flag  ||
              sltiu_flag || 
              // 新增第二部分信号
              lb_flag   || mfc0_flag || lbu_flag   || lh_flag   || lhu_flag) ? inst[20:16] : (jal_flag ? 5'd31 : (jalr_flag ? 5'd31 : 5'b0)));

assign rf_w = (!(jr_flag || sw_flag || beq_flag || bne_flag || j_flag || 
                ((add_flag || addi_flag || sub_flag) && overflow)    ||
                mult_flag || multu_flag || div_flag || divu_flag || 
                mtlo_flag || mthi_flag || sb_flag || sh_flag || bgez_flag || 
                break_flag || syscall_flag || teq_flag || eret_flag || mtc0_flag
                    )) ? 1'b1 : 1'b0;

assign aluc[3] = (slt_flag  || sltu_flag  || sll_flag  || srl_flag  ||
                  sra_flag  || sllv_flag  || srlv_flag || srav_flag ||
                  slti_flag || sltiu_flag || lui_flag) ? 1'b1 : 1'b0;
assign aluc[2] = (and_flag  || or_flag    || xor_flag  || nor_flag  ||
                  sll_flag  || srl_flag   || sra_flag  || sllv_flag ||
                  srlv_flag  || srav_flag || andi_flag || ori_flag  ||
                  xori_flag) ? 1'b1 : 1'b0;
assign aluc[1] = (add_flag  || sub_flag   || xor_flag  || nor_flag  ||
                  slt_flag  || sltu_flag  || sll_flag  || sllv_flag ||
                  addi_flag || xori_flag  || slti_flag || sltiu_flag ||
                  teq_flag) ? 1'b1 : 1'b0;
assign aluc[0] = (sub_flag  || subu_flag  || or_flag   || nor_flag  ||
                  slt_flag  || srl_flag   || srlv_flag || ori_flag  ||
                  beq_flag  || bne_flag   || slti_flag || bgez_flag ||
                  teq_flag) ? 1'b1 : 1'b0;

assign mux[0] = !(j_flag || jal_flag) ? 1'b1 : 1'b0; // m1
assign mux[1] = !lw_flag ? 1'b1 : 1'b0; // m2
assign mux[2] = !(sll_flag || srl_flag || sra_flag) ? 1'b1 : 1'b0; // m3
assign mux[3] = (andi_flag  || ori_flag  || xori_flag || lui_flag) ? 1'b1 : 1'b0; // m4_0
assign mux[4] = (addi_flag  || addiu_flag || lw_flag  || sw_flag    ||
                 slti_flag  || sltiu_flag ||
                 //新增信号开始
                 lb_flag || lbu_flag || lh_flag || lhu_flag || sb_flag || sh_flag
                 ) ? 1'b1 : 1'b0; // m4_1
assign mux[5] = ((beq_flag && zero) || (bne_flag && !zero) || (bgez_flag && !negative)) ? 1'b1 : 1'b0; // m5
assign mux[6] = (addi_flag  || addiu_flag || andi_flag || ori_flag  ||
                xori_flag   || lw_flag    || lui_flag  ||
                //新增信号
                lb_flag || lbu_flag || lh_flag || lhu_flag) ? 1'b1 : 1'b0; // m6
assign mux[7] = !(jr_flag || jalr_flag) ? 1'b1 : 1'b0;
//assign mux[8] = (slt_flag   || sltu_flag  || slti_flag || sltiu_flag) ? 1'b1 : 1'b0;
assign mux[8] = 0;
assign mux[9] = (jal_flag || jalr_flag) ? 1'b1 : 1'b0;
assign mux[10] = (slti_flag || sltiu_flag) ? 1'b1 : 1'b0;

//新增mux
// M11: 3位信号
assign m11[2] = mthi_flag;              
assign m11[1] = div_flag || divu_flag;    
assign m11[0] = multu_flag || divu_flag; 

// M12: 3位信号
assign m12[2] = mtlo_flag;             
assign m12[1] = div_flag || divu_flag;  
assign m12[0] = multu_flag || divu_flag;

// M13: 4位信号
assign m13[3] = clz_flag;              
assign m13[2] = lbu_flag || lh_flag || lhu_flag || mfc0_flag; 
assign m13[1] = mflo_flag || lb_flag || lhu_flag || mfc0_flag;
assign m13[0] = mfhi_flag || lb_flag || lh_flag || mfc0_flag; 

// M14: 1位信号
assign m14 = bgez_flag;                 

// M15: 2位信号
assign m15[1] = eret_flag;               
assign m15[0] = break_flag || syscall_flag || teq_flag;


assign cs = (lw_flag || sw_flag || lb_flag || lbu_flag || 
            lh_flag || lhu_flag || sb_flag || sh_flag) ? 1'b1 : 1'b0;
assign dm_r = (lw_flag || lb_flag || lbu_flag || 
                lh_flag || lhu_flag) ? 1'b1 : 1'b0;
assign dm_w = (sw_flag || sb_flag || sh_flag) ? 1'b1 : 1'b0;  

assign HI_we = (mult_flag || multu_flag || div_flag || divu_flag || mthi_flag) ? 1'b1 : 1'b0;
assign LO_we =(mult_flag || multu_flag || div_flag || divu_flag || mtlo_flag) ? 1'b1 : 1'b0;

assign mfc0 = mfc0_flag;
assign mtc0 = mtc0_flag;
assign exception = (break_flag || syscall_flag || teq_flag) ? 1'b1 : 1'b0;
assign eret = eret_flag;

assign cause = break_flag ? 5'b01001 :
                  syscall_flag ? 5'b01000 :
                  teq_flag ? 5'b01101 :                 
                  5'b0; // 加上默认值                  
//assign CP0_Rd = (mfc0_flag || mtc0_flag) ? inst[15:11] : 5'hz;

//assign mult_start = mult_flag && !busy;
//assign multu_start = multu_flag && !busy;
//assign div_start = div_flag && !busy;
//assign divu_start = divu_flag && !busy;
reg multu_done;
always @(negedge clk or posedge rst) begin
    if (rst) begin
        multu_done <= 0;
    end else if (!busy && multu_flag && !multu_done) begin
        multu_done <= 1;
    end else if (!busy && multu_done) begin
        multu_done <= 0; // 等下一条指令来时再拉高
    end
end
assign multu_start = multu_flag && !busy && !multu_done;

reg mult_done;
always @(negedge clk or posedge rst) begin
    if (rst) begin
        mult_done <= 0;
    end else if (!busy && mult_flag && !mult_done) begin
        mult_done <= 1;
    end else if (!busy && mult_done) begin
        mult_done <= 0; // 等下一条指令来时再拉高
    end
end
assign mult_start = mult_flag && !busy && !mult_done;

reg div_done;
always @(negedge clk or posedge rst) begin
    if (rst) begin
        div_done <= 0;
    end else if (!busy && div_flag && !div_done) begin
        div_done <= 1;
    end else if (!busy && div_done) begin
        div_done <= 0; // 等下一条指令来时再拉高
    end
end
assign div_start = div_flag && !busy && !div_done;

reg divu_done;
always @(negedge clk or posedge rst) begin
    if (rst) begin
        divu_done <= 0;
    end else if (!busy && divu_flag && !divu_done) begin
        divu_done <= 1;
    end else if (!busy && divu_done) begin
        divu_done <= 0; // 等下一条指令来时再拉高
    end
end
assign divu_start = divu_flag && !busy && !divu_done;

wire [4:0] sa;        // SLL，SRL，SRA用
wire [15:0] imm;

assign sa =  inst[10:6];
assign imm = inst[15:0];
assign index = inst[25:0];

assign ext5_out = sa;
assign ext16_out = imm;
assign s_ext16_out = {{16{imm[15]}} , imm[15:0]};
assign s_ext18_out = {{14{imm[15]}}, imm[15:0], 2'b0};

endmodule