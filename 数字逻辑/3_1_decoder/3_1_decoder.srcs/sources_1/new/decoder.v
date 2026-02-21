module decoder (
    input [2:0] iData,    
    input [1:0] iEna,    
    output [7:0] oData   
);
    wire [7:0] decode_out;

   
    wire [3:0] lower, upper;

    assign lower[0] = ~iData[1] & ~iData[0];
    assign lower[1] = ~iData[1] & iData[0];
    assign lower[2] = iData[1] & ~iData[0];
    assign lower[3] = iData[1] & iData[0];

    assign upper[0] = ~iData[1] & ~iData[0];
    assign upper[1] = ~iData[1] & iData[0];
    assign upper[2] = iData[1] & ~iData[0];
    assign upper[3] = iData[1] & iData[0];

    assign decode_out[0] = ~iData[2] & lower[0];
    assign decode_out[1] = ~iData[2] & lower[1];
    assign decode_out[2] = ~iData[2] & lower[2];
    assign decode_out[3] = ~iData[2] & lower[3];
    assign decode_out[4] = iData[2] & upper[0];
    assign decode_out[5] = iData[2] & upper[1];
    assign decode_out[6] = iData[2] & upper[2];
    assign decode_out[7] = iData[2] & upper[3];

    assign oData = (iEna == 2'b10) ? ~decode_out : 8'b11111111;
endmodule
