# python-js
[![Build status](https://ci.appveyor.com/api/projects/status/ijhto5tj0im9yybo?svg=true)](https://ci.appveyor.com/project/Hackzzila/python-js) [![Build Status](https://travis-ci.org/require-x/python-js.svg?branch=master)](https://travis-ci.org/require-x/python-js)

## Getting Started
In this example we will be using the Python library [Requests](http://docs.python-requests.org/) in JavaScript.  
**Note:** Using this example in the real world is **not** a good idea. Just use a JavaScript HTTP library ([superagent](http://visionmedia.github.io/superagent/), [request](https://www.npmjs.com/package/request)) instead.

### Installing 

#### Installing Python
The following python versions are supported: 2.6, 2.7, 3.3, 3.4, 3.5, 3.6.  
**Python 2 is required to build regardless of your target python version.**

##### Debian/Ubuntu
```bash
sudo apt-get install python python3 python3-dev
```

##### Windows
Download the installer from [python.org](https://www.python.org/)

##### macOS
```bash
brew install python3
```

#### Installing require-py
If you have multiple python versions on your system, you may want to set a target python version when you install. This can be done by passing something like `--python_version=3.3` to `npm install`.

```bash
npm install @require-x/python # Uses python 3 if installed, falling back to python 2.
npm install @require-x/python --python_version=2
npm install @require-x/python --python_version=3.3
```

#### Installing Requests
##### Windows
```
pip install requests
```

##### Linux or macOS
```
pip3 install requests
```

### Coding
Lets start coding.

First we need to import the library.
```js
const py = require('@require-x/python');
```

Then we need to import requests.
```js
const requests = py.require('requests');
```

Then we can make a new request.
```js
const r = requests.post('http://httpbin.org/post', py.kwargs({ data: JSON.stringify(["Hello, world!"]) }));
```

Lets break this down.  
We are calling the method `requests.post` with a url as the first argument and then passing kwargs through `py.kwargs()`.  
The method returns a `py.Object` object which can be manipulated in several ways.

#### Getting an Attribute
To get an attribute, you can use `o.attr_name` (the same as you do in Python).

```js
// Get the status code. This is equal to "r.status_code" in Python.
console.log(r.status_code); // Prints: 200
```

#### Getting a Value from a Dict
To get a value from a dict, you can use `o.$key`, this is equal to `o["key"]` in Python.

```js
// Get the content-type header received from the server. This is equal to "r.headers["content-type"]" in Python
console.log(r.headers['$content-type']); // Prints: application/json, 
```

#### Getting a Value from a List or Tuple
To get a value from a list or tuple, you can use `o[key]` (the same as you do in Python).

```js
// Get the first value of the JSON array we sent that the server sent back in it's response. This is equal to "r.json().json[0]` in Python.
console.log(r.json().$json[0]) // Prints: Hello, world!
```

#### Full Code
```js
const py = require('@require-x/python'); // Import the library

const requests = py.require('requests'); // Import requests

const r = requests.post('http://httpbin.org/post', py.kwargs({ data: JSON.stringify(["Hello, world!"]) })); // Make a request

console.log(r.status_code); // Log the status code
console.log(r.headers['$content-type']); // Log the content type
console.log(r.json().$json[0]); // Log the first value of the JSON array we sent that the server sent back in it's response
```
