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
    output [31:0] pc_out          // 本次执行指令的地址，由pc输出
    );
/* ------------信号定义------------ */
/* 控制信号，即Controller的输出 */
wire [4:0] RsC;                 //Rs对应的寄存器的地址
wire [4:0] RtC;                 //Rt对应的寄存器的地址
wire [4:0] RdC;                 //Rd对应的寄存器的地址
wire rf_w;                     //RegFile寄存器堆是否可写入
wire [3:0]  aluc; 
wire [10:0] mux;  
    
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

/* 无需控制信号的部件固定输出值 */
wire [4:0] sa;        // SLL，SRL，SRA用
wire [15:0] imm;
wire [25:0] index;     // 跳转地址（J,JAL用）

// ext部件
wire [31:0] ext1_out;
wire [31:0] ext5_out;
wire [31:0] ext16_out;
wire signed [31:0] s_ext16_out;
wire signed [31:0] s_ext18_out;

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
assign mux8_out = mux[8] ? ext1_out : alu_out;
assign mux9_out = mux[9] ? pc_out + 4 : mux2_out;
assign mux10_out = mux[10] ? carry : negative;

// NPC
assign npc = pc_out + 4;

// PC
assign pc_in = mux7_out;

// IMEM
assign imem_in = pc_out - 32'h00400000;

// RegFile
assign Rd_data_in = mux9_out;

// ALU
assign a = mux3_out;
assign b = mux4_out; 

// DMEM
assign dm_addr = (alu_out -  32'h10010000)/4;
assign dm_data_w = Rt_data_out;

/* 无需控制信号的部件固定输出值 */
assign sa =  inst[10:6];
assign imm = inst[15:0];
assign index = inst[25:0];

// ext部件
assign ext1_out = mux10_out;
assign ext5_out = sa;
assign ext16_out = imm;
assign s_ext16_out = {{16{imm[15]}} , imm[15:0]};
assign s_ext18_out = {{14{imm[15]}}, imm[15:0], 2'b0};

// ||部件
assign cat_out = {pc_out[31:28], index[25:0], 2'h0};


/* ------------实例化------------ */
/* 实例化各部件 */
Controller Controller_inst(              
    .inst(inst),
    .zero(zero),                //ALU标志位ZF
    .overflow(overflow),
    .RsC(RsC),
    .RtC(RtC),
    .RdC(RdC),
    .rf_w(rf_w),  
    .aluc(aluc),  
    .mux(mux),     
    .cs(cs),
    .dm_r(dm_r),            
    .dm_w(dm_w)           
    );

PC PC_inst(                     
    .clk(clk),               // 下降沿有效，写入为同步，读取为异步
    .rst(rst),              //复位信号，高电平有效
    .pc_in(pc_in),    // 本次输入PC寄存器的指令地址，也就是下一次要执行的指令
    .pc_out(pc_out)   // 本次从PC寄存器中传出的指令地址，也就是当前需要执行的指令
    );

//imem imem_inst(
//    .a(imem_in[12:2]),  //11位指令码地址，从IMEM中读指令
//    .spo(inst)     //32位指令码
//    );
    
Regfiles cpu_ref(                //寄存器堆RegFile，写入为同步，读取为异步
    .clk(clk),             // 时钟信号，下降沿写入数据
    .rst(rst),             // 异步复位信号，高电平时全部寄存器置零
    .we(rf_w),              // 写使能信号，高电平允许写入
    .RdC(RdC),               
    .RtC(RtC),               
    .RsC(RsC),                
    .Rd_data_in(Rd_data_in),    
    .Rs_data_out(Rs_data_out),
    .Rt_data_out(Rt_data_out) 
    );

alu alu_inst(                      
    .a(a),                     
    .b(b),                   
    .aluc(aluc),           
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
endmodule