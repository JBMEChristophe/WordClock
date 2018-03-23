$fn=500;

module stand_base()
{
	cube([30,150,30]);
}

module stand_minus_slopes()
{
	difference()
	{
		stand_base();
		translate([-5,-0,60])
		slopes_cutout();
		translate([-5,150,60])
		slopes_cutout();
	}

}

module stand_minus_slopes_minus_frame()
{
	difference()
	{
		stand_minus_slopes();
		translate([-5,51.25,15])
		frame_cutout();
	}
}

module stand_complete()
{
	union()
	{
		stand_minus_slopes_minus_frame();
		translate([0,41.25,30])
		stand_riser();
		translate([0,98.75,30])
		stand_riser();
	}
}

module stand_cutaway()
{
	difference()
	{
		stand_complete();
		translate([-290,75,-5])
		cylinder_cutout();
		translate([320,75,-5])
		cylinder_cutout();
	}
}

module slopes_cutout()
{
	rotate([90,0,90])
	cylinder(r=51, h=40);
}

module frame_cutout()
{
	cube([40,47.5,50]);
}

module stand_riser()
{
	cube([30,10,20]);
}

module cylinder_cutout()
{
	cylinder(r=300,h=60);
}

stand_cutaway();

//translate([-290,75,-5])
//cylinder_cutout();
//translate([200,0,50])
//slopes_cutout();
//translate([100,0,0])
//frame_cutout();
//translate([-100,0,0])
//stand_riser();
