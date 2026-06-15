(function() {
  if (window.djazair) return;
  window.djazair = {
    _local: {},
    invoke: function(channel) {
      var args = Array.prototype.slice.call(arguments, 1);
      return __dz_invoke(channel, args);
    },
    on: function(channel, callback) {
      if (!this._local[channel]) {
        this._local[channel] = [];
      }
      this._local[channel].push(callback);
    },
    send: function(channel, data) {
      var listeners = this._local[channel];
      if (listeners) {
        listeners.forEach(function(cb) { cb(data); });
      }
    }
  };
})();
