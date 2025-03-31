document.addEventListener("DOMContentLoaded", function() {
  // Find the element with id 'projectnumber'
  var targetElem = document.getElementById("projectnumber");
  if (!targetElem) return;

  // Create a dropdown (select element)
  var select = document.createElement("select");
  select.id = "version-dropdown";

  // Fetch versions.json from the root of your site
  fetch(`${window.location.origin}/uniot-core/versions.json`)
    .then(function(response) {
      if (!response.ok) {
        throw new Error("Failed to fetch versions.json");
      }
      return response.json();
    })
    .then(function(data) {
      // The JSON should have a key "versions" that is an array of version strings
      var versions = data.versions || [];
      // Include "latest" as the first option
      versions.unshift("latest");

      // Sort versions by numeric values, ignoring 'v' prefix and keeping 'latest' at the top
      versions.sort(function(a, b) {
        if (a === "latest") return -1;
        if (b === "latest") return 1;

        // Remove 'v' prefix if it exists
        var verA = a.startsWith('v') ? a.substring(1) : a;
        var verB = b.startsWith('v') ? b.substring(1) : b;

        // Split version strings into components and convert to numbers
        var partsA = verA.split('.').map(Number);
        var partsB = verB.split('.').map(Number);

        // Compare each component
        for (var i = 0; i < Math.max(partsA.length, partsB.length); i++) {
          var numA = i < partsA.length ? partsA[i] : 0;
          var numB = i < partsB.length ? partsB[i] : 0;

          if (numA !== numB) {
            return numB - numA; // Descending order (newer versions first)
          }
        }

        return 0;
      });

      // Populate the dropdown with options
      versions.forEach(function(version) {
        var option = document.createElement("option");
        option.value = version;
        option.textContent = version;
        // Set as selected if the current URL indicates this version
        var currentPath = window.location.pathname;
        if ((version === "latest" && currentPath.includes("/latest/")) ||
            (version !== "latest" && currentPath.includes("/" + version + "/"))) {
          option.selected = true;
        }
        select.appendChild(option);
      });

      // Replace the target element's content with the dropdown
      targetElem.innerHTML = "";
      targetElem.appendChild(select);

      // Add an event listener to handle version changes
      select.addEventListener("change", function() {
        var selectedVersion = this.value;
        // Construct the URL for the selected version's documentation
        const getNewUrl = (selectedVersion) => `/uniot-core/${selectedVersion}/index.html`;

        // Check if the target documentation exists by sending a HEAD request
        fetch(getNewUrl(selectedVersion), { method: "HEAD" })
          .then((response) => {
            if (response.ok) {
              window.location.href = getNewUrl(selectedVersion);
            } else {
              alert("Documentation for version " + selectedVersion + " is not available.");
              this.value = "latest";
              window.location.href = getNewUrl(this.value);
            }
          })
          .catch((error) => {
            alert("Documentation for version " + selectedVersion + " is not available.");
            this.value = "latest";
            window.location.href = getNewUrl(this.value);
          });
      });
    })
    .catch(function(error) {
      console.error("Error loading versions:", error);
    });
});
