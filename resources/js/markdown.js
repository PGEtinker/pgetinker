import { conformStorage, getStorageValue, setStorageValue } from "./lib/storage";

conformStorage();

// Create a media query for prefers-color-scheme
const darkModeMediaQuery = window.matchMedia('(prefers-color-scheme: dark)');

// Function to handle theme change
function handleThemeChange(e) {
    setTimeout(() =>
    {
        const theme = getStorageValue("theme");
        let light = (theme === "light");

        document.body.classList.toggle("dark", !light);
        document.body.classList.toggle("light", light);
        
        let darkThemeStyle = document.querySelector("#dark-theme");
        let lightThemeStyle = document.querySelector("#light-theme");
        
        darkThemeStyle.disabled = light;
        lightThemeStyle.disabled = !light;
    }, 100);
}

// Add event listener for theme changes
darkModeMediaQuery.addEventListener('change', handleThemeChange);

handleThemeChange(darkModeMediaQuery);


