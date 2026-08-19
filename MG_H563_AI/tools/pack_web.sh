#!/bin/sh
set -eu
NODE_BIN=${NODE_BIN:-node}
"$NODE_BIN" Mongoose/pack.js \
  Web/index.html:index.html \
  Web/app.css:app.css \
  Web/app.js:app.js \
  "Logotype/PR Tech Talk 1024_549.jpg:logo.jpg" > App/Src/packed_fs.c

