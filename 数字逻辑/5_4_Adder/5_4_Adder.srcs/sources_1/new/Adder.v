module FA(
    input iA,  
    input iB,  
    input iC,  
    output oS,
    output oC  
);

    wire w1, w2, w3;

    xor (w1, iA, iB);
    xor (oS, w1, iC);

    and (w2, iA, iB);
    and (w3, w1, iC);

    or (oC, w2, w3);

endmodule

module Adder(
    input [7:0] iData_a, // 8 位二进制加数
    input [7:0] iData_b, // 8 位二进制被加数
    input iC,            // 低位的进位信号
    output [7:0] oData,  // 8位和数
    output oData_C       // 向高位的进位信号
);

    wire [7:0] carry; // 用于保存中间进位

    // 实例化第0位加法器
    FA fa0 (
        .iA(iData_a[0]),
        .iB(iData_b[0]),
        .iC(iC),
        .oS(oData[0]),
        .oC(carry[0])
    );

    // 实例化第1到6位加法器
    genvar i;
    generate
        for (i = 1; i < 7; i = i + 1) begin : generate_fa
            FA fa (
                .iA(iData_a[i]),
                .iB(iData_b[i]),
                .iC(carry[i-1]),
                .oS(oData[i]),
                .oC(carry[i])
            );
        end
    endgenerate

    // 实例化第7位加法器
    FA fa7 (
        .iA(iData_a[7]),
        .iB(iData_b[7]),
        .iC(carry[6]),
        .oS(oData[7]),
        .oC(oData_C)
    );

endmodule
