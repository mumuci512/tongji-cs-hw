module MULTU(
    input clk,             // 时钟信号
    input reset,           // 复位信号，高电平有效
    input [31:0] a,        // 被乘数
    input [31:0] b,        // 乘数
    output reg [63:0] z    // 乘积输出（64位无符号数）
);

    reg [63:0] temp;      
    integer i;           

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            temp <= 64'd0;
        end
        else begin
            temp = 64'd0;  // 每次计算前清零
            for (i = 0; i < 32; i = i + 1) begin
                if (b[i])
                    temp = temp + ({32'd0, a} << i); // 通过字符拼接方式表示出中间相乘值，并相加
            end
        end
    end

    // 输出寄存器，同步更新
    always @(posedge clk or posedge reset) begin
        if (reset)
            z <= 64'd0;
        else
            z <= temp;
    end

endmodule
