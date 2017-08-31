const test = require('ava'); // eslint-disable-line

process.env.PYTHONPATH = __dirname;

const py = require('..');

const pyModule = py.require('test');

test('string', (t) => {
  t.is(pyModule.string, 'foo');
  pyModule.string = 'bar';
  t.is(pyModule.string, 'bar');
});

test('null', (t) => {
  t.is(pyModule.null, null);
});

test('boolean', (t) => {
  t.false(pyModule.boolean);
  pyModule.boolean = true;
  t.true(pyModule.boolean);
});

test('number', (t) => {
  t.is(pyModule.num, 1);
  pyModule.num = 33.3;
  t.is(pyModule.num, 33.3);
});
