//5-32译码器
module decoder (
    input [4:0] addr,       // 输入地址
    input enb,              // 使能信号
    output reg [31:0] decode // 解码输出
);
    always @(*) begin
        if (enb) begin
            decode = 32'b0;
            decode[addr] = 1'b1; // 根据地址激活对应位
        end else begin
            decode = 32'b0; // 禁止状态下输出全零
        end
    end
endmodule


//寄存器
module pcreg(
    input clk,               //时钟信号，上升沿时为PC寄存器赋值
    input rst,               //异步重置信号，高电平时清零PC寄存器
    input ena,               //有效信号，高电平时PC寄存器读取data_in
    input [31:0] data_in,    //32位输入数据，将被存入寄存器
    output reg [31:0] data_out //32位输出，始终输出PC寄存器的存储值
);

    //为了和寄存器堆的接口定义一致，将posedge clk改为negedge clk
    always @(negedge clk or posedge rst) begin
        if (rst) begin
            data_out <= 32'b0; //复位信号高时，清零寄存器
        end else if (ena) begin
            data_out <= data_in;//ena有效时，将data_in的值存入寄存器
        end
        //如果ena无效，则保持原有data_out值不变
    end

endmodule

// 32选1选择器 (带使能信号)
module selector (
    input [31:0] in0, input [31:0] in1, input [31:0] in2, input [31:0] in3,
    input [31:0] in4, input [31:0] in5, input [31:0] in6, input [31:0] in7,
    input [31:0] in8, input [31:0] in9, input [31:0] in10, input [31:0] in11,
    input [31:0] in12, input [31:0] in13, input [31:0] in14, input [31:0] in15,
    input [31:0] in16, input [31:0] in17, input [31:0] in18, input [31:0] in19,
    input [31:0] in20, input [31:0] in21, input [31:0] in22, input [31:0] in23,
    input [31:0] in24, input [31:0] in25, input [31:0] in26, input [31:0] in27,
    input [31:0] in28, input [31:0] in29, input [31:0] in30, input [31:0] in31,
    input [4:0] sel,      // 选择控制信号
    input enb,            // 使能信号，高电平时输出选择的值，低电平时输出 x
    output [31:0] out     // 输出
);
    assign out = enb ? ((sel == 5'd0) ? in0 :
                        (sel == 5'd1) ? in1 :
                        (sel == 5'd2) ? in2 :
                        (sel == 5'd3) ? in3 :
                        (sel == 5'd4) ? in4 :
                        (sel == 5'd5) ? in5 :
                        (sel == 5'd6) ? in6 :
                        (sel == 5'd7) ? in7 :
                        (sel == 5'd8) ? in8 :
                        (sel == 5'd9) ? in9 :
                        (sel == 5'd10) ? in10 :
                        (sel == 5'd11) ? in11 :
                        (sel == 5'd12) ? in12 :
                        (sel == 5'd13) ? in13 :
                        (sel == 5'd14) ? in14 :
                        (sel == 5'd15) ? in15 :
                        (sel == 5'd16) ? in16 :
                        (sel == 5'd17) ? in17 :
                        (sel == 5'd18) ? in18 :
                        (sel == 5'd19) ? in19 :
                        (sel == 5'd20) ? in20 :
                        (sel == 5'd21) ? in21 :
                        (sel == 5'd22) ? in22 :
                        (sel == 5'd23) ? in23 :
                        (sel == 5'd24) ? in24 :
                        (sel == 5'd25) ? in25 :
                        (sel == 5'd26) ? in26 :
                        (sel == 5'd27) ? in27 :
                        (sel == 5'd28) ? in28 :
                        (sel == 5'd29) ? in29 :
                        (sel == 5'd30) ? in30 : in31) : 32'bz;
endmodule

// 寄存器堆
module Regfiles (
    input clk,                // 时钟信号，下降沿写入数据
    input rst,                // 异步复位信号，高电平时全部寄存器置零
    input we,                 // 写使能信号，高电平允许写入
    input [4:0] raddr1,       // 第一个读地址
    input [4:0] raddr2,       // 第二个读地址
    input [4:0] waddr,        // 写地址
    input [31:0] wdata,       // 写入数据
    output [31:0] rdata1,     // 第一个读数据
    output [31:0] rdata2      // 第二个读数据
);

    // 解码器输出，表示写使能信号分发到的寄存器
    wire [31:0] decode;

    // 每个寄存器的输出数据
    wire [31:0] reg_out [31:0];

    // 实例化译码器，增加使能信号
    decoder decoder_inst (
        .addr(waddr),
        .enb(we), // 将写使能信号作为译码器使能信号
        .decode(decode)
    );

    // 实例化 32 个寄存器
    genvar i;
    generate
        for (i = 0; i < 32; i = i + 1) begin : reg_block
            pcreg reg_inst (
                .clk(clk),             // 时钟信号
                .rst(rst),             // 异步复位信号
                .ena(decode[i]),       // 使能信号，由译码器输出控制
                .data_in(wdata),       // 写入数据
                .data_out(reg_out[i])  // 输出数据
            );
        end
    endgenerate

    // 实例化两个选择器，分别用于选择 raddr1 和 raddr2 对应的寄存器数据
    selector selector1_inst (
        .in0(reg_out[0]), .in1(reg_out[1]), .in2(reg_out[2]), .in3(reg_out[3]),
        .in4(reg_out[4]), .in5(reg_out[5]), .in6(reg_out[6]), .in7(reg_out[7]),
        .in8(reg_out[8]), .in9(reg_out[9]), .in10(reg_out[10]), .in11(reg_out[11]),
        .in12(reg_out[12]), .in13(reg_out[13]), .in14(reg_out[14]), .in15(reg_out[15]),
        .in16(reg_out[16]), .in17(reg_out[17]), .in18(reg_out[18]), .in19(reg_out[19]),
        .in20(reg_out[20]), .in21(reg_out[21]), .in22(reg_out[22]), .in23(reg_out[23]),
        .in24(reg_out[24]), .in25(reg_out[25]), .in26(reg_out[26]), .in27(reg_out[27]),
        .in28(reg_out[28]), .in29(reg_out[29]), .in30(reg_out[30]), .in31(reg_out[31]),
        .sel(raddr1),      // 第一个读地址
        .enb(~we),         // 使能信号，写使能取非
        .out(rdata1)       // 第一个读数据
    );

    selector selector2_inst (
        .in0(reg_out[0]), .in1(reg_out[1]), .in2(reg_out[2]), .in3(reg_out[3]),
        .in4(reg_out[4]), .in5(reg_out[5]), .in6(reg_out[6]), .in7(reg_out[7]),
        .in8(reg_out[8]), .in9(reg_out[9]), .in10(reg_out[10]), .in11(reg_out[11]),
        .in12(reg_out[12]), .in13(reg_out[13]), .in14(reg_out[14]), .in15(reg_out[15]),
        .in16(reg_out[16]), .in17(reg_out[17]), .in18(reg_out[18]), .in19(reg_out[19]),
        .in20(reg_out[20]), .in21(reg_out[21]), .in22(reg_out[22]), .in23(reg_out[23]),
        .in24(reg_out[24]), .in25(reg_out[25]), .in26(reg_out[26]), .in27(reg_out[27]),
        .in28(reg_out[28]), .in29(reg_out[29]), .in30(reg_out[30]), .in31(reg_out[31]),
        .sel(raddr2),      // 第二个读地址
        .enb(~we),         // 使能信号，写使能取非
        .out(rdata2)       // 第二个读数据
    );

endmodule
