const test = require('ava'); // eslint-disable-line

process.env.PYTHONPATH = __dirname;

const py = require('..');

const pyModule = py.require('test');

test('no args', (t) => {
  t.is(pyModule.no_args(), 'foo');
});

test('string', (t) => {
  t.is(pyModule.strfunc('foo'), 'bar');
});

test('null', (t) => {
  t.is(pyModule.nullfunc(null), null);
});

test('bool', (t) => {
  t.is(pyModule.boolfunc(true), true);
});

test('num', (t) => {
  t.is(pyModule.numfunc(33.3), 1);
});

test('array', (t) => {
  t.true(pyModule.arrfunc([1, 2, 3]) instanceof py.Object);
});

test('tuple', (t) => {
  t.true(pyModule.tuplefunc(py.builtins.tuple([1, 2, 3])) instanceof py.Object);
});

test('object', (t) => {
  t.true(pyModule.objfunc({ foo: 'bar' }) instanceof py.Object);
});
