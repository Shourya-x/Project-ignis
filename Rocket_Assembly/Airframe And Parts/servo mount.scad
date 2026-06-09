// IGNIS 4-Servo Mount Ring with Base Plate (Cruciform / "+" Configuration)
// Fits inside 75mm PVC pipe (ID ≈ 73mm)
// Servo: standard 9g micro (e.g., SG90, MG90S)

// ========== PARAMETERS ==========
pipe_id = 73;               // inner diameter of 75mm pipe (mm)
ring_height = 8;            // thickness of the ring wall (mm)
base_thick = 3;             // thickness of the base plate (mm)
wall_thick = 3;             // wall thickness of the ring
servo_width = 23;           // servo width (mm)
servo_depth = 12;           // servo depth (mm)
screw_dia = 2.5;            // M2.5 screw hole diameter (mm)
screw_offset = 8;           // screw hole distance from center (mm)

// derived
ring_od = pipe_id - 0.5;    // outer diameter for snug fit (mm)
ring_id = ring_od - 2*wall_thick; // inner diameter
pocket_width = servo_width + 2;   // clearance
pocket_depth = servo_depth + 2;   // clearance
total_height = ring_height + base_thick;

// ========== MODULES ==========
module servo_pocket() {
    // cutout only through the ring portion (not the base)
    translate([0, 0, base_thick/2])
        cube([pocket_width, pocket_depth, ring_height + 1], center = true);
}

module screw_hole() {
    // screw hole goes through the base plate and ring
    cylinder(d = screw_dia, h = total_height + 1, center = true, $fn = 20);
}

module servo_mount_ring() {
    difference() {
        union() {
            // base plate (solid disk)
            cylinder(h = base_thick, d = ring_od, center = true, $fn = 128);
            // ring wall
            cylinder(h = ring_height, d = ring_od, center = true, $fn = 128);
            cylinder(h = ring_height + 2, d = ring_id, center = true, $fn = 128);
        }
        
        // cutout the inner part of the ring (but not the base)
        translate([0, 0, base_thick])
            cylinder(h = ring_height + 2, d = ring_id, center = true, $fn = 128);
        
        // 4 servo pockets at 0°, 90°, 180°, 270°
        for (angle = [0, 90, 180, 270]) {
            rotate([0, 0, angle]) {
                // servo pocket cutout (only through ring, not base)
                translate([0, ring_od/2 - pocket_depth/2, 0])
                    servo_pocket();
                
                // screw holes (two per servo, on either side of pocket)
                translate([-screw_offset, ring_od/2 - pocket_depth/2 - 2, 0])
                    screw_hole();
                translate([screw_offset, ring_od/2 - pocket_depth/2 - 2, 0])
                    screw_hole();
            }
        }
    }
}

// render
servo_mount_ring();