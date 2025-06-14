const path = require('path');

module.exports = {
  entry: './src/index.js',
  output: {
    path: path.resolve(__dirname, '..', 'web'),
    filename: 'easm.js',
  },
};
