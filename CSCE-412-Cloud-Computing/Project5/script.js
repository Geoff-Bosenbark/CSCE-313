// Toggles the style and saves the selection to localStorage
function toggleStyle() {
    var styleSheet = document.getElementById('styleLink');
    var professionalContent = document.querySelectorAll('.professional');
    var personalContent = document.querySelectorAll('.personal');

    if (styleSheet.getAttribute('href') === 'style.css') {
        styleSheet.setAttribute('href', 'alt_style.css');
        professionalContent.forEach(function (content) {
            content.style.display = 'none';
        });
        personalContent.forEach(function (content) {
            content.style.display = 'block';
        });
        // Save the current style to localStorage
        localStorage.setItem('style', 'alt_style.css');
    } else {
        styleSheet.setAttribute('href', 'style.css');
        professionalContent.forEach(function (content) {
            content.style.display = 'block';
        });
        personalContent.forEach(function (content) {
            content.style.display = 'none';
        });
        // Save the current style to localStorage
        localStorage.setItem('style', 'style.css');
    }
    // Set a timeout to wait for the stylesheet to apply toprevent loading issues
    setTimeout(function () {
        showSlides(0); // Reinitialize the carousel
    }, 100); // amount
}

// Called when the document's DOM is fully parsed
document.addEventListener('DOMContentLoaded', function () {
    var style = localStorage.getItem('style') || "style.css";
    var styleSheet = document.getElementById('styleLink');
    var professionalContent = document.querySelectorAll('.professional');
    var personalContent = document.querySelectorAll('.personal');

    // Function to set the display style for content
    function updateContentDisplay(style) {
        professionalContent.forEach(function (content) {
            content.style.display = style === 'alt_style.css' ? 'none' : 'block';
        });
        personalContent.forEach(function (content) {
            content.style.display = style === 'alt_style.css' ? 'block' : 'none';
        });
    }

    // Set the correct stylesheet based on localStorage
    if (style) {
        styleSheet.setAttribute('href', style);
        updateContentDisplay(style);

        // Ensure carousel is initialized after stylesheet switch
        setTimeout(function () {
            showSlides(0);
        }, 0); // Timeout can be set to 0 to queue the function after stylesheet is applied
    } else {
        // Default behavior if no style is set in localStorage
        showSlides(0); // Assuming the default style doesn't require specific timing
    }
});



let slideIndex = 0;
showSlides(slideIndex);

function moveSlides(n) {
    showSlides(slideIndex += n);
}

function showSlides(n) {
    let i;
    let slides = document.getElementsByClassName("slide");
    if (n >= slides.length) { slideIndex = 0 }
    if (n < 0) { slideIndex = slides.length - 1 }
    for (i = 0; i < slides.length; i++) {
        slides[i].style.display = "none";
    }
    slides[slideIndex].style.display = "block";
}

function showPopup() {
    var slides = document.getElementsByClassName('slide');
    var popup = document.getElementById('popup');
    var popupDescription = document.getElementById('popupDescription');

    // Get the alt attribute of the image in the current slide
    var description = slides[slideIndex].getElementsByTagName('img')[0].alt;
    popupDescription.textContent = description;
    popup.style.display = 'block';
    document.getElementById('overlay').style.display = 'block';
}

// Adjust hidePopup function to hide the popup
function hidePopup() {
    document.getElementById('popup').style.display = 'none';
    document.getElementById('overlay').style.display = 'none';
}