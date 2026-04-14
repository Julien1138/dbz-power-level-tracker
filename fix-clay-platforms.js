// Adds gabbro and flint platform support to pebble-clay after npm install.
// These platforms were released after pebble-clay was last maintained;
// the library code is platform-agnostic so copying existing platform dirs works.
const fs = require('fs');
const path = require('path');

const base = path.join(__dirname, 'node_modules', 'pebble-clay', 'dist');
const patches = [
  { platform: 'gabbro', from: 'emery'  },  // color platform
  { platform: 'flint',  from: 'diorite' }, // b&w platform
];

for (const { platform, from } of patches) {
  for (const subdir of ['binaries', path.join('include', 'pebble-clay')]) {
    const src  = path.join(base, subdir, from);
    const dest = path.join(base, subdir, platform);
    if (!fs.existsSync(dest)) {
      fs.cpSync(src, dest, { recursive: true });
      console.log(`pebble-clay: added ${platform} to ${subdir}`);
    }
  }
}
