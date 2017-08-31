const test = require('ava'); // eslint-disable-line

process.env.PYTHONPATH = __dirname;

const py = require('..');

const pyModule = py.require('test');

test('Object.keys()', (t) => {
  t.deepEqual(Object.keys(pyModule.sequence), ['0', '1']);
});

test('length', (t) => {
  t.is(pyModule.sequence.length, 2);
});

test('get', (t) => {
  t.is(pyModule.sequence[0], 'foo');
});

test('set', (t) => {
  pyModule.sequence[0] = 'bar';
  t.is(pyModule.sequence[0], 'bar');
});

test('delete', (t) => {
  t.true(1 in pyModule.sequence);
  delete pyModule.sequence[1];
  t.false(1 in pyModule.sequence);
});

test('has', (t) => {
  t.true(0 in pyModule.sequence);
});
