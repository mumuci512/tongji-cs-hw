module tb_selector41();
    reg [3:0] iC0, iC1, iC2, iC3;
    reg iS1, iS0;
    wire [3:0] oZ;

    selector41 uut (
        .iC0(iC0),
        .iC1(iC1),
        .iC2(iC2),
        .iC3(iC3),
        .iS1(iS1),
        .iS0(iS0),
        .oZ(oZ)
    );

    initial begin
        iC0 = 4'b0001;
        iC1 = 4'b0010;
        iC2 = 4'b0100;
        iC3 = 4'b1000;
        iS1 = 0; iS0 = 0;
        #10 iS0 = 1;
        #10 iS1 = 1; iS0 = 0;
        #10 iS0 = 1;
        #10 $finish;
    end
endmodule