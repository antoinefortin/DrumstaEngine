module blink(
    input  wire sysclk_p,
    input  wire sysclk_n,
    input  wire btnc,
    input  wire btnu,
    input  wire btnr,
    output wire [7:0] led
);

    wire clk;

    IBUFDS clock_input (
        .I(sysclk_p),
        .IB(sysclk_n),
        .O(clk)
    );

    reg [31:0] counter = 32'd0;

    // werird shit gogtta revierw
    
    always @(posedge clk) begin
        counter <= counter + 1'b1;
    end


    assign led[0] = btnc;
    assign led[1] = btnu;
      assign led[5] = btnr ? btnu : counter[26];

  //  assign led[2] = counter[25];
   // assign led[3] = counter[26];
    //assign led[4] = counter[27];
//    assign led[5] = counter[28];
  //  assign led[6] = counter[29];
   // assign led[7] = counter[30];

endmodule