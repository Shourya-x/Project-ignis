#!/bin/bash

# IGNIS Website - Create complete website structure

echo "🔥 Creating IGNIS Project Website..."

# Create directories
mkdir -p css js assets/{images/{rocket_renders,pcb_renders,wiring_diagrams},videos,downloads/{firmware,cad_files,datasheets},3d_models} docs

# Create HTML files
touch index.html about.html hardware.html software.html gallery.html downloads.html

# Create CSS and JS
touch css/style.css
touch js/main.js

# Create documentation files
touch docs/{getting-started.md,wiring-guide.md,calibration.md,api-reference.md}

# Create README
touch README.md

echo "✅ Website structure created!"
echo ""
echo "📁 Structure:"
tree -L 2 2>/dev/null || ls -R