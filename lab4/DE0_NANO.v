`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144

///////* DON'T CHANGE THIS PART *///////
module DE0_NANO(
	CLOCK_50,
	GPIO_0_D,
	GPIO_1_D,
	KEY
);

//=======================================================
//  PARAMETER declarations
//=======================================================
localparam RED = 8'b111_000_00;
localparam GREEN = 8'b000_111_00;
localparam BLUE = 8'b000_000_11;

//=======================================================
//  PORT declarations
//=======================================================

//////////// CLOCK - DON'T NEED TO CHANGE THIS //////////
input 		          		CLOCK_50;

//////////// GPIO_0, GPIO_0 connect to GPIO Default //////////
output 		    [33:0]		GPIO_0_D;
//////////// GPIO_0, GPIO_1 connect to GPIO Default //////////
input 		    [33:0]		GPIO_1_D;
input 		     [1:0]		KEY;

///// PIXEL DATA /////
reg [7:0]	pixel_data_RGB332;// ={R,G,B}; //= 8'b000_111_00;
reg BYTE_NUM;

///// READ/WRITE ADDRESS /////
reg [14:0] X_ADDR;
reg [14:0] Y_ADDR;
wire [14:0] WRITE_ADDRESS;
reg [14:0] READ_ADDRESS;
reg RESET;

assign WRITE_ADDRESS = X_ADDR + Y_ADDR*(`SCREEN_WIDTH);
//assign WRITE_ADDRESS = 1'd1 + READ_ADDRESS;
//assign WRITE_ADDRESS = VSYNC? 15'b0: BYTE_NUM? WRITE_ADDRESS +1'b1: WRITE_ADDRESS; 



///// VGA INPUTS/OUTPUTS /////
wire 			VGA_RESET;
wire [7:0]	VGA_COLOR_IN;
wire [9:0]	VGA_PIXEL_X;
wire [9:0]	VGA_PIXEL_Y;
wire [7:0]	MEM_OUTPUT;
wire			VGA_VSYNC_NEG;
wire			VGA_HSYNC_NEG;
reg			VGA_READ_MEM_EN;

assign GPIO_0_D[5] = VGA_VSYNC_NEG;
assign VGA_RESET = ~KEY[0];

///// I/O for Img Proc /////
wire [8:0] RESULT;
wire PCLK, HREF, VSYNC;
reg HREF_PREV;
reg VSYNC_PREV;
//wire [2:0] R;
//wire [2:0] G;
//wire [1:0] B;
assign PCLK = GPIO_1_D[28];
assign HREF = GPIO_1_D[29];
assign VSYNC = GPIO_1_D[30];
//assign R = {GPIO_1_D[27],GPIO_1_D[26], GPIO_1_D[25]}; //D[7:5]
//assign G = {GPIO_1_D[22],GPIO_1_D[21], GPIO_1_D[20]}; //D[2:0]
//assign B = {GPIO_1_D[24],GPIO_1_D[23]}; //D[4:3]
reg [7:0] BG_COLOR;
wire [7:0] CAM_INPUT = {GPIO_1_D[2],GPIO_1_D[4], GPIO_1_D[6],GPIO_1_D[8],GPIO_1_D[10],GPIO_1_D[12],GPIO_1_D[14],GPIO_1_D[16]};
reg [15:0] TEMP;
assign GPIO_0_D[28] = (RESULT==3'b111)? 1'b1: 1'b0; //BLUE
assign GPIO_0_D[30] = (RESULT==3'b110)? 1'b1: 1'b0; //RED


/* WRITE ENABLE */
reg W_EN;

///////* CREATE ANY LOCAL WIRES YOU NEED FOR YOUR PLL *///////
wire c0_24;
wire c1_25;
wire c2_50;
assign GPIO_0_D[26] = c0_24;

///////* INSTANTIATE YOUR PLL HERE *///////
PLL_Lab4	pll (
	.inclk0 (CLOCK_50),
	.c0 (c0_24), //Camera
	.c1 (c1_25), //VGA
	.c2 (c2_50)  
	);


	///////* M9K Module *///////
Dual_Port_RAM_M9K mem(
	.input_data(pixel_data_RGB332),
	.w_addr(WRITE_ADDRESS),
	.r_addr(READ_ADDRESS),
	.w_en(W_EN),
	.clk_W(c2_50),
	.clk_R(c1_25), // DO WE NEED TO READ SLOWER THAN WRITE??
	.output_data(MEM_OUTPUT)
);

///////* VGA Module *///////
VGA_DRIVER driver (
	.RESET(VGA_RESET),
	.CLOCK(c1_25),
	.PIXEL_COLOR_IN(VGA_READ_MEM_EN ? MEM_OUTPUT : BLUE),
	.PIXEL_X(VGA_PIXEL_X),
	.PIXEL_Y(VGA_PIXEL_Y),
	.PIXEL_COLOR_OUT({GPIO_0_D[9],GPIO_0_D[11],GPIO_0_D[13],GPIO_0_D[15],GPIO_0_D[17],GPIO_0_D[19],GPIO_0_D[21],GPIO_0_D[23]}),
   .H_SYNC_NEG(GPIO_0_D[7]),
   .V_SYNC_NEG(VGA_VSYNC_NEG)
);

///////* Image Processor *///////
IMAGE_PROCESSOR proc(
	.PIXEL_IN(MEM_OUTPUT),
	.CLK(c1_25),
	.VGA_PIXEL_X(VGA_PIXEL_X),
	.VGA_PIXEL_Y(VGA_PIXEL_Y),
	//.VGA_VSYNC_NEG(VGA_VSYNC_NEG),
	.VSYNC(VSYNC),
	.HREF(HREF),
	.RESULT(RESULT)
);

///////* Update Read Write Address *///////
always @ (VGA_PIXEL_X, VGA_PIXEL_Y) begin
		READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
		if(VGA_PIXEL_X>(`SCREEN_WIDTH-1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT-1))begin
				VGA_READ_MEM_EN = 1'b0;
				//W_EN = 1'b0;
		end
		else begin
				VGA_READ_MEM_EN = 1'b1;
				//W_EN =  1'b1;
		end
end

///////* READ CAMERA INPUT */////
always @(posedge PCLK) begin
	if (VSYNC & ~VSYNC_PREV) begin 
		BYTE_NUM=1'b0;
		X_ADDR = 15'b0;
		Y_ADDR = 15'b0;
		//BG_COLOR = 8'b111_111_00;

	end
	else if (~HREF & HREF_PREV) begin 
				Y_ADDR = Y_ADDR + 1'd1;
				X_ADDR = 15'b0;
				BYTE_NUM=1'b0;
	end
	else begin
		Y_ADDR = Y_ADDR;
		if(HREF) begin
			
			if(BYTE_NUM ==1'b0) begin
				W_EN = 1'b0;
				BYTE_NUM = 1'b1;
				TEMP[7:0] = CAM_INPUT; 
				pixel_data_RGB332[7:5] = CAM_INPUT[3:1];
				X_ADDR = X_ADDR;
			end
			else if (BYTE_NUM ==1'b1) begin
				TEMP[15:8] = CAM_INPUT;
				pixel_data_RGB332 = {TEMP[15:13] , TEMP[11:9] , TEMP[3:2]};
				//pixel_data_RGB332 = pixel_data_RGB332 & {3'b111,3'b111,B};
				BYTE_NUM = 1'b0;
				X_ADDR = X_ADDR + 1'b1;
				W_EN = 1'b1;
			end
		end
		else begin
			//BG_COLOR = 8'b000_000_11;
			X_ADDR = 15'b0;
			//BYTE_NUM = 1'b0;
			//W_EN = 1'b0;
			//BG_COLOR = 8'b111_000_00;
		end
	end
	HREF_PREV = HREF;
	VSYNC_PREV = VSYNC;
end

//always @(VSYNC) begin
	//if (VSYNC) RESET =1'b1;
	//else RESET = 1'b0;
//end
endmodule 
