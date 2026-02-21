module tb_FA();
    reg iA;
    reg iB;
    reg iC;
    wire oS;
    wire oC;

    FA uut (
        .iA(iA),
        .iB(iB),
        .iC(iC),
        .oS(oS),
        .oC(oC)
    );

    initial begin
        $monitor("iA = %b, iB = %b, iC = %b -> oS = %b, oC = %b", iA, iB, iC, oS, oC);

        iA = 0; iB = 0; iC = 0; #10;
        iA = 0; iB = 0; iC = 1; #10;
        iA = 0; iB = 1; iC = 0; #10;
        iA = 0; iB = 1; iC = 1; #10;
        iA = 1; iB = 0; iC = 0; #10;
        iA = 1; iB = 0; iC = 1; #10;
        iA = 1; iB = 1; iC = 0; #10;
        iA = 1; iB = 1; iC = 1; #10;

        $finish;
    end
endmodule