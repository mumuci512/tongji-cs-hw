`timescale 1ns / 1ps
module cpu_tb;
reg clk;            //时钟信号
reg rst;            //复位信号
wire [31:0] inst;   //要执行的指令
//wire [31:0] pc;     //下一条指令的地址
//wire [10:0] mux;
//wire [31:0] alu_out;
//wire [31:0] a;
//wire [31:0] b;
//wire [4:0] RdC;
//wire [4:0] RsC;
//wire [4:0] RtC;
//wire [31:0] Rd_data_in;     //要向寄存器中写入的值
//wire [31:0] Rs_data_out;    //Rs对应的寄存器的输出值
//wire [31:0] Rt_data_out;    //Rt对应的寄存器的输出值
//wire [31:0] cat_out;
reg  [31:0] cnt;    //计数器，已经执行了几条指令
integer file_open;

// 新增信号：缓存当前指令的 PC 和 inst
reg [31:0] current_pc;
reg [31:0] current_inst;

initial 
begin
    file_open = $fopen("D:\\hw-dlogic\\projects\\CPU31\\output.txt", "w");
    clk = 1'b0;
    rst = 1'b1;
    #50 rst = 1'b0;
    cnt = 0;
    current_pc = 32'h00400000;
    current_inst = 0;
end

always  #50 clk = ~clk;

// 在时钟下降沿捕获当前指令的 PC 和 inst
always @ (negedge clk) begin
    current_pc <= uut.sccpu.pc_out;   // 当前指令的 PC（尚未更新）
    current_inst <= uut.sccpu.inst;   // 当前指令的机器码
end

always @ (posedge clk) begin
    if (cnt > 0) begin  // 跳过复位后的第一个周期    
        //$fdisplay(file_open, "OP: %d", cnt);
        $fdisplay(file_open, "pc: %h", current_pc);
        $fdisplay(file_open, "instr: %h", current_inst);
        $fdisplay(file_open, "regfile0: %h", uut.sccpu.cpu_ref.array_reg[0]);
        $fdisplay(file_open, "regfile1: %h", uut.sccpu.cpu_ref.array_reg[1]);
        $fdisplay(file_open, "regfile2: %h", uut.sccpu.cpu_ref.array_reg[2]);
        $fdisplay(file_open, "regfile3: %h", uut.sccpu.cpu_ref.array_reg[3]);
        $fdisplay(file_open, "regfile4: %h", uut.sccpu.cpu_ref.array_reg[4]);
        $fdisplay(file_open, "regfile5: %h", uut.sccpu.cpu_ref.array_reg[5]);
        $fdisplay(file_open, "regfile6: %h", uut.sccpu.cpu_ref.array_reg[6]);
        $fdisplay(file_open, "regfile7: %h", uut.sccpu.cpu_ref.array_reg[7]);
        $fdisplay(file_open, "regfile8: %h", uut.sccpu.cpu_ref.array_reg[8]);
        $fdisplay(file_open, "regfile9: %h", uut.sccpu.cpu_ref.array_reg[9]);
        $fdisplay(file_open, "regfile10: %h", uut.sccpu.cpu_ref.array_reg[10]);
        $fdisplay(file_open, "regfile11: %h", uut.sccpu.cpu_ref.array_reg[11]);
        $fdisplay(file_open, "regfile12: %h", uut.sccpu.cpu_ref.array_reg[12]);
        $fdisplay(file_open, "regfile13: %h", uut.sccpu.cpu_ref.array_reg[13]);
        $fdisplay(file_open, "regfile14: %h", uut.sccpu.cpu_ref.array_reg[14]);
        $fdisplay(file_open, "regfile15: %h", uut.sccpu.cpu_ref.array_reg[15]);
        $fdisplay(file_open, "regfile16: %h", uut.sccpu.cpu_ref.array_reg[16]);
        $fdisplay(file_open, "regfile17: %h", uut.sccpu.cpu_ref.array_reg[17]);
        $fdisplay(file_open, "regfile18: %h", uut.sccpu.cpu_ref.array_reg[18]);
        $fdisplay(file_open, "regfile19: %h", uut.sccpu.cpu_ref.array_reg[19]);
        $fdisplay(file_open, "regfile20: %h", uut.sccpu.cpu_ref.array_reg[20]);
        $fdisplay(file_open, "regfile21: %h", uut.sccpu.cpu_ref.array_reg[21]);
        $fdisplay(file_open, "regfile22: %h", uut.sccpu.cpu_ref.array_reg[22]);
        $fdisplay(file_open, "regfile23: %h", uut.sccpu.cpu_ref.array_reg[23]);
        $fdisplay(file_open, "regfile24: %h", uut.sccpu.cpu_ref.array_reg[24]);
        $fdisplay(file_open, "regfile25: %h", uut.sccpu.cpu_ref.array_reg[25]);
        $fdisplay(file_open, "regfile26: %h", uut.sccpu.cpu_ref.array_reg[26]);
        $fdisplay(file_open, "regfile27: %h", uut.sccpu.cpu_ref.array_reg[27]);
        $fdisplay(file_open, "regfile28: %h", uut.sccpu.cpu_ref.array_reg[28]);
        $fdisplay(file_open, "regfile29: %h", uut.sccpu.cpu_ref.array_reg[29]);
        $fdisplay(file_open, "regfile30: %h", uut.sccpu.cpu_ref.array_reg[30]);
        $fdisplay(file_open, "regfile31: %h", uut.sccpu.cpu_ref.array_reg[31]);
    //    $fdisplay(file_open, "dmem_addr = %h", uut.dmem.dm_addr);
    //    $fdisplay(file_open, "dm_data_in = %h", uut.dmem.dm_data_in);
    //    $fdisplay(file_open, "dm_data_out = %h", uut.dmem.dm_data_out);
    //    $fdisplay(file_open, "$dmem0 = %h", uut.dmem.dmem[0]);
    //    $fdisplay(file_open, "$dmem1 = %h", uut.dmem.dmem[1]);
    //    $fdisplay(file_open, "$dmem2 = %h", uut.dmem.dmem[2]);
    //    $fdisplay(file_open, "$dmem3 = %h", uut.dmem.dmem[3]);
    //    $fdisplay(file_open, "$dmem4 = %h", uut.dmem.dmem[4]);
    //    $fdisplay(file_open, "$dmem5 = %h", uut.dmem.dmem[5]);
    //    $fdisplay(file_open, "$dmem6 = %h", uut.dmem.dmem[6]);
    //    $fdisplay(file_open, "$dmem7 = %h", uut.dmem.dmem[7]);
    //    $fdisplay(file_open, "$dmem8 = %h", uut.dmem.dmem[8]);
    //    $fdisplay(file_open, "$dmem9 = %h", uut.dmem.dmem[9]);
    //    $fdisplay(file_open, "$dmem10 = %h", uut.dmem.dmem[10]);
    //    $fdisplay(file_open, "$dmem11 = %h", uut.dmem.dmem[11]);
    //    $fdisplay(file_open, "$dmem12 = %h", uut.dmem.dmem[12]);
    //    $fdisplay(file_open, "$dmem13 = %h", uut.dmem.dmem[13]);
    //    $fdisplay(file_open, "$dmem14 = %h", uut.dmem.dmem[14]);                                                          
    //    $fdisplay(file_open, "$dmem15 = %h", uut.dmem.dmem[15]);
    //    $fdisplay(file_open, "$dmem16 = %h", uut.dmem.dmem[16]);
    //    $fdisplay(file_open, "$dmem17 = %h", uut.dmem.dmem[17]);
    //    $fdisplay(file_open, "$dmem18 = %h", uut.dmem.dmem[18]);
    //    $fdisplay(file_open, "$dmem19 = %h", uut.dmem.dmem[19]);
    //    $fdisplay(file_open, "$dmem20 = %h", uut.dmem.dmem[20]);
    //    $fdisplay(file_open, "$dmem21 = %h", uut.dmem.dmem[21]);
    //    $fdisplay(file_open, "$dmem22 = %h", uut.dmem.dmem[22]);
    //    $fdisplay(file_open, "$dmem23 = %h", uut.dmem.dmem[23]);                                                          
    //    $fdisplay(file_open, "$dmem24 = %h", uut.dmem.dmem[24]);
    //    $fdisplay(file_open, "$dmem25 = %h", uut.dmem.dmem[25]);
    //    $fdisplay(file_open, "$dmem26 = %h", uut.dmem.dmem[26]);
    //    $fdisplay(file_open, "$dmem27 = %h", uut.dmem.dmem[27]);
    //    $fdisplay(file_open, "$dmem28 = %h", uut.dmem.dmem[28]);
    //    $fdisplay(file_open, "$dmem29 = %h", uut.dmem.dmem[29]);
    //    $fdisplay(file_open, "$dmem30 = %h", uut.dmem.dmem[30]);
    //    $fdisplay(file_open, "$dmem31 = %h", uut.dmem.dmem[31]);
    end
    cnt <= cnt + 1'b1;
end

// 仿真结束时关闭文件
initial begin
    #18000;          // 根据仿真时间调整
    $fclose(file_open);
    $finish;
end

sccomp_dataflow uut(
    .clk_in(clk),
    .reset(rst),
    .inst(inst),
    .pc(pc)
);

endmodule