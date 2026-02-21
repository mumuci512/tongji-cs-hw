module tb_transmission8();
    reg [7:0] iData;
    reg A, B, C;
    wire [7:0] oData;

    transmission8 uut (
        .iData(iData),
        .A(A),
        .B(B),
        .C(C),
        .oData(oData)
    );

    initial begin
        iData = 8'b00000000;
        A = 0; B = 0; C = 0;
        #10 C = 1;
        #10 B = 1; C = 0;
        #10 C = 1;
        #10 A = 1; B = 0; C = 0;
        #10 C = 1;
        #10 $finish;
    end
endmodule