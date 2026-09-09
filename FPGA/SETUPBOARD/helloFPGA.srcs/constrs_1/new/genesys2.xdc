## Genesys 2 differential system clock
set_property -dict { PACKAGE_PIN AD12 IOSTANDARD LVDS } [get_ports { sysclk_p }]
set_property -dict { PACKAGE_PIN AD11 IOSTANDARD LVDS } [get_ports { sysclk_n }]

## 200 MHz clock
create_clock -period 5.000 -name sysclk [get_ports { sysclk_p }]

## LEDs
set_property -dict { PACKAGE_PIN T28 IOSTANDARD LVCMOS33 } [get_ports { led[0] }]
set_property -dict { PACKAGE_PIN V19 IOSTANDARD LVCMOS33 } [get_ports { led[1] }]
set_property -dict { PACKAGE_PIN U30 IOSTANDARD LVCMOS33 } [get_ports { led[2] }]
set_property -dict { PACKAGE_PIN U29 IOSTANDARD LVCMOS33 } [get_ports { led[3] }]
set_property -dict { PACKAGE_PIN V20 IOSTANDARD LVCMOS33 } [get_ports { led[4] }]
set_property -dict { PACKAGE_PIN V26 IOSTANDARD LVCMOS33 } [get_ports { led[5] }]
set_property -dict { PACKAGE_PIN W24 IOSTANDARD LVCMOS33 } [get_ports { led[6] }]
set_property -dict { PACKAGE_PIN W23 IOSTANDARD LVCMOS33 } [get_ports { led[7] }]


# inpout butons
#set_property -dict { PACKAGE_PIN E18 IOSTANDARD LVCMOS12 } [get_ports { btnc }]
#set_property -dict { PACKAGE_PIN B19   IOSTANDARD LVCMOS12 } [get_ports { btnu }];

set_property -dict { PACKAGE_PIN E18   IOSTANDARD LVCMOS12 } [get_ports { btnc }];
set_property -dict { PACKAGE_PIN M19   IOSTANDARD LVCMOS12 } [get_ports { btnd }]; 
set_property -dict { PACKAGE_PIN M20   IOSTANDARD LVCMOS12 } [get_ports { btnl }]; 
set_property -dict { PACKAGE_PIN C19   IOSTANDARD LVCMOS12 } [get_ports { btnr }]; 
set_property -dict { PACKAGE_PIN B19   IOSTANDARD LVCMOS12 } [get_ports { btnu }]; 

