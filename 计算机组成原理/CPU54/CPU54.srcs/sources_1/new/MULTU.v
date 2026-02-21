module MULTU(
    input clk,             // 时钟信号
    input reset,           // 复位信号，高电平有效
    input start,           // 启动信号，高电平开始计算
    input [31:0] a,        // 被乘数
    input [31:0] b,        // 乘数
    output reg [63:0] z,   // 乘积输出（64位无符号数）
    output reg busy        // 忙信号，高电平表示计算中
);

    reg [63:0] temp;          // 临时计算结果
    reg [6:0] count;          // 迭代计数器
    reg running;              // 计算进行中标志

    always @(negedge clk or posedge reset) begin
        if (reset) begin
            temp <= 64'd0;
            count <= 6'd0;
            running <= 1'b0;
            busy <= 1'b0;
            z <= 64'd0;
        end
        else begin
            // 启动信号处理
            if (start && !running) begin
                // 初始化临时结果
                temp <= 64'd0;
                count <= 6'd0;
                running <= 1'b1;
                busy <= 1'b1;
            end
            
            // 计算过程
            if (running) begin
                if (b[count]) begin
                    temp <= temp + ({32'd0, a} << count);
                end
                
                count <= count + 1;
                
                // 计算完成
                if (count == 32) begin
                    running <= 1'b0;
                    z <= temp;  // 更新输出寄存器
                    busy <= 1'b0;
                end
            end
        end
    end

endmodule