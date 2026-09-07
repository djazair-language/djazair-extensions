$(document).ready(function() {
  // ── 1. Real-Time API Search Filter ──────────────────────────────────────────
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

    // Handle No Results Feedback
    $(".no-results-msg").remove();
    if (matchCount === 0) {
      $("#contentArea").prepend(
        '<div class="no-results-msg alert-box warn">' +
        '<i class="fas fa-exclamation-triangle"></i>' +
        '<div><strong>No matching MySQL APIs or functions found.</strong> Try searching for "connect", "safeQuery", "beginTransaction", "fetchAssoc", "insertId", "ping", or "seek".</div>' +
        '</div>'
      );
    }
  });

  // ── 2. Code Copy to Clipboard ───────────────────────────────────────────────
  $(document).on("click", ".btn-copy", function() {
    var $btn = $(this);
    var $codeBlock = $btn.closest(".code-wrapper").find(".code-body pre");
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

  // ── 3. Tabbed Code Switcher ─────────────────────────────────────────────────
  $(document).on("click", ".tab-btn", function() {
    var $btn = $(this);
    var targetTab = $btn.data("tab");
    var $container = $btn.closest(".tab-container");

    $container.find(".tab-btn").removeClass("active");
    $btn.addClass("active");

    $container.find(".tab-pane").removeClass("active");
    $container.find(".tab-pane[data-pane='" + targetTab + "']").addClass("active");
  });

  // ── 4. Scrollspy Navigation ─────────────────────────────────────────────────
  var $sections = $(".doc-section");
  var $navLinks = $(".nav-item");

  $(window).on("scroll", function() {
    var scrollPos = $(window).scrollTop() + 140;

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

  // ── 5. Dark / Light Theme Switcher ──────────────────────────────────────────
  var savedTheme = localStorage.getItem("djazair_mysql_docs_theme") || "dark";
  applyTheme(savedTheme);

  $("#themeToggleBtn").on("click", function() {
    var currentTheme = $("html").attr("data-theme");
    var newTheme = (currentTheme === "light") ? "dark" : "light";
    applyTheme(newTheme);
    localStorage.setItem("djazair_mysql_docs_theme", newTheme);
  });

  function applyTheme(theme) {
    $("html").attr("data-theme", theme);
    if (theme === "light") {
      $("#themeToggleBtn").html('<i class="fas fa-moon"></i>');
    } else {
      $("#themeToggleBtn").html('<i class="fas fa-sun"></i>');
    }
  }

  // Mobile sidebar drawer
  $("#mobileMenuBtn").on("click", function() {
    $(".sidebar").toggleClass("open");
  });

  // ── 6. Interactive MySQL Simulator Engine ───────────────────────────────────
  var mockDatabase = {
    connected: true,
    serverVersion: "8.0.36-MySQL Community Server (GPL)",
    hostInfo: "localhost via TCP/IP",
    charset: "utf8mb4",
    threadId: 1042,
    protoVersion: 10,
    inTransaction: false,
    nextId: 6,
    users: [
      { id: 1, name: "Riad Harizi", email: "riad@djazair.dz", balance: 2500.0, status: "Active" },
      { id: 2, name: "Amine Brahimi", email: "amine@example.com", balance: 180.5, status: "Active" },
      { id: 3, name: "Sara Boualem", email: "sara@domain.dz", balance: 920.0, status: "Pending" },
      { id: 4, name: "Nour Eddine", email: "nour@tech.dz", balance: 430.25, status: "Active" },
      { id: 5, name: "Yacine Mansouri", email: "yacine@algiers.dz", balance: 3100.0, status: "Active" }
    ]
  };

  // Pre-configured Scenarios
  var scenarios = {
    "select_filter": {
      sql: "SELECT id, name, email, balance FROM users WHERE balance >= ? AND status = ?",
      params: "[500.0, \"Active\"]",
      dzCode: 'let res = db.safeQuery("SELECT id, name, email, balance FROM users WHERE balance >= ? AND status = ?", [500.0, "Active"])\nif !isNull(res)\n    let rows = res.fetchAllAssoc()\n    for r in rows\n        print(r["name"] + " -> $" + str(r["balance"]))\n    end\n    res.close()\nend'
    },
    "safe_insert": {
      sql: "INSERT INTO users (name, email, balance, status) VALUES (?, ?, ?, ?)",
      params: "[\"Farid Khelifi\", \"farid@casbah.dz\", 1250.0, \"Active\"]",
      dzCode: 'let ok = db.safeExecute(\n    "INSERT INTO users (name, email, balance, status) VALUES (?, ?, ?, ?)",\n    ["Farid Khelifi", "farid@casbah.dz", 1250.0, "Active"]\n)\nif ok\n    print("Inserted user with ID: " + str(db.insertId()))\n    print("Affected rows: " + str(db.affectedRows()))\nend'
    },
    "safe_update": {
      sql: "UPDATE users SET balance = balance + ? WHERE status = ?",
      params: "[100.0, \"Active\"]",
      dzCode: 'let ok = db.safeExecute("UPDATE users SET balance = balance + ? WHERE status = ?", [100.0, "Active"])\nprint("Updated rows: " + str(db.affectedRows()))'
    },
    "injection_defense": {
      sql: "SELECT * FROM users WHERE email = ? AND status = 'Active'",
      params: "[\"admin' OR '1'='1\"]",
      dzCode: '# Input string with SQL injection attempt is automatically escaped safely:\nlet maliciousInput = "admin\' OR \'1\'=\'1"\nlet res = db.safeQuery("SELECT * FROM users WHERE email = ? AND status = \'Active\'", [maliciousInput])\n# Result safely returns 0 rows (escaped to: \'admin\\\' OR \\\'1\\\'=\\\'1\') without breach!'
    },
    "transaction_flow": {
      sql: "BEGIN TRANSACTION -> INSERT ... -> COMMIT",
      params: "[]",
      dzCode: 'if db.beginTransaction()\n    db.safeExecute("INSERT INTO users (name, email, balance, status) VALUES (?, ?, ?, ?)", ["Zaki", "zaki@dz.dz", 800.0, "Active"])\n    db.safeExecute("UPDATE users SET balance = balance - 800.0 WHERE id = ?", [1])\n    \n    if db.commit()\n        print("ACID Transaction safely committed!")\n    else\n        db.rollback()\n    end\nend'
    },
    "server_diagnostics": {
      sql: "SHOW STATUS / SERVER DIAGNOSTICS",
      params: "[]",
      dzCode: 'print("Connected: " + str(db.isConnected()))\nprint("Server Info: " + db.serverInfo())\nprint("Host Info: " + db.hostInfo())\nprint("Thread ID: " + str(db.threadId()))\nprint("Client Info: " + db.clientInfo())\nprint("Server Stat: " + db.stat())\nprint("Protocol: " + str(db.protocolInfo()))'
    }
  };

  // Scenario Selector Change
  $("#simScenario").on("change", function() {
    var key = $(this).val();
    if (scenarios[key]) {
      $("#simSql").val(scenarios[key].sql);
      $("#simParams").val(scenarios[key].params);
      $("#simDzCode").text(scenarios[key].dzCode);
    }
  });

  // Execute Simulated Query
  $("#simRunBtn").on("click", function() {
    var scenarioKey = $("#simScenario").val();
    var sql = $("#simSql").val().trim();
    var paramsStr = $("#simParams").val().trim();
    var startTime = performance.now();

    var params = [];
    try {
      if (paramsStr !== "") {
        params = JSON.parse(paramsStr);
      }
    } catch (e) {
      alert("Invalid JSON parameters format! Example: [100, \"Active\"]");
      return;
    }

    var resultRows = [];
    var affected = 0;
    var insertId = 0;
    var statusMsg = "Query OK";

    // Simulate query logic
    if (scenarioKey === "server_diagnostics") {
      resultRows = [
        { Diagnostic: "Connection State", Value: "Connected (Live Socket)" },
        { Diagnostic: "Server Version", Value: mockDatabase.serverVersion },
        { Diagnostic: "Host Info", Value: mockDatabase.hostInfo },
        { Diagnostic: "Active Thread ID", Value: mockDatabase.threadId },
        { Diagnostic: "Protocol Version", Value: mockDatabase.protoVersion },
        { Diagnostic: "Default Charset", Value: mockDatabase.charset },
        { Diagnostic: "Client Library", Value: "libmysqlclient 8.0 / Djazair Native" }
      ];
      affected = 0;
    } else if (scenarioKey === "injection_defense") {
      // Safe parameterized query renders no match because the literal string is escaped
      resultRows = [];
      affected = 0;
      statusMsg = "Safe Parameterization: Escaped maliciously formed parameter, 0 matches.";
    } else if (scenarioKey === "safe_insert") {
      var newName = params[0] || "New User";
      var newEmail = params[1] || "user@example.com";
      var newBal = parseFloat(params[2]) || 0.0;
      var newStat = params[3] || "Active";
      insertId = mockDatabase.nextId++;
      mockDatabase.users.push({
        id: insertId,
        name: newName,
        email: newEmail,
        balance: newBal,
        status: newStat
      });
      affected = 1;
      resultRows = [{ "status": "Success", "new_id": insertId, "name": newName, "email": newEmail }];
    } else if (scenarioKey === "safe_update") {
      var inc = parseFloat(params[0]) || 0;
      var st = params[1] || "Active";
      mockDatabase.users.forEach(function(u) {
        if (u.status === st) {
          u.balance += inc;
          affected++;
        }
      });
      resultRows = mockDatabase.users.filter(function(u) { return u.status === st; });
    } else if (scenarioKey === "transaction_flow") {
      affected = 2;
      statusMsg = "Transaction: 2 queries executed -> COMMIT confirmed.";
      resultRows = [
        { Step: "1. BEGIN", Detail: "Autocommit disabled" },
        { Step: "2. INSERT", Detail: "New user staged with ID " + mockDatabase.nextId },
        { Step: "3. UPDATE", Detail: "Deducted balance from account #1" },
        { Step: "4. COMMIT", Detail: "Changes persisted to engine successfully" }
      ];
    } else {
      // Default: select_filter
      var minBal = (params.length > 0 && typeof params[0] === "number") ? params[0] : 0;
      var filterStat = (params.length > 1) ? params[1] : null;

      resultRows = mockDatabase.users.filter(function(u) {
        var matchBal = u.balance >= minBal;
        var matchStat = filterStat ? (u.status === filterStat) : true;
        return matchBal && matchStat;
      });
      affected = resultRows.length;
    }

    var endTime = performance.now();
    var duration = (endTime - startTime).toFixed(2);

    // Update Stats Bar
    $("#statAffected").text(affected);
    $("#statInsertId").text(insertId);
    $("#statDuration").text(duration + " ms");
    $("#statStatus").text(statusMsg);

    // Render Table Output
    renderSimTable(resultRows);

    // Render JSON Output
    $("#simJsonOutput").text(JSON.stringify(resultRows, null, 2));
  });

  function renderSimTable(rows) {
    var $wrap = $("#simResultTableWrap");
    if (!rows || rows.length === 0) {
      $wrap.html('<div style="padding:20px; text-align:center; color:var(--text-muted); font-size:13px;">(Empty result set - 0 rows returned)</div>');
      return;
    }

    var columns = Object.keys(rows[0]);
    var html = '<table class="sim-result-table"><thead><tr>';
    columns.forEach(function(col) {
      html += '<th>' + escapeHtml(col) + '</th>';
    });
    html += '</tr></thead><tbody>';

    rows.forEach(function(row) {
      html += '<tr>';
      columns.forEach(function(col) {
        var val = row[col];
        if (typeof val === "number" && col === "balance") {
          val = "$" + val.toFixed(2);
        }
        html += '<td>' + escapeHtml(String(val)) + '</td>';
      });
      html += '</tr>';
    });
    html += '</tbody></table>';
    $wrap.html(html);
  }

  function escapeHtml(str) {
    return $("<div>").text(str).html();
  }

  // Initial trigger for playground
  $("#simScenario").trigger("change");
  $("#simRunBtn").trigger("click");
});
