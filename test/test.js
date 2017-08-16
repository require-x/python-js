/* eslint-env node, jest */

process.env.PYTHONPATH = __dirname;

const py = require('..');

const pyModule = py.require('test');
const variables = [
  {
    name: 'str',
    displayName: 'String',
    value: 'foo',
    edit: 'bar',
  },
  {
    name: 'arr',
    displayName: 'Array',
    value: [1, 2, [3, 4]],
    edit: [1, 2, [3, '4'], 5],
  },
  {
    name: 'tuple',
    displayName: 'Tuple',
    value: new py.Tuple(1, 2, new py.Tuple(3, 4)),
    edit: new py.Tuple(1, 2, new py.Tuple(3, '4'), 5),
  },
  {
    name: 'obj',
    displayName: 'Object',
    value: { foo: { bar: 'baz' } },
    edit: { foo: { bar: 'baz?!' }, bar: 1 },
  },
  {
    name: 'null',
    displayName: 'Null',
    value: null,
  },
  {
    name: 'true',
    displayName: 'True',
    value: true,
  },
  {
    name: 'false',
    displayName: 'False',
    value: false,
  },
  {
    name: 'num',
    displayName: 'Number',
    value: 1,
    edit: 2,
    float: true,
  },
  {
    name: 'float',
    displayName: 'Float',
    value: 1.5,
    edit: 2.5,
    float: true,
  },
];

test('load', () => {
  expect(Object.keys(pyModule).length).toBeTruthy();
});

test('Object.keys()', () => {
  expect(Object.keys(pyModule).sort()).toEqual([
    'obj',
    'func',
    'false',
    'arr',
    'tuple',
    'null',
    'num',
    'float',
    'deletable',
    'true',
    'no_args',
    'str',
  ].sort());
});

test('delete', () => {
  expect('deletable' in pyModule).toEqual(true);
  delete pyModule.deletable;
  expect('deletable' in pyModule).toEqual(false);
});

describe('get', () => {
  for (const variable of variables) {
    test(variable.displayName, () => { // eslint-disable-line
      expect(pyModule[variable.name]).toEqual(variable.value);
    });
  }
});

describe('set', () => {
  for (const variable of variables) {
    if (!variable.edit) continue;
    test(variable.displayName, () => { // eslint-disable-line
      pyModule[variable.name] = variable.edit;
      expect(pyModule[variable.name]).toEqual(variable.edit);
    });
  }
});

describe('has', () => {
  for (const variable of [{ name: 'str', displayName: 'Existing' }, { name: 'nonexistent', displayName: 'Nonexistent' }]) {
    test(variable.displayName, () => {
      if (variable.name !== 'nonexistent') {
        expect(variable.name in pyModule).toEqual(true);
      } else {
        expect(variable.name in pyModule).toEqual(false);
      }
    });
  }
});

describe('func', () => {
  test('No Arguments', () => {
    expect(pyModule.no_args()).toEqual('foo');
  });

  for (const variable of variables) {
    test(variable.displayName, () => { // eslint-disable-line
      expect(pyModule.func(variable.value)).toEqual(variable.value);
    });
  }
});
