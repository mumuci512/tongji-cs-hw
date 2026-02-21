`timescale 1ns / 1ps
module cpu(
    input clk,                  //CPU执行时钟
    input rst,               //复位信号
    input [31:0] inst,              // 本次执行的指令，由IMEM输出
    output [31:0] imem_in,          // imem的输入
    output cs,
    output dm_w,
    output dm_r,
    output [31:0] dm_addr,
    output [31:0] dm_data_w,
    input [31:0] dm_data_r,
    output [31:0] pc_out,          // 本次执行指令的地址，由pc输出
    output busy,
    output sw_flag,
    output sb_flag,
    output sh_flag,
    output lb_flag,             //当前读取指令是否是LB发出
    output lh_flag,             //当前读取指令是否是LH发出
    output lbu_flag,            //当前读取指令是否是LBU发出
    output lhu_flag,            //当前读取指令是否是LHU发出
    output lw_flag              //当前读取指令是否是LW发出
    );
/* ------------信号定义------------ */
/* 控制信号，即Controller的输出 */
wire [4:0] RsC;                 //Rs对应的寄存器的地址
wire [4:0] RtC;                 //Rt对应的寄存器的地址
wire [4:0] RdC;                 //Rd对应的寄存器的地址
wire rf_w;                     //RegFile寄存器堆是否可写入
wire [3:0]  aluc; 
wire [10:0] mux;  
wire [2:0] m11;
wire [2:0] m12;
wire [3:0] m13;
wire m14;
wire [1:0] m15;
    
//wire cs;
//wire dm_w;
//wire dm_r;

/* 其他部件输入输出 */
// MUX
wire [31:0] mux1_out;
wire [31:0] mux2_out;
wire [31:0] mux3_out;
wire [31:0] mux4_out;
wire [31:0] mux5_out;
wire [4:0] mux6_out;
wire [31:0] mux7_out;
wire [31:0] mux8_out;
wire [31:0] mux9_out;
wire mux10_out;
wire [31:0] mux11_out;
wire [31:0] mux12_out;
wire [31:0] mux13_out;
wire [31:0] mux14_out;
wire [31:0] mux15_out;

// NPC
wire [31:0] npc;

// PC
wire [31:0] pc_in;     // 本次输入PC寄存器的指令地址，也就是下一次要执行的指令

// IMEM
//wire [31:0] imem_in;

// RegFile
wire [31:0] Rd_data_in;     //要向寄存器中写入的值
wire [31:0] Rs_data_out;    //Rs对应的寄存器的输出值
wire [31:0] Rt_data_out;    //Rt对应的寄存器的输出值

// ALU
wire [31:0] a;
wire [31:0] b;
wire [31:0] alu_out;               //ALU输出的数据
wire zero, carry, negative, overflow;   //四个标志位

// DMEM
//wire [31:0] dm_addr;
//wire [31:0] dm_data_w;
//wire [31:0] dm_data_r;

//wire busy;
wire start;
wire stall;
// MULT
wire [31:0] mult_a;
wire [31:0] mult_b;
wire [63:0] mult_out;
wire mult_start;
wire mult_busy;
// MULTU
wire [31:0] multu_a;
wire [31:0] multu_b;
wire [63:0] multu_out;
wire multu_start;
wire multu_busy;
// DIV
wire [31:0] div_a;
wire [31:0] div_b;
wire [31:0] div_r;
wire [31:0] div_q;
wire div_start;
wire div_busy;
// DIVU
wire [31:0] divu_a;
wire [31:0] divu_b;
wire [31:0] divu_r;
wire [31:0] divu_q;
wire divu_start;
wire divu_busy;

// HI_LO
wire [31:0] HI_in;
wire [31:0] LO_in;
wire HI_we; //写使能信号
wire LO_we; //写使能信号
wire [31:0] HI_out;
wire [31:0] LO_out;

//CLZ
wire [31:0] clz_in;
wire [31:0] clz_out;

// CP0
wire mfc0; // CPU指令Mfc0 
wire mtc0; // CPU指令Mtc0 
wire [4:0] CP0_Rd; // 指定Cp0寄存器
wire [31:0] CP0_wdata; // 数据从GP寄存器到CP0寄存器
wire exception; 
wire eret; // 指令ERET (Exception Return) 
wire [4:0]cause; 
wire CP0_intr;  // 外设中断请求
wire [31:0] CP0_rdata; //数据从 CP0寄存器到GP寄存器
wire [31:0] status; 
wire timer_int;  // 定时产生中断，定时器的中断输出，现在不做了
wire [31:0]exc_addr;// 异常起始地址

/* 无需控制信号的部件固定输出值 */
//wire [4:0] sa;        // SLL，SRL，SRA用
//wire [15:0] imm;
wire [25:0] index;     // 跳转地址（J,JAL用）

// ext部件
//wire [31:0] ext1_out;
wire [31:0] ext5_out;
wire [31:0] ext16_out;
wire signed [31:0] s_ext16_out;
wire signed [31:0] s_ext18_out;

//wire signed [31:0] s_ext8_out;
//wire [31:0] ext8_out;
//wire signed [31:0] s_ext16_out_2;
//wire [31:0] ext16_out_2;

// ||部件
wire [31:0] cat_out;

/* ------------信号赋值------------ */
// MUX
assign mux1_out = mux[0] ? mux5_out : cat_out;
assign mux2_out = mux[1] ? mux8_out : dm_data_r;
assign mux3_out = mux[2] ? Rs_data_out : ext5_out;
assign mux4_out = (mux[4:3] == 2'b00) ? Rt_data_out :
                  (mux[4:3] == 2'b01) ? ext16_out :
                  (mux[4:3] == 2'b10) ? s_ext16_out :
                  (mux[4:3] == 2'b11) ? s_ext18_out :
                  32'b0; // 加上默认值
assign mux5_out = mux[5] ? s_ext18_out + npc : npc;
assign mux6_out = mux[6] ? inst[20:16] : inst[15:11]; // 其实不需要,因为Rc的输入已经在Controller中筛选完成
assign mux7_out = mux[7] ? mux1_out : Rs_data_out;
//assign mux8_out = mux[8] ? ext1_out : alu_out;
assign mux9_out = mux[9] ? pc_out + 4 : mux2_out;
assign mux10_out = mux[10] ? carry : negative;

assign mux11_out = (m11 == 3'b000) ? mult_out[63:32] :
                  (m11 == 3'b001) ? multu_out[63:32] :
                  (m11 == 3'b010) ? div_r :
                  (m11 == 3'b011) ? divu_r :
                  (m11 == 3'b100) ? Rs_data_out :                  
                  32'b0; // 加上默认值
assign mux12_out = (m12 == 3'b000) ? mult_out[31:0] :
                  (m12 == 3'b001) ? multu_out[31:0] :
                  (m12 == 3'b010) ? div_q :
                  (m12 == 3'b011) ? divu_q :
                  (m12 == 3'b100) ? Rs_data_out :                  
                  32'b0; // 加上默认值
assign mux13_out = (m13 == 4'b0000) ? mux9_out :
                  (m13 == 4'b0001) ? HI_out :
                  (m13 == 4'b0010) ? LO_out :
                  (m13 == 4'b0011) ? dm_data_r :
                  (m13 == 4'b0100) ? dm_data_r :                  
                  (m13 == 4'b0101) ? dm_data_r :                  
                  (m13 == 4'b0110) ? dm_data_r :                  
                  (m13 == 4'b0111) ? CP0_rdata :                  
                  (m13 == 4'b1000) ? clz_out :                  
                  32'b0; // 加上默认值
assign mux14_out = m14 ? 32'b0 : mux4_out;
assign mux15_out = (m15 == 3'b00) ? mux7_out :
                  (m15 == 3'b01) ? exc_addr :
                  (m15 == 3'b10) ? CP0_rdata :                 
                  32'b0; // 加上默认值
                  
// NPC
assign npc = pc_out + 4;

// PC
//assign pc_in = mux15_out;
// 将关键路径拆分为两级流水
reg [31:0] pc_next_stage1;
reg [31:0] pc_next_stage2;

always @(negedge clk) begin
    // 第一级：计算mux7_out和exc_addr
    if (m15[1:0] == 2'b00) 
        pc_next_stage1 <= mux7_out;
    else if (m15[1:0] == 2'b01)
        pc_next_stage1 <= exc_addr;
    else 
        pc_next_stage1 <= CP0_rdata;
        
    // 第二级：最终选择
    pc_next_stage2 <= pc_next_stage1;
end

assign pc_in = pc_next_stage2;  // 使用两级流水后的结果

// IMEM
//assign imem_in = (pc_out - 32'h00400000) >> 2; // 除以4得到字地址
    // 新增寄存器
reg [29:0] imem_addr_reg;
reg [31:0] inst_reg;
// 预先计算的常量
parameter IMEM_BASE = 32'h00400000 >> 2;  // 综合时计算
always @(negedge clk) begin
    imem_addr_reg <= pc_out[31:2] - IMEM_BASE[29:0];  // 仅用高位计算
end
assign imem_in = imem_addr_reg;

// RegFile
assign Rd_data_in = mux13_out;

// ALU
//assign a = mux3_out;
//assign b = mux14_out; 
// 增加操作数寄存器
reg [31:0] alu_a_reg;
reg [31:0] alu_b_reg;
reg [3:0] aluc_reg;

always @(negedge clk) begin
    alu_a_reg <= mux3_out;
    alu_b_reg <= mux14_out;
    aluc_reg <= aluc;  // 同时寄存控制信号
end


// DMEM
assign dm_addr = (alu_out -  32'h10010000);
assign dm_data_w = Rt_data_out;

// MULT
assign mult_a = Rs_data_out;
assign mult_b = Rt_data_out;
// MULTU
assign multu_a = Rs_data_out;
assign multu_b = Rt_data_out;
// DIV
assign div_a = Rs_data_out;
assign div_b = Rt_data_out;
// DIVU
assign divu_a = Rs_data_out;
assign divu_b = Rt_data_out;

assign busy = mult_busy || multu_busy || div_busy || divu_busy;
assign start = mult_start || multu_start || div_start || divu_start;
assign stall = start || busy;

// HI_LO
assign HI_in = mux11_out;
assign LO_in = mux12_out;

//CLZ
assign clz_in = Rs_data_out;

// CP0
assign  CP0_wdata = Rt_data_out; // 数据从GP寄存器到CP0寄存器
assign CP0_intr = 0;  // 外设中断请求
assign CP0_Rd = inst[15:11];

/* 无需控制信号的部件固定输出值 */
//assign sa =  inst[10:6];
//assign imm = inst[15:0];
//assign index = inst[25:0];

// ext部件
//assign ext1_out = mux10_out;


// 字节选择逻辑
//wire [1:0] sel = alu_out[1:0]; // 内存地址低2位

//wire [7:0] selected_byte;
//assign selected_byte = 
//    (sel == 2'b00) ? dm_data_r[7:0] :
//    (sel == 2'b01) ? dm_data_r[15:8] :
//    (sel == 2'b10) ? dm_data_r[23:16] :
//    dm_data_r[31:24];
//assign s_ext8_out = {{24{selected_byte[7]}}, selected_byte};
//assign ext8_out = selected_byte;
//assign s_ext8_out = dm_data_r;
//assign ext8_out = dm_data_r;

//wire [15:0] selected_halfword;
//assign selected_halfword = 
//    (sel == 2'b00) ? dm_data_r[15:0] :  // 低半字
//    (sel == 2'b10) ? dm_data_r[31:16] : // 高半字
//    16'b0; // 其他情况（应处理未对齐异常）
//assign s_ext16_out_2 = {{16{selected_halfword[15]}} , selected_halfword[15:0]};
//assign ext16_out_2 = selected_halfword;
//assign s_ext16_out_2 = dm_data_r;
//assign ext16_out_2 = dm_data_r;

// ||部件
assign cat_out = {pc_out[31:28], index[25:0], 2'h0};



always @(negedge clk) begin
    inst_reg <= inst;  // 寄存 IMEM 输出
end
/* ------------实例化------------ */
/* 实例化各部件 */
Controller uut (
    // 输入信号连接
    .clk        (clk),
    .rst        (rst),
//    .inst       (inst),
    .inst(inst_reg), 
    .zero       (zero),
    .overflow   (overflow),
    .negative   (negative),
    .busy       (busy),
    // 输出信号连接
    .RsC        (RsC),
    .RtC        (RtC),
    .RdC        (RdC),
    .rf_w       (rf_w),
    .aluc       (aluc),
    .mux        (mux),
    .m11        (m11),
    .m12        (m12),
    .m13        (m13),
    .m14        (m14),
    .m15        (m15),
    .cs         (cs),
    .dm_r       (dm_r),
    .dm_w       (dm_w),
    .HI_we       (HI_we),
    .LO_we       (LO_we),
    .mfc0       (mfc0),
    .mtc0       (mtc0),
    .exception   (exception),
    .eret       (eret),
    .cause      (cause),
//    .CP0_Rd     (CP0_Rd),
    .mult_start (mult_start),
    .multu_start    (multu_start),
    .div_start  (div_start),
    .divu_start (divu_start),
    .sw_flag    (sw_flag),
    .sb_flag    (sb_flag),
    .sh_flag    (sh_flag),
    .lb_flag(lb_flag),              //当前读取指令是否是LB发出
    .lh_flag(lh_flag),              //当前读取指令是否是LH发出
    .lbu_flag(lbu_flag),            //当前读取指令是否是LBU发出
    .lhu_flag(lhu_flag),            //当前读取指令是否是LHU发出
    .lw_flag(lw_flag),               //当前读取指令是否是LW发出
    .ext5_out(ext5_out),
    .ext16_out(ext16_out),
    .s_ext16_out(s_ext16_out),
    .s_ext18_out(s_ext18_out),
    .index(index)
);

PC PC_inst(                     
    .clk(clk),               // 下降沿有效，写入为同步，读取为异步
    .rst(rst),              //复位信号，高电平有效
    .pc_in(pc_in),    // 本次输入PC寄存器的指令地址，也就是下一次要执行的指令
    .pc_out(pc_out),   // 本次从PC寄存器中传出的指令地址，也就是当前需要执行的指令
    .stall(stall)
    );

//imem imem_inst(
//    .a(imem_in[12:2]),  //11位指令码地址，从IMEM中读指令
//    .spo(inst)     //32位指令码
//    );
    
Regfiles cpu_ref(                //寄存器堆RegFile，写入为同步，读取为异步
    .clk(clk),             // 时钟信号，下降沿写入数据
    .rst(rst),             // 异步复位信号，高电平时全部寄存器置零
    .we(rf_w && !busy),              // 写使能信号，高电平允许写入
    .RdC(RdC),               
    .RtC(RtC),               
    .RsC(RsC),                
    .Rd_data_in(Rd_data_in),    
    .Rs_data_out(Rs_data_out),
    .Rt_data_out(Rt_data_out) 
    );

alu alu_inst(                      
//    .a(a),                     
//    .b(b),                   
//    .aluc(aluc),   
    .a(alu_a_reg),                     
    .b(alu_b_reg),                   
    .aluc(aluc_reg),            
    .r(alu_out),
    .zero(zero),                // ZF标志位，BEQ/BNE使用
    .carry(carry),              // CF标志位，SLTI/SLTIU使用
    .negative(negative),        // NF(SF)标志位，SLT/SLTU使用
    .overflow(overflow)         // OF标志位，溢出时不修改寄存器，add、。。。使用？
    );

//DMEM dmem_inst (
//    .clk(clk),
//    .cs(cs),
//    .dm_w(dm_w),
//    .dm_r(dm_r),
//    .addr(dm_addr), 
//    .data_in(dm_data_w),
//    .data_out(dm_data_r)
//);

CP0 cp0_inst (
    .clk        (clk),
    .rst        (rst),    
    .mfc0       (mfc0 && !busy && !exception),
    .mtc0       (mtc0 && !busy && !exception),    
    .pc         (pc_out),
    .Rd         (CP0_Rd),
    .wdata      (CP0_wdata),    
    .exception  (exception),
    .eret       (eret),
    .cause      (cause),
    .intr       (CP0_intr),
    .rdata      (CP0_rdata),    
    .status     (status),
    .timer_int  (timer_int),
    .exc_addr   (exc_addr)
);

    MULT mult(
    .clk(clk),
    .reset(rst),
    .start(mult_start),  // 控制单元发出的启动信号
    .a(mult_a),          // 从寄存器堆读取的操作数a
    .b(mult_b),          // 从寄存器堆读取的操作数b
    .z(mult_out),     // 64位结果
    .busy(mult_busy)     // 忙信号
    );
    MULTU multu(
        .clk(clk),
        .reset(rst),
        .start(multu_start),  // 控制单元发出的启动信号
        .a(multu_a),          // 从寄存器堆读取的操作数a
        .b(multu_b),          // 从寄存器堆读取的操作数b
        .z(multu_out),     // 64位结果
        .busy(multu_busy)     // 忙信号
    );
    DIV div(
        .clock(clk),
        .reset(rst),
        .start(div_start),  // 控制单元发出的启动信号
        .dividend(div_a),          // 从寄存器堆读取的操作数a
        .divisor(div_b),          // 从寄存器堆读取的操作数b
        .q(div_q),
        .r(div_r),
        .busy(div_busy)     // 忙信号
    );
    DIVU divu(
        .clock(clk),
        .reset(rst),
        .start(divu_start),  // 控制单元发出的启动信号
        .dividend(divu_a),          // 从寄存器堆读取的操作数a
        .divisor(divu_b),          // 从寄存器堆读取的操作数b
        .q(divu_q),
        .r(divu_r),
        .busy(divu_busy)     // 忙信号
    );
    
    HI_LO hi_lo(
        .clk(clk),
        .rst(rst),
        .HI_in(HI_in), // 乘法结果高32位
        .LO_in(LO_in),   // 乘法结果低32位
        .HI_we(HI_we),
        .LO_we(LO_we),
        .HI_out(HI_out),
        .LO_out(LO_out)
    );
    
    clz clz(
        .in(clz_in),
        .out(clz_out)
    );
endmodule