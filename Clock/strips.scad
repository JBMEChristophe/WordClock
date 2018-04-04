os = 60;
led_offset = 8;
materialThickness = 3;
columns = 11;
rows = 10;

height = 25;
width = 500;

slot_start = os+led_offset;
slot_end = width - (os) - (led_offset);
h_distance_between_led = (slot_end-slot_start)/(columns-1);
v_distance_between_led = (slot_end-slot_start)/(rows-1);
module horizontal_strip()
{
   square([(2*os)+380, height],center = false);
}

module strip_cutout()
{
    square([materialThickness, height/2+1], center = false);
}


module draw_strip_cutout(){
      for(x = [hs_start : h_distance_between_led: hs_end]){
        echo(x);
        translate([x, 0,0]){
        strip_cutout();
      }
    }
}
echo (slot_start);
echo (slot_end);
echo (h_distance_between_led);
echo (v_distance_between_led);

hs_start = (slot_start + (h_distance_between_led/2)) - (materialThickness/2);
hs_end = slot_end;

difference(){
    //horizontal_strip();
    draw_strip_cutout();
    
}

