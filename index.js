process.env.PYTHONPATH = process.env.PYTHONPATH || process.cwd();

const util = require('util');
const { init, cleanup, version, load, call, toString, objectGet, objectSet, objectHas, objectDel, mappingGet, mappingSet, mappingHas, mappingKeys, mappingDel, sequenceGet, sequenceSet, sequenceSize, sequenceDel, pyEval, pyExec } = require('bindings')('python');

class PyError extends Error {
  constructor(jsError, pyTraceback) {
    super(jsError.message);

    this.name = 'PyError';

    if (pyTraceback.length > 1) {
      this.message = jsError.message;
      this.stack = `${jsError.stack}\n\n${'-'.repeat(process.stderr.columns)}\n\n${pyTraceback.join('')}`;
    }
  }
}

class Kwargs extends Object {
  constructor(obj) {
    super();
    Object.assign(this, obj);
  }
}

class PyObject {
  static [Symbol.hasInstance](instance) {
    return instance.__require_x__ !== undefined;
  }
}

init(PyError);

process.on('exit', cleanup);

function objProxy(obj) {
  obj[util.inspect.custom] = () => `[PyObject: ${toString(obj.__require_x__.address)}]`;
  obj[Symbol.toStringTag] = () => `[PyObject: ${toString(obj.__require_x__.address)}]`;

  const proxy = new Proxy(obj, {
    get(target, key) {
      if (target instanceof Array && key === 'length') return sequenceSize(target.__require_x__.address);

      if (key in target) return target[key];
      if (typeof key === 'symbol') return undefined;

      let val;
      if (objectHas(target.__require_x__.address, key)) {
        val = objectGet(target.__require_x__.address, key);
      } else if (target instanceof Array &&
        !Number.isNaN(Number.parseInt(key, 10)) &&
        Number.parseInt(key, 10) < sequenceSize(target.__require_x__.address)
      ) {
        val = sequenceGet(target.__require_x__.address, Number.parseInt(key, 10));
      } else if (key.startsWith('$') && mappingHas(target.__require_x__.address, key.substr(1))) {
        val = mappingGet(target.__require_x__.address, key.substr(1));
      }

      if (typeof val === 'function' || val instanceof Object || val instanceof Array) return objProxy(val);
      return val;
    },

    set(target, key, val) {
      if (target instanceof Array && !Number.isNaN(Number.parseInt(key, 10))) {
        return sequenceSet(target.__require_x__.address, Number.parseInt(key, 10), val);
      } else if (key.startsWith('$')) {
        return mappingSet(target.__require_x__.address, key.substr(1), val);
      }

      return objectSet(target.__require_x__.address, key, val);
    },

    has(target, key) {
      if (target instanceof Array && !Number.isNaN(Number.parseInt(key, 10))) {
        return key.toString() in Object.keys(proxy);
      } else if (key.startsWith('$')) {
        return mappingHas(target.__require_x__.address, key.substr(1));
      }

      return objectHas(target.__require_x__.address, key);
    },

    deleteProperty(target, key) {
      if (objectHas(target.__require_x__.address, key)) {
        return objectDel(target.__require_x__.address, key);
      } else if (target instanceof Array &&
        !Number.isNaN(Number.parseInt(key, 10)) &&
        Number.parseInt(key, 10) < sequenceSize(target.__require_x__.address)
      ) {
        sequenceDel(target.__require_x__.address, Number.parseInt(key, 10));
      } else if (key.startsWith('$') && mappingHas(target.__require_x__.address, key.substr(1))) {
        return mappingDel(target.__require_x__.address, key.substr(1));
      }

      return true;
    },

    apply(target, _, args) {
      const v = call(target.__require_x__.address, ...args);
      if (v instanceof Object || v instanceof Array) return objProxy(v);
      return v;
    },

    ownKeys(target) {
      if (target instanceof Array) {
        const keys = new Array(sequenceSize(target.__require_x__.address));

        for (let i = 0; i < keys.length; i += 1) {
          keys[i] = i.toString();
        }

        return keys.concat(['length']);
      }

      return mappingKeys(target.__require_x__.address);
    },

    getOwnPropertyDescriptor(target, key) {
      if ((target instanceof Array && key === 'length') || key.startsWith('__')) {
        return {
          enumerable: false,
          configurable: false,
        };
      }

      return {
        enumerable: true,
        configurable: true,
      };
    },
  });

  return proxy;
}

module.exports = {
  version: version(),
  Kwargs,
  Error: PyError,
  Object: PyObject,

  format(str, ...args) {
    let final = str[0];
    const finalArgs = [];

    let i = 0;
    let n = 0;
    for (const arg of args) {
      if (arg.__require_x__) {
        final += `__require_x_module__${arg.__require_x__.name}`;
      } else {
        final += `__require_x_argument__${n}`;
        finalArgs.push(arg);
        n += 1;
      }

      final += `${str[i + 1]}`;
      i += 1;
    }

    return [final, ...finalArgs];
  },

  eval(str) {
    const val = typeof str === 'string' ? pyEval(str) : pyEval(...str);

    if (val instanceof Object || val instanceof Array) return objProxy(val);

    return val;
  },

  exec(str) {
    const val = typeof str === 'string' ? pyExec(str) : pyExec(...str);

    if (val instanceof Object || val instanceof Array) return objProxy(val);

    return val;
  },

  kwargs(obj) {
    return new Kwargs(obj);
  },

  require(module) {
    const address = load(module);

    return objProxy({
      __require_x__: {
        name: module,
        address,
      },
    });
  },
};

if (version()[0] === 3) {
  module.exports.builtins = module.exports.require('builtins');
} else {
  module.exports.builtins = module.exports.require('__builtin__');
}
