const childProcess = require('child_process');

try {
  childProcess.execSync(`${process.argv[2]} ${process.argv[3]}`, { stdio: ['ignore', process.stdout, 'ignore'] });
} catch (err) {
  childProcess.execSync(`python-config ${process.argv[3]}`, { stdio: ['ignore', process.stdout, 'ignore'] });
}
