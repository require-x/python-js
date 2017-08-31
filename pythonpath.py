import os
import sys

if sys.argv[1] == 'include':
  print(os.path.dirname(sys.executable) + '\\include')

else:
  print(os.path.dirname(sys.executable) + '\\libs\\python%d%d.lib' % (sys.version_info[0], sys.version_info[1]))
