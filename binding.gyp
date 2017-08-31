{
  'variables': {
    'python_version': '3',
  },
  "targets": [
    {
      "target_name": "python",
      "sources": [ "src/main.cc", "src/resolver.cc", "src/protocols/mapping.cc", "src/protocols/object.cc", "src/protocols/sequence.cc" ],
      'conditions': [
        ['OS=="win"', {
          'include_dirs': ['<!(python pythonpath.py include)'],
          'libraries': ['<!(python pythonpath.py lib)']
        }, {
          'xcode_settings': {
            'OTHER_CFLAGS': ['<!@(./python-config.sh python<(python_version)-config --cflags)'],
          },
          'cflags': ['<!@(./python-config.sh python<(python_version)-config --cflags)'],
          'libraries': ['<!@(./python-config.sh python<(python_version)-config --ldflags)'],
        }]
      ]
    }
  ]
}