module pc (
    input         clk,
    input         rst,
    input [31:0]  pc_in,
    output reg [31:0] pc_out, 
    input stall
);

    // 时钟上升沿/复位上升沿触发，更新PC值
    always @(posedge clk or posedge rst) begin
        if (rst) 
            // 复位时PC初始化为MIPS标准起始地址0x00400000
            pc_out = 32'h00400000;
        else if (!stall)
            // 流水线未暂停时，加载下一个PC值
            pc_out = pc_in;
    end

endmodule