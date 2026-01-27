// Documentation navigation and iframe management
document.addEventListener("DOMContentLoaded", function () {
  const sidebar = document.querySelector(".sidebar");
  const contentFrame = document.getElementById("content-frame");

  if (sidebar && contentFrame) {
    // Handle navigation clicks
    sidebar.addEventListener("click", function (e) {
      if (e.target.tagName === "A" && e.target.hasAttribute("data-content")) {
        e.preventDefault();

        // Update active state
        sidebar
          .querySelectorAll("a")
          .forEach((link) => link.classList.remove("active"));
        e.target.classList.add("active");

        // Load content
        const contentPath = e.target.getAttribute("data-content");
        contentFrame.src = contentPath;
      }
    });
  }

  // Handle messages from iframe for hiding navigation items
  window.addEventListener("message", function (event) {
    if (event.data && event.data.type === "hideApiTestConsoleNav") {
      const apiTestNav = document.getElementById("api-test-nav");
      if (apiTestNav) {
        apiTestNav.style.display = "none";
      }
    }

    if (event.data && event.data.type === "hideLoadTestConsoleNav") {
      const loadTestNav = document.getElementById("load-test-nav");
      if (loadTestNav) {
        loadTestNav.style.display = "none";
      }
    }
  });
});
