`timescale 1ns / 1ps

module register(
    input               clk, 
    input               rst, 
    input               wena, 
    input [31:0]        data_in, 
    output reg [31:0]   data_out 
    );
	
    // 时序逻辑：时钟上升沿或复位上升沿触发，实现数据锁存与清零
    always@(posedge clk or posedge rst)
    begin
        if(rst)
            // 复位状态：输出端口清零，确保初始状态无无效数据
            data_out <= 32'b0;
        else if(wena)
            // 写使能有效：将输入数据锁存到输出，完成数据暂存
            data_out <= data_in;
    end

endmodule