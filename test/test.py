string = "foo"
tupl = (1, 2, (3, 4))
null = None
boolean = False
num = 1

setable = "foo"
deletable = None

mapping = {
  "setable": "foo",
  "deletable": None
}

sequence = ["foo", None]

def no_args():
  return "foo"

def strfunc(string):
  assert type(string) is str
  return "bar"

def arrfunc(arr):
  assert type(arr) is list
  return [1, 2, 3, 4, 5]

def tuplefunc(tupl):
  assert type (tupl) is tuple
  return (1, 2, 3, 4, 5)

def objfunc(obj):
  assert type(obj) is dict
  return { "foo": 1 }

def nullfunc(null):
  assert null is None
  return None

def boolfunc(boolean):
  assert type(boolean) is bool
  return True

def numfunc(num):
  assert type(num) is float
  return 1