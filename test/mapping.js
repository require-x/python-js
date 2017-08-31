const test = require('ava'); // eslint-disable-line

process.env.PYTHONPATH = __dirname;

const py = require('..');

const pyModule = py.require('test');

test('Object.keys()', (t) => {
  t.deepEqual(Object.keys(pyModule.mapping).sort(), ['deletable', 'setable'].sort());
});

test('get', (t) => {
  t.is(pyModule.mapping.$setable, 'foo');
});

test('set', (t) => {
  pyModule.mapping.$setable = 'bar';
  t.is(pyModule.mapping.$setable, 'bar');
});

test('delete', (t) => {
  t.true('$deletable' in pyModule.mapping);
  delete pyModule.mapping.$deletable;
  t.false('$deletable' in pyModule.mapping);
});

test('has', (t) => {
  t.true('$setable' in pyModule.mapping);
});
