// IGNIS Project Website - Main JavaScript

// Smooth scrolling for anchor links
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        e.preventDefault();
        const target = document.querySelector(this.getAttribute('href'));
        if (target) {
            target.scrollIntoView({
                behavior: 'smooth',
                block: 'start'
            });
        }
    });
});

// Navbar background change on scroll
window.addEventListener('scroll', () => {
    const navbar = document.querySelector('.navbar');
    if (window.scrollY > 50) {
        navbar.style.background = 'rgba(5, 5, 5, 0.98)';
    } else {
        navbar.style.background = 'rgba(5, 5, 5, 0.95)';
    }
});

// Add active class to nav items based on scroll position
const sections = document.querySelectorAll('section');
const navItems = document.querySelectorAll('.nav-menu a');

window.addEventListener('scroll', () => {
    let current = '';
    sections.forEach(section => {
        const sectionTop = section.offsetTop;
        const sectionHeight = section.clientHeight;
        if (scrollY >= (sectionTop - 200)) {
            current = section.getAttribute('id');
        }
    });

    navItems.forEach(item => {
        item.classList.remove('active');
        if (item.getAttribute('href') === `#${current}`) {
            item.classList.add('active');
        }
    });
});

// Count-up animation for stats
const statNumbers = document.querySelectorAll('.stat-number');
let animated = false;

function animateNumbers() {
    if (animated) return;
    
    statNumbers.forEach(stat => {
        const target = parseInt(stat.innerText);
        let current = 0;
        const increment = target / 50;
        const updateNumber = () => {
            current += increment;
            if (current < target) {
                stat.innerText = Math.ceil(current);
                requestAnimationFrame(updateNumber);
            } else {
                stat.innerText = target;
            }
        };
        updateNumber();
    });
    animated = true;
}

// Trigger animation when stats section is in view
const statsSection = document.querySelector('.stats');
const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            animateNumbers();
            observer.unobserve(entry.target);
        }
    });
});

if (statsSection) {
    observer.observe(statsSection);
}

// Add loading animation
window.addEventListener('load', () => {
    document.body.classList.add('loaded');
});

// Console welcome message
console.log('%c🔥 IGNIS TVC System 🔥', 'color: #ff4500; font-size: 20px; font-weight: bold;');
console.log('%cAdvanced Thrust Vector Control for Model Rockets', 'color: #ff8c00; font-size: 14px;');
console.log('%cVisit github.com/yourusername/ignis for more info', 'color: #cccccc; font-size: 12px;');