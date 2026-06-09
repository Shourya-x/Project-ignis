// IGNIS Rocket - X-Configuration Fins
// Top-Down View (looking from nose to tail)
// Shows 4 fins at 45-degree angles (X shape)

// ============================================
// PARAMETERS
// ============================================

rocket_radius = 10;      // Body tube radius (mm)
fin_length = 25;         // Fin span from body (mm)
fin_thickness = 2;       // Fin thickness (mm)
fin_chord = 15;          // Fin root chord length (mm)

// X-configuration angles (from front axis)
// Fin 1: front-right (45°)
// Fin 2: front-left  (315° / -45°)
// Fin 3: rear-left   (225°)
// Fin 4: rear-right  (135°)

fin_angles = [45, 315, 225, 135];
fin_names = ["Fin 1 (Front-Right)", 
             "Fin 2 (Front-Left)", 
             "Fin 3 (Rear-Left)", 
             "Fin 4 (Rear-Right)"];

// Colors for each fin
fin_colors = ["red", "blue", "green", "orange"];

// ============================================
// MODULES
// ============================================

module rocket_body() {
    color("silver", 0.3)
    cylinder(h = 5, r = rocket_radius, center = true);
    
    // Outline for visibility
    color("black")
    cylinder(h = 5.1, r = rocket_radius, center = true);
}

module fin(angle, name, color_name) {
    rotate([0, 0, angle]) {
        // Fin as a trapezoid (tapered)
        color(color_name, 0.8)
        translate([rocket_radius, 0, 0])
        linear_extrude(height = fin_thickness, center = true)
        polygon(points = [
            [0, -fin_chord/2],           // Root trailing
            [0, fin_chord/2],            // Root leading
            [fin_length, fin_chord/3],   // Tip
            [fin_length, -fin_chord/3]   // Tip
        ]);
        
        // Label for fin number
        color("black")
        translate([rocket_radius + fin_length/2, -fin_chord/2 - 3, 0])
        linear_extrude(height = 0.5)
        text(str(name), size = 3, halign = "center");
    }
}

module direction_arrows() {
    // North arrow (front of rocket)
    color("red")
    translate([0, -rocket_radius - 15, 0])
    rotate([0, 0, 90])
    linear_extrude(height = 0.5)
    polygon(points = [[0, 5], [-3, -3], [3, -3]]);
    
    color("red")
    translate([0, -rocket_radius - 8, 0])
    linear_extrude(height = 0.5)
    text("FRONT", size = 4, halign = "center");
    
    // South arrow (rear)
    color("blue")
    translate([0, rocket_radius + 15, 0])
    rotate([0, 0, -90])
    linear_extrude(height = 0.5)
    polygon(points = [[0, 5], [-3, -3], [3, -3]]);
    
    color("blue")
    translate([0, rocket_radius + 8, 0])
    linear_extrude(height = 0.5)
    text("REAR", size = 4, halign = "center");
    
    // East arrow (right)
    color("gray")
    translate([rocket_radius + 25, 0, 0])
    linear_extrude(height = 0.5)
    polygon(points = [[5, 0], [-3, -3], [-3, 3]]);
    
    color("gray")
    translate([rocket_radius + 30, 0, 0])
    linear_extrude(height = 0.5)
    text("RIGHT", size = 4, halign = "center");
    
    // West arrow (left)
    color("gray")
    translate([-rocket_radius - 25, 0, 0])
    rotate([0, 0, 180])
    linear_extrude(height = 0.5)
    polygon(points = [[5, 0], [-3, -3], [-3, 3]]);
    
    color("gray")
    translate([-rocket_radius - 30, 0, 0])
    linear_extrude(height = 0.5)
    text("LEFT", size = 4, halign = "center");
}

module angle_indicators() {
    for (angle = [0, 45, 90, 135, 180, 225, 270, 315]) {
        color("gray", 0.3)
        rotate([0, 0, angle])
        translate([rocket_radius + fin_length + 5, 0, 0])
        linear_extrude(height = 0.2)
        circle(r = 1);
    }
    
    // 45° angle labels
    color("gray")
    rotate([0, 0, 45])
    translate([rocket_radius + fin_length + 8, 0, 0])
    linear_extrude(height = 0.3)
    text("45°", size = 2.5, halign = "center");
    
    color("gray")
    rotate([0, 0, 135])
    translate([rocket_radius + fin_length + 8, 0, 0])
    linear_extrude(height = 0.3)
    text("135°", size = 2.5, halign = "center");
    
    color("gray")
    rotate([0, 0, 225])
    translate([rocket_radius + fin_length + 8, 0, 0])
    linear_extrude(height = 0.3)
    text("225°", size = 2.5, halign = "center");
    
    color("gray")
    rotate([0, 0, 315])
    translate([rocket_radius + fin_length + 8, 0, 0])
    linear_extrude(height = 0.3)
    text("315°", size = 2.5, halign = "center");
}

module legend() {
    translate([-60, -40, 0]) {
        color("black")
        linear_extrude(height = 0.5)
        text("IGNIS X-Configuration Fins (Top View)", size = 5, halign = "left");
        
        for (i = [0:3]) {
            translate([0, -8 - i*8, 0]) {
                color(fin_colors[i])
                linear_extrude(height = 0.5)
                square(size = [5, 5]);
                
                color("black")
                translate([8, 2, 0])
                linear_extrude(height = 0.5)
                text(fin_names[i], size = 3, halign = "left");
            }
        }
        
        color("gray")
        translate([0, -40, 0])
        linear_extrude(height = 0.5)
        text("Fins at 45° angles → X shape", size = 3, halign = "left");
        
        color("red")
        translate([0, -48, 0])
        linear_extrude(height = 0.5)
        text("FRONT = Nose of rocket", size = 3, halign = "left");
    }
}

// ============================================
// MAIN RENDER
// ============================================

module x_config_fins() {
    // Rocket body
    rocket_body();
    
    // Add all 4 fins
    for (i = [0:3]) {
        fin(fin_angles[i], i+1, fin_colors[i]);
    }
    
    // Direction arrows
    direction_arrows();
    
    // Angle indicators
    angle_indicators();
    
    // Legend
    legend();
}

// Render the X-configuration
x_config_fins();