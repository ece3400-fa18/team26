// `define SCREEN_WIDTH 176
// `define SCREEN_HEIGHT 144
// `define NUM_BARS 3
// `define BAR_HEIGHT 48
// `define COLOR_THRESHOLD 15000

// module IMAGE_PROCESSOR (
// 	PIXEL_IN,
// 	CLK,
// 	VGA_PIXEL_X,
// 	VGA_PIXEL_Y,
// 	VGA_VSYNC_NEG,
// 	RESULT
// );


// //=======================================================
// //  PORT declarations
// //=======================================================

// input	[7:0]	PIXEL_IN;
// input 		CLK;

// input [9:0] VGA_PIXEL_X;
// input [9:0] VGA_PIXEL_Y;
// input		VGA_VSYNC_NEG;

// output [7:0] RESULT;

// reg[9:0] red_count;
// reg[9:0] blue_count;
// reg[7:0] RESULT;


// wire[1:0] RED;
// wire[1:0] BLUE;
// wire[1:0] GREEN;

// assign RED = PIXEL_IN[7:6];
// assign GREEN = PIXEL_IN[4:3];
// assign BLUE = PIXEL_IN[1:0];


// always @(posedge CLK) begin
// 	if(!VGA_VSYNC_NEG) begin
// 		if(red_count > blue_count && red_count > 10'd15000) begin
// 			RESULT[7:6] = 2'b10;
// 		end
// 		else if (blue_count > red_count && blue_count > 10'd15000) begin
// 			RESULT[7:6] = 2'b01;
// 		end
// 		else begin
// 			RESULT[7:6] = 2'b00;
// 		end
// 		RESULT[5:0] = 6'b0;
// 		red_count = 0;
// 		blue_count = 0;
// 	end
// 	else begin
// 		if (VGA_PIXEL_Y >= 10'b0001000110 && VGA_PIXEL_Y <= 10'b0001001010) begin
// 			if (RED > BLUE && RED > GREEN) red_count = red_count + 1;
// 			else if (BLUE > RED && BLUE > GREEN) blue_count = blue_count + 1;
// 		end
// 	end
	
// end // end always


// endmodule

`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144
`define NUM_BARS 3
`define BAR_HEIGHT 48

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
input			CLK;
input 	[9:0]	VGA_PIXEL_X;
input	[9:0]	VGA_PIXEL_Y;
input			VGA_VSYNC_NEG;

output reg [2:0] RESULT;

reg [15:0] BLUECOUNT = 0;
reg [15:0] BLUECOUNTTEMP = 0;
reg [15:0] REDCOUNT = 0;
reg [15:0] REDCOUNTTEMP = 0;
reg [15:0] BLUECOUNTTOP = 0;
reg [15:0] REDCOUNTTOP = 0;
reg [15:0] REDCOUNTTOPMID = 0;
reg [15:0] BLUECOUNTTOPMID = 0;
reg [15:0] REDCOUNTMID = 0;
reg [15:0] BLUECOUNTMID = 0;
reg [15:0] REDCOUNTBOTMID = 0;
reg [15:0] BLUECOUNTBOTMID = 0;
reg [15:0] BLUECOUNTBOTTOM = 0;
reg [15:0] REDCOUNTBOTTOM = 0;
reg [15:0] REDCOUNTTEMPTOP = 0;
reg [15:0] BLUECOUNTTEMPTOP = 0;
reg [15:0] REDCOUNTTEMPTOPMID = 0;
reg [15:0] BLUECOUNTTEMPTOPMID = 0;
reg [15:0] REDCOUNTTEMPMID = 0;
reg [15:0] BLUECOUNTTEMPMID = 0;
reg [15:0] REDCOUNTTEMPBOTMID = 0;
reg [15:0] BLUECOUNTTEMPBOTMID = 0;
reg [15:0] REDCOUNTTEMPBOTTOM = 0;
reg [15:0] BLUECOUNTTEMPBOTTOM = 0;

always @ (posedge CLK, negedge VGA_VSYNC_NEG) begin
		if (!VGA_VSYNC_NEG) begin
			if ((BLUECOUNTTEMP) > (REDCOUNTTEMP) && (BLUECOUNTTEMP) > 16'd17000) begin
				if (BLUECOUNTTEMPTOPMID>BLUECOUNTTEMPTOP && BLUECOUNTTEMPMID>BLUECOUNTTEMPTOPMID && 
				BLUECOUNTTEMPBOTMID>BLUECOUNTTEMPMID && BLUECOUNTTEMPBOTTOM>BLUECOUNTTEMPBOTMID) begin //&& BLUECOUNTTEMPTOP<BLUECOUNTTEMPBOTTOM 
					RESULT = 3'b001; //blue triangle
				end
				else if (BLUECOUNTTEMPTOP<BLUECOUNTTEMPTOPMID && BLUECOUNTTEMPTOP<BLUECOUNTTEMPMID && 
				BLUECOUNTTEMPBOTTOM<BLUECOUNTTEMPMID && BLUECOUNTTEMPBOTTOM<BLUECOUNTTEMPBOTMID) begin
					RESULT = 3'b010; //blue diamond
				end
				else begin
					RESULT = 3'b011; //blue square
				end
			end
			else if ((REDCOUNTTEMP) > (BLUECOUNTTEMP) && (REDCOUNTTEMP) > 16'd17000) begin
				if (REDCOUNTTEMPTOPMID>REDCOUNTTEMPTOP && REDCOUNTTEMPMID>REDCOUNTTEMPTOPMID && 
				REDCOUNTTEMPBOTMID>REDCOUNTTEMPMID && REDCOUNTTEMPBOTTOM>REDCOUNTTEMPBOTMID) begin //&& REDCOUNTTEMPTOP<REDCOUNTTEMPBOTTOM 
					RESULT = 3'b100; //red triangle
				end
				else if (REDCOUNTTEMPTOP<REDCOUNTTEMPTOPMID && REDCOUNTTEMPTOP<REDCOUNTTEMPMID && 
				REDCOUNTTEMPBOTTOM<REDCOUNTTEMPMID && REDCOUNTTEMPBOTTOM<REDCOUNTTEMPBOTMID) begin
					RESULT = 3'b101; //red diamond
				end
				else begin
					RESULT = 3'b110; //red square
				end
			end
			else begin
				RESULT = 3'b000; //no color detected
			end
			BLUECOUNT = 0;
			REDCOUNT = 0;
			BLUECOUNTTOP = 0;
			REDCOUNTTOP = 0;
			REDCOUNTTOPMID = 0;
			BLUECOUNTTOPMID = 0;
			BLUECOUNTMID = 0;
			REDCOUNTMID = 0;
			BLUECOUNTBOTMID = 0;
			REDCOUNTBOTMID = 0;
			BLUECOUNTBOTTOM = 0;
			REDCOUNTBOTTOM = 0;
		end
		else begin
			RESULT = RESULT;
			if (PIXEL_IN[7:6] > PIXEL_IN[1:0] && PIXEL_IN[7:6] > PIXEL_IN[4:3]) begin
				REDCOUNT = REDCOUNT + 1'b1;
			end
			else if (PIXEL_IN[1:0] > PIXEL_IN[7:6] && PIXEL_IN[1:0] > PIXEL_IN[4:3]) begin
				BLUECOUNT = BLUECOUNT + 1'b1;
			end
			REDCOUNTTEMP = REDCOUNT;
			BLUECOUNTTEMP = BLUECOUNT;
			if (VGA_PIXEL_Y == 10'd42 && (VGA_PIXEL_X>=10'd48 && VGA_PIXEL_X<=10'd128))begin
				if (PIXEL_IN[7:6] > PIXEL_IN[1:0] && PIXEL_IN[7:6] > PIXEL_IN[4:3]) begin
					REDCOUNTTOP = REDCOUNTTOP + 1'b1;
				end
				else if (PIXEL_IN[1:0] > PIXEL_IN[7:6] && PIXEL_IN[1:0] > PIXEL_IN[4:3]) begin
					BLUECOUNTTOP = BLUECOUNTTOP + 1'b1;
				end
				REDCOUNTTEMPTOP = REDCOUNTTOP;
				BLUECOUNTTEMPTOP = BLUECOUNTTOP;
			end
			else if (VGA_PIXEL_Y == 10'd57 && (VGA_PIXEL_X>=10'd48 && VGA_PIXEL_X<=10'd128))begin
				if (PIXEL_IN[7:6] > PIXEL_IN[1:0] && PIXEL_IN[7:6] > PIXEL_IN[4:3]) begin
					REDCOUNTTOPMID = REDCOUNTTOPMID + 1'b1;
				end
				else if (PIXEL_IN[1:0] > PIXEL_IN[7:6] && PIXEL_IN[1:0] > PIXEL_IN[4:3]) begin
					BLUECOUNTTOPMID = BLUECOUNTTOPMID + 1'b1;
				end
				REDCOUNTTEMPTOPMID = REDCOUNTTOPMID;
				BLUECOUNTTEMPTOPMID = BLUECOUNTTOPMID;
			end
			else if (VGA_PIXEL_Y == 10'd72 && (VGA_PIXEL_X>=10'd48 && VGA_PIXEL_X<=10'd128)) begin
				if (PIXEL_IN[7:6] > PIXEL_IN[1:0] && PIXEL_IN[7:6] > PIXEL_IN[4:3]) begin
					REDCOUNTMID = REDCOUNTMID + 1'b1;
				end
				else if (PIXEL_IN[1:0] > PIXEL_IN[7:6] && PIXEL_IN[1:0] > PIXEL_IN[4:3]) begin
					BLUECOUNTMID = BLUECOUNTMID + 1'b1;
				end
				REDCOUNTTEMPMID = REDCOUNTMID;
				BLUECOUNTTEMPMID = BLUECOUNTMID;
			end
			else if (VGA_PIXEL_Y == 10'd87 && (VGA_PIXEL_X>=10'd48 && VGA_PIXEL_X<=10'd128)) begin
				if (PIXEL_IN[7:6] > PIXEL_IN[1:0] && PIXEL_IN[7:6] > PIXEL_IN[4:3]) begin
					REDCOUNTBOTMID = REDCOUNTBOTMID + 1'b1;
				end
				else if (PIXEL_IN[1:0] > PIXEL_IN[7:6] && PIXEL_IN[1:0] > PIXEL_IN[4:3]) begin
					BLUECOUNTBOTMID = BLUECOUNTBOTMID + 1'b1;
				end
				REDCOUNTTEMPBOTMID = REDCOUNTBOTMID;
				BLUECOUNTTEMPBOTMID = BLUECOUNTBOTMID;
			end
			else if (VGA_PIXEL_Y == 10'd102 && (VGA_PIXEL_X>=10'd48 && VGA_PIXEL_X<=10'd128)) begin
				if (PIXEL_IN[7:6] > PIXEL_IN[1:0] && PIXEL_IN[7:6] > PIXEL_IN[4:3]) begin
					REDCOUNTBOTTOM = REDCOUNTBOTTOM + 1'b1;
				end
				else if (PIXEL_IN[1:0] > PIXEL_IN[7:6] && PIXEL_IN[1:0] > PIXEL_IN[4:3]) begin
					BLUECOUNTBOTTOM = BLUECOUNTBOTTOM + 1'b1;
				end
				REDCOUNTTEMPBOTTOM = REDCOUNTBOTTOM;
				BLUECOUNTTEMPBOTTOM = BLUECOUNTBOTTOM;
			end
		end
end


endmodule