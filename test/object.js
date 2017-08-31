const test = require('ava'); // eslint-disable-line

process.env.PYTHONPATH = __dirname;

const py = require('..');

const pyModule = py.require('test');

test('get', (t) => {
  t.is(pyModule.setable, 'foo');
});

test('set', (t) => {
  pyModule.setable = 'bar';
  t.is(pyModule.setable, 'bar');
});

test('delete', (t) => {
  t.true(py.builtins.hasattr(pyModule, 'deletable'));
  delete pyModule.deletable;
  t.false(py.builtins.hasattr(pyModule, 'deletable'));
});
