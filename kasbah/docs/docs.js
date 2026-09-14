// docs.js — Kasbah Framework Interactive Documentation
// Powered by jQuery 3.7.1

$(document).ready(function() {

  // ── 1. Real-Time Search Filter ─────────────────────────────────────────────
  $("#apiSearch").on("input", function() {
    var query = $(this).val().toLowerCase().trim();

    if (query === "") {
      $(".doc-section, .method-card, .api-table tbody tr, .nav-item").show();
      $(".no-results-msg").remove();
      return;
    }

    var matchCount = 0;

    // Filter Method Cards
    $(".method-card").each(function() {
      var text = $(this).text().toLowerCase();
      if (text.indexOf(query) !== -1) {
        $(this).show();
        matchCount++;
      } else {
        $(this).hide();
      }
    });

    // Filter Table Rows
    $(".api-table tbody tr").each(function() {
      var text = $(this).text().toLowerCase();
      if (text.indexOf(query) !== -1) {
        $(this).show();
        matchCount++;
      } else {
        $(this).hide();
      }
    });

    // Filter Navigation Items
    $(".nav-item").each(function() {
      var text = $(this).text().toLowerCase();
      if (text.indexOf(query) !== -1) {
        $(this).show();
      } else {
        $(this).hide();
      }
    });

    // Filter Sections (show if has visible child cards or title matches)
    $(".doc-section").each(function() {
      var hasVisibleCards = $(this).find(".method-card:visible, tr:visible").length > 0;
      var titleText = $(this).find(".section-title").text().toLowerCase();
      if (hasVisibleCards || titleText.indexOf(query) !== -1) {
        $(this).show();
      } else {
        $(this).hide();
      }
    });

    // Handle No Results
    $(".no-results-msg").remove();
    if (matchCount === 0) {
      $("#contentArea").prepend(
        '<div class="no-results-msg alert-box alert-warning">' +
          '<i class="fas fa-exclamation-triangle"></i>' +
          '<div>' +
            '<strong>No matching APIs found</strong>' +
            '<p>No methods, classes, or properties matched "<em>' + $('<div>').text(query).html() + '</em>". Try searching for "get", "status", "json", "helmet", "cors", "session", or "uploader".</p>' +
          '</div>' +
        '</div>'
      );
    }
  });

  // ── 2. Theme Toggle (Dark / Light Mode) ─────────────────────────────────────
  var currentTheme = localStorage.getItem("kasbah_docs_theme") || "dark";
  $("html").attr("data-theme", currentTheme);
  updateThemeIcon(currentTheme);

  $("#themeToggleBtn").on("click", function() {
    var newTheme = $("html").attr("data-theme") === "dark" ? "light" : "dark";
    $("html").attr("data-theme", newTheme);
    localStorage.setItem("kasbah_docs_theme", newTheme);
    updateThemeIcon(newTheme);
  });

  function updateThemeIcon(theme) {
    var icon = $("#themeToggleBtn i");
    if (theme === "dark") {
      icon.removeClass("fa-sun").addClass("fa-moon");
    } else {
      icon.removeClass("fa-moon").addClass("fa-sun");
    }
  }

  // ── 3. Mobile Navigation Drawer ────────────────────────────────────────────
  $("#mobileMenuBtn").on("click", function() {
    $(".sidebar").toggleClass("open");
  });

  $(document).on("click", function(e) {
    if (!$(e.target).closest(".sidebar, #mobileMenuBtn").length) {
      $(".sidebar").removeClass("open");
    }
  });

  // ── 4. Scrollspy / Active Sidebar Navigation ───────────────────────────────
  $(window).on("scroll", function() {
    var scrollPos = $(window).scrollTop() + 100;
    $(".doc-section").each(function() {
      var top = $(this).offset().top;
      var height = $(this).outerHeight();
      var id = $(this).attr("id");

      if (scrollPos >= top && scrollPos < top + height) {
        $(".nav-item").removeClass("active");
        $('.nav-item[href="#' + id + '"]').addClass("active");
      }
    });
  });

  // ── 5. Copy Code to Clipboard ──────────────────────────────────────────────
  $(document).on("click", ".copy-btn", function() {
    var btn = $(this);
    var codeBlock = btn.closest(".code-container").find("pre");
    var textToCopy = codeBlock.text().trim();

    if (navigator.clipboard) {
      navigator.clipboard.writeText(textToCopy).then(function() {
        showCopiedFeedback(btn);
      });
    } else {
      var tempTextArea = $("<textarea>").val(textToCopy).appendTo("body").select();
      document.execCommand("copy");
      tempTextArea.remove();
      showCopiedFeedback(btn);
    }
  });

  function showCopiedFeedback(btn) {
    var origHtml = btn.html();
    btn.html('<i class="fas fa-check" style="color:var(--accent-emerald);"></i> Copied!').css("border-color", "var(--accent-emerald)");
    setTimeout(function() {
      btn.html(origHtml).css("border-color", "");
  }

});

