(function() {
  if (window.djazair) return;

  window.djazair = {
    // ── Event listeners for push notifications from Djazair (bridge.send)
    _local: {},

    // ─────────────────────────────────────────────────────────────────────
    // invoke(channel, payload) → Promise<any>
    //
    // Calls a Djazair handler by channel name and returns a Promise that
    // resolves with the handler's return value (auto-deserialized from JSON).
    //
    // Example:
    //   const user = await window.djazair.invoke('getUser', { id: 42 });
    //   console.log(user.name);
    // ─────────────────────────────────────────────────────────────────────
    invoke: function(channel, payload) {
      var data = (payload !== undefined && payload !== null) ? payload : null;
      // __dz_invoke is a webview.bind Promise — resolves with the parsed JSON
      // return value from the Djazair dispatcher (_onDispatch in bridge.dz).
      return __dz_invoke(channel, data).then(function(result) {
        // The dispatcher wraps results in { __dz_ok, __dz_data } envelope.
        if (result !== null && typeof result === 'object') {
          if (result.__dz_ok === false) {
            return Promise.reject(new Error(result.__dz_error || 'Handler error'));
          }
          if ('__dz_data' in result) {
            return result.__dz_data;
          }
        }
        return result;
      });
    },

    // ─────────────────────────────────────────────────────────────────────
    // on(channel, callback)
    //
    // Subscribe to push events sent from Djazair via bridge.send().
    // Multiple listeners per channel are supported.
    //
    // Example:
    //   window.djazair.on('notification', function(data) {
    //     console.log(data.message);
    //   });
    // ─────────────────────────────────────────────────────────────────────
    on: function(channel, callback) {
      if (!this._local[channel]) {
        this._local[channel] = [];
      }
      this._local[channel].push(callback);
    },

    // ─────────────────────────────────────────────────────────────────────
    // off(channel [, callback])
    //
    // Remove a specific listener, or all listeners if callback is omitted.
    // ─────────────────────────────────────────────────────────────────────
    off: function(channel, callback) {
      if (!this._local[channel]) return;
      if (!callback) {
        delete this._local[channel];
        return;
      }
      this._local[channel] = this._local[channel].filter(function(cb) {
        return cb !== callback;
      });
    },

    // ─────────────────────────────────────────────────────────────────────
    // send(channel, data)  [internal — called by Djazair via win.eval()]
    //
    // Dispatches data to all JS listeners registered via .on().
    // Data is already a parsed JS object (the JSON literal is eval'd by JS).
    // ─────────────────────────────────────────────────────────────────────
    send: function(channel, data) {
      var listeners = this._local[channel];
      if (listeners && listeners.length > 0) {
        listeners.forEach(function(cb) { cb(data); });
      }
    }
  };
})();

