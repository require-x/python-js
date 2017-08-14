process.env.PYTHONPATH = process.env.PYTHONPATH || process.cwd();

const { load, get, set, has, keys, del, call } = require('bindings')('python.node');

function objProxy(obj, id, name, parent) {
  return new Proxy(obj, {
    get(target, key) {
      const val = target[key];

      if (val instanceof Object || val instanceof Array) {
        return objProxy(val, id, key, this);
      }

      return val;
    },

    set(target, key, val) {
      target[key] = val;

      if (parent) parent[name] = this;
      else set(id, name, target);

      return val;
    },
  });
}

module.exports = (module) => {
  const id = load(module);

  return new Proxy({}, {
    get(_, key) {
      const val = get(id, key);

      if (typeof val === 'function') return (...args) => call(id, key, ...args);
      else if (val instanceof Object || val instanceof Array) {
        return objProxy(val, id, key);
      }

      return val;
    },

    set(_, key, val) {
      return set(id, key, val);
    },

    has(_, key) {
      return has(id, key);
    },

    ownKeys() {
      return keys(id);
    },

    getOwnPropertyDescriptor(_, key) {
      if (key.startsWith('__')) {
        return {
          enumerable: false,
          configurable: true,
        };
      }

      return {
        enumerable: true,
        configurable: true,
      };
    },

    deleteProperty(_, key) {
      return del(id, key);
    },
  });
};
