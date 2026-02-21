module display7(
    input [3:0] iData,
    output [6:0] oData 
);
    wire [6:0] seg_out;

    assign seg_out[0] = (~iData[3] & ~iData[2] & ~iData[1] &  iData[0]) | 
                        (~iData[3] &  iData[2] & ~iData[1] & ~iData[0]) | 
                        ( iData[3] & ~iData[2] &  iData[1] &  iData[0]) | 
                        ( iData[3] &  iData[2] & ~iData[1] &  iData[0]);

    assign seg_out[1] = (~iData[3] &  iData[2] & ~iData[1] &  iData[0]) | 
                        ( iData[3] &  iData[2] & ~iData[0]) | 
                        ( iData[2] &  iData[1] & ~iData[0]) | 
                        ( iData[3] &  iData[1] &  iData[0]);

    assign seg_out[2] = (~iData[3] & ~iData[2] &  iData[1] & ~iData[0]) | 
                        ( iData[3] &  iData[2] &  iData[0]) | 
                        ( iData[3] &  iData[2] &  iData[1]);

    assign seg_out[3] = (~iData[3] & ~iData[2] & ~iData[1] &  iData[0]) | 
                        (~iData[3] &  iData[2] & ~iData[1] & ~iData[0]) | 
                        ( iData[2] &  iData[1] &  iData[0]) | 
                        ( iData[3] & ~iData[2] &  iData[1] & ~iData[0]);

    assign seg_out[4] = (~iData[3] &  iData[0]) | 
                        (~iData[3] &  iData[2] & ~iData[1]) | 
                        (~iData[2] & ~iData[1] &  iData[0]);

    assign seg_out[5] = (~iData[3] & ~iData[2] &  iData[0]) | 
                        (~iData[3] & ~iData[2] &  iData[1]) | 
                        (~iData[3] &  iData[1] &  iData[0]) | 
                        ( iData[3] &  iData[2] & ~iData[1] &  iData[0]);

    assign seg_out[6] = (~iData[3] & ~iData[2] & ~iData[1]) | 
                        (~iData[3] &  iData[2] &  iData[1] &  iData[0]) | 
                        ( iData[3] &  iData[2] & ~iData[1] & ~iData[0]);

    assign oData = seg_out;
endmodule