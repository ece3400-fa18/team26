`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144
`define NUM_BARS 3
`define BAR_HEIGHT 48
`define COLOR_THRESHOLD 15000

module IMAGE_PROCESSOR (
	PIXEL_IN,
	CLK,
	VGA_PIXEL_X,
	VGA_PIXEL_Y,
	VGA_VSYNC_NEG,
	RESULT
);


//=======================================================
//  PORT declarations
//=======================================================

input	[7:0]	PIXEL_IN;
input 		CLK;

input [9:0] VGA_PIXEL_X;
input [9:0] VGA_PIXEL_Y;
input		VGA_VSYNC_NEG;

output [7:0] RESULT;

reg[9:0] red_count;
reg[9:0] blue_count;
reg[7:0] RESULT;


wire[1:0] RED;
wire[1:0] BLUE;
wire[1:0] GREEN;

assign RED = PIXEL_IN[7:6];
assign GREEN = PIXEL_IN[4:3];
assign BLUE = PIXEL_IN[1:0];


always @(posedge CLK) begin
	if(!VGA_VSYNC_NEG) begin
		if(red_count > blue_count && red_count > 10'd15000) begin
			RESULT[7:6] = 2'b10;
		end
		else if (blue_count > red_count && blue_count > 10'd15000) begin
			RESULT[7:6] = 2'b01;
		end
		else begin
			RESULT[7:6] = 2'b00;
		end
		RESULT[5:0] = 6'b0;
		red_count = 0;
		blue_count = 0;
	end
	else begin
		if (VGA_PIXEL_Y >= 10'b0001000110 && VGA_PIXEL_Y <= 10'b0001001010) begin
			if (RED > BLUE && RED > GREEN) red_count = red_count + 1;
			else if (BLUE > RED && BLUE > GREEN) blue_count = blue_count + 1;
		end
	end
	
end // end always


endmodule