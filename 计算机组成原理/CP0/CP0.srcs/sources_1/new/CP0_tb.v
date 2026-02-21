`timescale 1ns / 1ps

module CP0_tb;

reg clk;
reg rst;
reg mfc0;
reg mtc0;
reg [31:0] pc;
reg [4:0] Rd;
reg [31:0] wdata;
reg exception;
reg eret;
reg [4:0] cause;
reg intr;

wire [31:0] rdata;
wire [31:0] status;
wire timer_int;
wire [31:0] exc_addr;

CP0 uut (
    .clk(clk),
    .rst(rst),
    .mfc0(mfc0),
    .mtc0(mtc0),
    .pc(pc),
    .Rd(Rd),
    .wdata(wdata),
    .exception(exception),
    .eret(eret),
    .cause(cause),
    .intr(intr),
    .rdata(rdata),
    .status(status),
    .timer_int(timer_int),
    .exc_addr(exc_addr)
);

always #5 clk = ~clk;

task set_at_falling_edge;
    input [31:0] new_pc;
    input [4:0] new_cause;
    input new_exception;
    input new_eret;
    input new_intr;
    begin
        @(negedge clk); // 等待下降沿
        #1; // 稍作延迟确保在下降沿之后
        pc = new_pc;
        cause = new_cause;
        exception = new_exception;
        eret = new_eret;
        intr = new_intr;
    end
endtask

initial begin
    // 初始化信号
    clk = 0;
    rst = 1;
    mfc0 = 0;
    mtc0 = 0;
    pc = 32'h0;
    Rd = 5'd0;
    wdata = 32'h0;
    exception = 0;
    eret = 0;
    cause = 5'b0;
    intr = 0;

    // 复位阶段
    #10;
    rst = 0;
    #10; // 等待复位完成

    // 步骤1: 向Status寄存器写入值
    @(negedge clk); // 等待下降沿
    #1;
    mtc0 = 1;
    Rd = 5'd12; // Status寄存器
    wdata = 32'b00000000000000000000000100010101; // IE=1, syscall mask bit[8]=1
    #10;
    @(negedge clk); // 等待下降沿
    #1;
    mtc0 = 0;
    
    // 步骤2: 模拟syscall异常
    set_at_falling_edge(32'h12345678, 5'b01000, 1, 0, 0);
    #10; // 保持异常信号一个时钟周期
    set_at_falling_edge(32'h0, 5'b0, 0, 0, 0); // 清除异常信号
    
    // 步骤3: 读取EPC内容
    set_at_falling_edge(32'h0, 5'b0, 0, 0, 0);
    #1;
    mfc0 = 1;
    Rd = 5'd14; // EPC寄存器
    #10;
    $display("EPC captured: %h", rdata);
    set_at_falling_edge(32'h0, 5'b0, 0, 0, 0);
    #1;
    mfc0 = 0;
    
    // 步骤4: 执行eret恢复中断状态
    set_at_falling_edge(32'h0, 5'b0, 0, 1, 0);
    @(posedge clk); // 等待下一个上升沿（执行eret）,防止连续2个下降沿导致status连续右移2次
    set_at_falling_edge(32'h0, 5'b0, 0, 0, 0); // 清除eret
    
    // 步骤5: 模拟中断信号
    set_at_falling_edge(32'h0, 5'b0, 0, 0, 1);
    #15;
    set_at_falling_edge(32'h0, 5'b0, 0, 0, 0); // 清除中断
    
    // 显示最终状态
    #20;
    $display("Final Status: %b", status);
    $display("Final Cause: %b", uut.cp0_reg[13]); // 直接访问内部寄存器
    $display("Exception Address: %h", exc_addr);
    
    $stop;
end

// 添加监控器跟踪关键信号
initial begin
    $monitor("Time=%0t: clk=%b, exception=%b, ex_occur=%b, EPC=%h, Status=%h, Cause[6:2]=%b",
             $time, clk, exception, uut.ex_occur, uut.cp0_reg[uut.EPC], 
             status, uut.cp0_reg[uut.Cause][6:2]);
end

endmodule