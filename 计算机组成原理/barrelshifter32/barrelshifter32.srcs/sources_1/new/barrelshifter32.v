module barrelshifter32(
    input [31:0] a,        
    input [4:0] b,         
    input [1:0] aluc,     
    output reg [31:0] c    
);
    
    always @(*) begin
        case (aluc)
            2'b00: //ËãÊýÓÒÒÆ
                c = $signed(a) >>> b;
            2'b10: //Âß¼­ÓÒÒÆ
                c = a >> b;
            2'b01: //ËãÊõ×óÒÆ
                 c = a << b;
            2'b11: //Âß¼­×óÒÆ
                c = a << b;
        endcase
    end
    
endmodule