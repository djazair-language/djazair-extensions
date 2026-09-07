/**
 * Djazair Clipboard Documentation Script (jQuery)
 * Author: Harizi Riyadh (hariziriyadh@gmail.com)
 * Copyright (c) 2026 Djazair Language Project
 */

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

    // Filter Sections
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
        '<div class="no-results-msg alert-box warn">' +
        '<i class="fas fa-exclamation-triangle"></i>' +
        '<div><strong>No matching APIs found for "' + $("<div>").text(query).html() + '".</strong> Try searching for "copy", "paste", "sequenceNumber", "clear", "formats", or "setText".</div>' +
        '</div>'
      );
    }
  });

  // ── 2. Code Copy to Clipboard ──────────────────────────────────────────────
  $(document).on("click", ".btn-copy", function() {
    var $btn = $(this);
    var $codeBlock = $btn.closest(".code-wrapper").find(".code-body pre code");
    var codeText = $codeBlock.text();

    if (navigator.clipboard && navigator.clipboard.writeText) {
      navigator.clipboard.writeText(codeText).then(function() {
        showCopySuccess($btn);
      });
    } else {
      var $temp = $("<textarea>");
      $("body").append($temp);
      $temp.val(codeText).select();
      document.execCommand("copy");
      $temp.remove();
      showCopySuccess($btn);
    }
  });

  function showCopySuccess($btn) {
    var origHtml = $btn.html();
    $btn.html('<i class="fas fa-check" style="color:var(--accent-emerald);"></i> Copied!').css("color", "var(--accent-emerald)");
    setTimeout(function() {
      $btn.html(origHtml).css("color", "");
    }, 2000);
  }

  // ── 3. Tabbed Code Switcher ────────────────────────────────────────────────
  $(document).on("click", ".tab-btn", function() {
    var $btn = $(this);
    var targetTab = $btn.data("tab");
    var $container = $btn.closest(".tab-container");

    $container.find(".tab-btn").removeClass("active");
    $btn.addClass("active");

    $container.find(".tab-pane").removeClass("active");
    $container.find(".tab-pane[data-pane='" + targetTab + "']").addClass("active");
  });

  // ── 4. Scrollspy Navigation ────────────────────────────────────────────────
  var $sections = $(".doc-section");
  var $navLinks = $(".nav-item");

  $(window).on("scroll", function() {
    var scrollPos = $(window).scrollTop() + 120;

    $sections.each(function() {
      var top = $(this).offset().top;
      var bottom = top + $(this).outerHeight();
      var id = $(this).attr("id");

      if (scrollPos >= top && scrollPos <= bottom) {
        $navLinks.removeClass("active");
        $('.nav-item[href="#' + id + '"]').addClass("active");
      }
    });
  });

  // ── 5. Dark / Light Theme Switcher ─────────────────────────────────────────
  var savedTheme = localStorage.getItem("djazair_clipboard_theme");
  if (savedTheme) {
    $("html").attr("data-theme", savedTheme);
    updateThemeIcon(savedTheme);
  }

  $("#themeToggleBtn").on("click", function() {
    var currentTheme = $("html").attr("data-theme");
    var nextTheme = (currentTheme === "light") ? "dark" : "light";
    $("html").attr("data-theme", nextTheme);
    localStorage.setItem("djazair_clipboard_theme", nextTheme);
    updateThemeIcon(nextTheme);
  });

  function updateThemeIcon(theme) {
    if (theme === "light") {
      $("#themeToggleBtn").html('<i class="fas fa-moon"></i>');
    } else {
      $("#themeToggleBtn").html('<i class="fas fa-sun"></i>');
    }
  }

  // Mobile sidebar toggle
  $("#mobileMenuBtn").on("click", function() {
    $(".sidebar").toggleClass("open");
  });

  // ── 6. Live Interactive Clipboard Simulator ────────────────────────────────
  var simState = {
    buffer: "Hello from Djazair! 🇩🇿",
    sequence: 142,
    formats: ["unicode", "text", "locale"],
    hasText: true
  };

  function updateSimUI() {
    $("#simStatusHasText").text(simState.hasText ? "True (Text Available)" : "False (Empty)").css("color", simState.hasText ? "var(--accent-emerald)" : "var(--accent-rose)");
    $("#simStatusSeq").text(simState.sequence);
    $("#simStatusBytes").text(new TextEncoder().encode(simState.buffer).length + " bytes");

    var $formatBox = $("#simFormatTags").empty();
    if (simState.formats.length === 0) {
      $formatBox.append('<span style="color:var(--text-muted);font-size:12px;">(No formats available - clipboard empty)</span>');
    } else {
      simState.formats.forEach(function(fmt) {
        $formatBox.append('<span class="format-pill">' + fmt + '</span>');
      });
    }

    $("#simVirtualDisplay").text(simState.buffer ? simState.buffer : "(empty)");
  }

  // Initial UI Render
  updateSimUI();

  // Copy Action
  $("#btnSimCopy").on("click", function() {
    var text = $("#simInput").val();
    simState.buffer = text;
    simState.sequence++;
    simState.hasText = (text.length > 0);
    simState.formats = simState.hasText ? ["unicode", "text", "locale"] : [];

    updateSimUI();
    updateGeneratedCode('clipboard.copy("' + escapeDjazairString(text) + '")');

    // Visual feedback
    $(this).addClass("sim-btn-primary").css("filter", "brightness(1.3)");
    setTimeout(function() {
      $("#btnSimCopy").css("filter", "");
    }, 300);
  });

  // Paste Action
  $("#btnSimPaste").on("click", function() {
    $("#simPasteTarget").val(simState.buffer);
    updateGeneratedCode('let content = clipboard.paste()\nprint(content)');

    $("#simPasteTarget").css("border-color", "var(--accent-emerald)");
    setTimeout(function() {
      $("#simPasteTarget").css("border-color", "");
    }, 400);
  });

  // Clear Action
  $("#btnSimClear").on("click", function() {
    simState.buffer = "";
    simState.sequence++;
    simState.hasText = false;
    simState.formats = [];

    updateSimUI();
    updateGeneratedCode('clipboard.clear()');
  });

  // Preset Buttons
  $(".btn-preset").on("click", function() {
    var preset = $(this).data("preset");
    var val = "";

    switch (preset) {
      case "arabic":
        val = "مرحبا بكم في لغة البرمجة جزائر — الجزائر 🇩🇿";
        break;
      case "code":
        val = 'use clipboard\n\nclipboard.copy("Generated Snippet")\nprint(clipboard.paste())';
        break;
      case "json":
        val = '{\n  "extension": "clipboard",\n  "version": "0.2.0",\n  "features": ["win32-retry", "sequence", "formats"]\n}';
        break;
      case "emojis":
        val = "🇩🇿 🚀 ✨ ⚡ 🎯 💎 🔥 💻 🌟";
        break;
    }

    $("#simInput").val(val);
  });

  function escapeDjazairString(str) {
    return str.replace(/"/g, '\\"').replace(/\n/g, '\\n');
  }

  function updateGeneratedCode(snippet) {
    var fullCode = "use clipboard\n\n" + snippet;
    $("#simGeneratedCode").text(fullCode);
  }
});
