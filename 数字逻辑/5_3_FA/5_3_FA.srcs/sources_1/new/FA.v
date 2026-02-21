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