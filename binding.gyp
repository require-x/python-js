{
  "targets": [
    {
      "target_name": "python",
      "sources": [ "src/main.cc", "src/resolver.cc" ],
      'conditions': [
        ['OS=="win"', {
          'include_dirs': ['<!(python pythonpath.py include)'],
          'libraries': ['<!(python pythonpath.py lib)']
        }, {
          'cflags': ['<!@(python3-config --cflags)'],
          'libraries': ['<!@(python3-config --ldflags)'],
        }]
      ]
    }
  ]
}